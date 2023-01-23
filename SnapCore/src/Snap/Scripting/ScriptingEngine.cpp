#include "SnapPCH.h"
#include "ScriptingEngine.h"

#include <Snap/Core/asserts.h>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

#include <FileWatch.h>

#include "ScriptGlue.h"
#include <Snap/Core/UUID.h>

#include <Snap/Scene/Comps/Components.h>

#include <mono/metadata/tabledefs.h>

#include <Snap/Core/Buffer.h>

namespace Scripting
{
	namespace Utils
	{
		// TODO:: Change this to file system
		static SnapEngine::Buffer ReadBytes(const std::string& filepath)
		{
			std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

			if (!stream)
			{
				// Failed to open the file
				return SnapEngine::Buffer();
			}

			std::streampos end = stream.tellg();
			stream.seekg(0, std::ios::beg);
			uint32_t size = end - stream.tellg();

			if (size == 0)
			{
				// File is empty
				return SnapEngine::Buffer();
			}

			SnapEngine::Buffer buffer(size);
			stream.read(buffer.As<char*>(), size);
			stream.close();


			return buffer;
		}

		static MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath)
		{
			SnapEngine::Buffer assemblybuffer = ReadBytes(assemblyPath);

			// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(assemblybuffer.As<char*>(), assemblybuffer.m_Size, 1, &status, 0);

			if (status != MONO_IMAGE_OK)
			{
				const char* errorMessage = mono_image_strerror(status);
				// Log some error message using the errorMessage data
				return nullptr;
			}

			MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
			mono_image_close(image);

			// Don't forget to free the assembly buffer
			assemblybuffer.Release();

			return assembly;
		}

		static void PrintAssemblyTypes(MonoAssembly* Assembly, MonoImage* Image)
		{
			MonoImage* image = mono_assembly_get_image(Assembly);
			const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
			int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

			for (int32_t i = 0; i < numTypes; i++)
			{
				uint32_t cols[MONO_TYPEDEF_SIZE];
				mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

				const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
				const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

				MonoClass* mono_class = mono_class_from_name(Image, nameSpace, name);

				SNAP_DEBUG("NmaeSpce-> {}, ClassName-> {}", nameSpace, name);
			}
		}
	}








	struct ScriptEngineData
	{
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
		MonoImage* CoreAssemblyImage = nullptr;

		// Application
		MonoAssembly* AppAssembly = nullptr;
		MonoImage* AppAssemblyImage = nullptr;

		std::unordered_map<std::string, SnapEngine::SnapPtr<ScriptClass>> m_EntityClasses;
		std::unordered_map<SnapEngine::UUID, SnapEngine::SnapPtr<ScriptInstance>> m_EntityScriptInstances;

		std::unordered_map<SnapEngine::UUID, ScriptFieldMap> m_EntityScriptFiledsInstances;

		SnapEngine::Scene* SceneContext = nullptr;

		std::string m_AssemblyFilePath;
		std::string m_AppAssemblyFilePath;

		MonoClass* EntityClass;
		MonoMethod* EntityConstructor;

		SnapEngine::SnapUniquePtr<filewatch::FileWatch<std::string>> AppAssemblyFileWatcher;
		bool AssemblyReloadPending = false;
		bool IsAppAssemblyReloadingPending = false;
	};


	static ScriptEngineData* s_Data = nullptr;

	static ScriptFieldDataType MonoClassTypeNameToScriptFieldDataType(std::string TypeName)
	{
		if (TypeName == "Int32")
			return ScriptFieldDataType::Int;
		else if (TypeName == "Single")
			return ScriptFieldDataType::Float;
		else if (TypeName == "Double")
			return ScriptFieldDataType::Double;
		else if (TypeName == "Boolean")
			return ScriptFieldDataType::Bool;
		else if (TypeName == "UInt64")
			return ScriptFieldDataType::UInt64;
		else if (TypeName == "vec2")
			return ScriptFieldDataType::Vec2;
		else if (TypeName == "vec3")
			return ScriptFieldDataType::Vec3;
		else if (TypeName == "vec4")
			return ScriptFieldDataType::Vec4;
		else if (TypeName == "Entity")
			return ScriptFieldDataType::Entity;
		else
			return ScriptFieldDataType::Invalid;
	}

	
	static void InitMono()
	{
		mono_set_assemblies_path("mono/lib");

		MonoDomain* rootdomain = mono_jit_init("SnapEngineJitRunTime");
		SNAP_ASSERT_MSG(rootdomain, "Mono Jit Initialization Failed!");
		s_Data->RootDomain = rootdomain;

		// Load App Assembly
		s_Data->AppDomain = mono_domain_create_appdomain("SnapEngineScriptRunTime", nullptr);
		mono_domain_set(s_Data->AppDomain, true);
	}

	static void ShutdownMonon()
	{
		// TODO:: ComeBack To This

		mono_domain_set(mono_get_root_domain(), false);

		mono_domain_unload(s_Data->AppDomain);
		s_Data->AppDomain = nullptr;

		mono_jit_cleanup(s_Data->RootDomain);
		s_Data->RootDomain = nullptr;
		/*
		s_Data->AppDomain = nullptr;
		s_Data->RootDomain = nullptr;
		*/
	}

	void ScriptingEngine::LoadAppAssembly(const std::filesystem::path& DllFilePath)
	{
		s_Data->AppAssembly = Utils::LoadCSharpAssembly(DllFilePath.string().c_str());
		s_Data->AppAssemblyImage = mono_assembly_get_image(s_Data->AppAssembly);
	}

	void ScriptingEngine::LoadAssembly(const std::filesystem::path& DllFilePath)
	{
		s_Data->CoreAssembly = Utils::LoadCSharpAssembly(DllFilePath.string().c_str());
		s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
		
		//Utils::PrintAssemblyTypes(s_Data->CoreAssembly, s_Data->CoreAssemblyImage);
	}

	void ScriptingEngine::LoadCSharpAssemblyClasses()
	{
		s_Data->m_EntityClasses.clear();

		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(s_Data->AppAssemblyImage, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

		
		s_Data->EntityClass = mono_class_from_name(s_Data->CoreAssemblyImage, "SnapEngine", "Entity");
		s_Data->EntityConstructor = mono_class_get_method_from_name(s_Data->EntityClass, ".ctor", 1);

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAME]);

			std::string fullname;
			if (strlen(nameSpace) != 0)
				fullname = fmt::format("{}.{}", nameSpace, name);
			else
				fullname = name;

			MonoClass* mono_class = mono_class_from_name(s_Data->AppAssemblyImage, nameSpace, name);
			
			bool IsEntitySubClass = mono_class_is_subclass_of(mono_class, s_Data->EntityClass, false);
			if (IsEntitySubClass)
			{
				SNAP_DEBUG("NmaeSpce>> {}, ClassName>> {}", nameSpace, name);
				
				// Add Script Class
				auto& Script_Class = SnapEngine::CreatSnapPtr<ScriptClass>(nameSpace, name, false);
				s_Data->m_EntityClasses[fullname] = Script_Class;

				// Get Fileds
				int FieldsCount = mono_class_num_fields(mono_class);
				void* iterator = nullptr;
				
				for (size_t i = 0; i < FieldsCount; i++)
				{
					MonoClassField* Field = mono_class_get_fields(mono_class, &iterator);
					if(!Field) continue;
					
					// Example if Field >> public float speed = 10.0f;
					uint32_t flags = mono_field_get_flags(Field); // puplic, static
					MonoType* FieldType = mono_field_get_type(Field); // int, float (MonoType*)
					const char* FieldTypeName = mono_type_get_name(FieldType); // int, float (const char*
					const char* FieldName = mono_field_get_name(Field); // speed

					// C# TypeName
					std::string TypeName = FieldTypeName;
					int index = TypeName.find_last_of(".");
					int last = TypeName.size();
					TypeName = TypeName.substr(index + 1, last - index);


					// Save ScriptClassFields Names And DataTypes If Public
					if (flags & FIELD_ATTRIBUTE_PUBLIC)
					{
						ScriptFieldDataType DataType = MonoClassTypeNameToScriptFieldDataType(TypeName);
						Script_Class->m_ScriptFields[FieldName] = { FieldName, DataType, Field };
						
						SNAP_WARN("public {0} {1}", TypeName, FieldName);
					}
					else
						SNAP_WARN("{0} {1}", TypeName, FieldName);
				}
			}
		}
	}

	std::unordered_map<std::string, SnapEngine::SnapPtr<ScriptClass>> ScriptingEngine::GetEntityClassesMap()
	{
		return s_Data->m_EntityClasses;
	}

	bool ScriptingEngine::IsEntityClassExist(std::string ClassName)
	{
		return s_Data->m_EntityClasses.find(ClassName) != s_Data->m_EntityClasses.end();
	}

	void ScriptingEngine::OnCreatEntity(SnapEngine::Entity e)
	{
		const auto& sc = e.GetComponent<SnapEngine::ScriptComponent>();
		auto& uuid = e.GetComponent<SnapEngine::IDComponent>().ID;

		if (IsEntityClassExist(sc.ClassName))
		{
			// Get Script Class From Component ClassName
			SnapEngine::SnapPtr<ScriptClass> script_class = s_Data->m_EntityClasses[sc.ClassName];
			// Creat Script Instance From ScriptClass
			SnapEngine::SnapPtr<ScriptInstance> instance = SnapEngine::CreatSnapPtr<ScriptInstance>(script_class, uuid);
			// Add Instance to Map with uuid keyvalue
			s_Data->m_EntityScriptInstances[uuid] = instance;
			// Set Fields From Editor (FieldsInsatnces->m_Data) To This Instance
			if (s_Data->m_EntityScriptFiledsInstances.find(uuid) != s_Data->m_EntityScriptFiledsInstances.end())
			{
				auto& ScriptFiledsMap = ScriptingEngine::GetScriptFieldMap(e);

				for (auto& [name, FiledInstance] : ScriptFiledsMap)
				{
					instance->SetFieldValue(name, FiledInstance.m_Data);
				}
			}
			// run C# OnCreat Method From Instance With This UUID
			instance->InvokeOnCreatMethod();
		}
	}

	void ScriptingEngine::OnUpdateEntity(SnapEngine::Entity e, SnapEngine::TimeStep Time)
	{
		const auto& sc = e.GetComponent<SnapEngine::ScriptComponent>();
		if (IsEntityClassExist(sc.ClassName))
		{
			auto& uuid = e.GetComponent<SnapEngine::IDComponent>().ID;

			// run C# OnUpdate Method From Instance With This UUID
			s_Data->m_EntityScriptInstances[uuid]->InvokeOnUpdateMethod(Time);
		}
	}

	void ScriptingEngine::OnRunTimeStart(SnapEngine::Scene* Scene)
	{
		s_Data->SceneContext = Scene;
	}

	void ScriptingEngine::OnRunTimeStop()
	{
		//s_Data->m_EntityScriptInstances.clear();
		//s_Data->SceneContext = nullptr;
	}

	SnapEngine::Scene* ScriptingEngine::GetSceneContext()
	{
		return s_Data->SceneContext;
	}

	MonoImage* ScriptingEngine::GetCoreAssemblyImage()
	{
		return s_Data->CoreAssemblyImage;
	}

	static void OnAppAssemblyFileSystemEvent(const std::string& path, const filewatch::Event change_type)
	{
		if (!s_Data->AssemblyReloadPending && change_type == filewatch::Event::modified)
		{
			s_Data->AssemblyReloadPending = true;
			s_Data->IsAppAssemblyReloadingPending = true;
		}
	}

	void ScriptingEngine::Init()
	{
		s_Data = new ScriptEngineData();
		s_Data->m_AssemblyFilePath = "Resources/Scripts/Snap-ScriptCore.dll";
		s_Data->m_AppAssemblyFilePath = "Resources/Scripts/SandBoxGame.dll";
		

		InitMono(); // Setup MonoDomain(Root & App), MonoAssembly(CoreAssembly)
		
		ScriptGlue::RegisterGlue(); // Setup C# Internal Calls
		
		LoadAssembly(s_Data->m_AssemblyFilePath.c_str()); // Load dll file
		LoadAppAssembly(s_Data->m_AppAssemblyFilePath.c_str());
		LoadCSharpAssemblyClasses(); // Load Classes From App Assembly Image
		
		ScriptGlue::RegisterComponents();

		//s_Data->EntityClass = ScriptClass("SnapEngine", "Entity", true);

		s_Data->AppAssemblyFileWatcher = SnapEngine::CreatSnapUiquePtr<filewatch::FileWatch<std::string>>
		(
			"Resources/Scripts", OnAppAssemblyFileSystemEvent
		);
	}

	void ScriptingEngine::ShutDown()
	{
		ShutdownMonon();
		delete s_Data;
		s_Data = nullptr;
	}




	MonoObject* ScriptingEngine::InstantiateClass(MonoClass* monoClass)
	{
		// Creat an Object and Call Constructor
		MonoObject* instance = mono_object_new(s_Data->AppDomain, monoClass);
		mono_runtime_object_init(instance);
		return instance;
	}

	SnapEngine::SnapPtr<ScriptClass> ScriptingEngine::GetScriptClass(const std::string& name)
	{
		if (s_Data->m_EntityClasses.find(name) == s_Data->m_EntityClasses.end())
			return nullptr;

		return s_Data->m_EntityClasses.at(name);
	}

	SnapEngine::SnapPtr<ScriptInstance> ScriptingEngine::GetScriptInstance(SnapEngine::UUID uuid)
	{
		return s_Data->m_EntityScriptInstances[uuid]; 
	}

	ScriptFieldMap& ScriptingEngine::GetScriptFieldMap(SnapEngine::Entity entity)
	{
		SNAP_ASSERT_MSG(entity, "entity is null!");

		SnapEngine::UUID entityID = entity.GetComponent<SnapEngine::IDComponent>().ID;

		//if(s_Data->m_EntityScriptFiledsInstances.find(entityID) != s_Data->m_EntityScriptFiledsInstances.end())
		// Add Key And Return Value Map
		return s_Data->m_EntityScriptFiledsInstances[entityID];
	}

	MonoObject* ScriptingEngine::GetManagedObject(SnapEngine::UUID uuid)
	{
		if (s_Data->m_EntityScriptInstances.find(uuid) != s_Data->m_EntityScriptInstances.end())
			return s_Data->m_EntityScriptInstances.at(uuid)->m_Instance;
		return nullptr;
	}

	void ScriptingEngine::ReloadAssemby()
	{
		s_Data->m_EntityClasses.clear();
		s_Data->m_EntityScriptFiledsInstances.clear();
		s_Data->m_EntityScriptInstances.clear();

		mono_domain_set(mono_get_root_domain(), false);
		mono_domain_unload(s_Data->AppDomain);
		
		// ReCreat AppDomain
		s_Data->AppDomain = mono_domain_create_appdomain("SnapEngineScriptRunTime", nullptr);
		mono_domain_set(s_Data->AppDomain, true);

		// Reload Assembly Dll File And Retrieve Its Classes and public fields
		// ReCreat CoreAssembly & CoreAssemblyImage
		LoadAssembly(s_Data->m_AssemblyFilePath);
		LoadAppAssembly(s_Data->m_AppAssemblyFilePath);
		LoadCSharpAssemblyClasses(); // Load Classes From App AssemblyImage

		ScriptGlue::RegisterComponents(); // Reregister Components cause CoreAssemblyImage Changed

		s_Data->AssemblyReloadPending = false;
		s_Data->IsAppAssemblyReloadingPending = false;
	}

	bool ScriptingEngine::IsReloadAppAssemblyPending()
	{
		return s_Data->IsAppAssemblyReloadingPending;
	}





	ScriptClass::ScriptClass(const std::string& nameSpace, const std::string& name, bool IsCore)
		: m_NameSpace(nameSpace), mName(name)
	{
		m_MonoClass = mono_class_from_name(IsCore ? s_Data->CoreAssemblyImage : s_Data->AppAssemblyImage, nameSpace.c_str(), name.c_str());
	}

	MonoObject* ScriptClass::Instantiate()
	{
		return ScriptingEngine::InstantiateClass(m_MonoClass);
	}

	MonoMethod* ScriptClass::GetMethod(const std::string& name, int paramCount)
	{
		return mono_class_get_method_from_name(m_MonoClass, name.c_str(), paramCount);
	}


	MonoObject* ScriptClass::InvokeMethod(MonoMethod* method, MonoObject* instance, void** params)
	{
		return mono_runtime_invoke(method, instance, params, nullptr);
	}

	ScriptInstance::ScriptInstance(const SnapEngine::SnapPtr<ScriptClass>& ScriptClass, SnapEngine::UUID EntityID)
		: m_ScriptClass(ScriptClass)
	{
		m_Instance = ScriptingEngine::InstantiateClass(*ScriptClass);

		m_OnCreatMethod = ScriptClass->GetMethod("OnCreat", 0);
		m_OnUpdateMethod = ScriptClass->GetMethod("OnUpdate", 1);

		{// Run Entity Constructor
			void* param = &EntityID;
			ScriptClass->InvokeMethod(s_Data->EntityConstructor, m_Instance, &param);
		}
	}

	void ScriptInstance::InvokeOnCreatMethod()
	{
		if(m_OnCreatMethod)
			m_ScriptClass->InvokeMethod(m_OnCreatMethod, m_Instance, nullptr);
	}

	void ScriptInstance::InvokeOnUpdateMethod(float TimeStep)
	{
		if (m_OnUpdateMethod)
		{
			void* param = &TimeStep;
			m_ScriptClass->InvokeMethod(m_OnUpdateMethod, m_Instance, &param);
		}
	}

	bool ScriptInstance::GetFieldValueInternal(const std::string& FieldName, void* Buffer)
	{
		auto& FieldsMap = this->m_ScriptClass->GetFieldsMap();
		auto& it = FieldsMap.find(FieldName);
		if (it == FieldsMap.end())
			return false;

		mono_field_get_value(this->m_Instance, it->second.m_MonoField, Buffer);

		return true;
	}
	bool ScriptInstance::SetFieldValueInternal(const std::string& FieldName, const void* Buffer)
	{
		auto& FieldsMap = this->m_ScriptClass->GetFieldsMap();
		auto& it = FieldsMap.find(FieldName);
		if (it == FieldsMap.end())
			return false;

		mono_field_set_value(this->m_Instance, it->second.m_MonoField, (void*)Buffer);
		return true;
	}
}