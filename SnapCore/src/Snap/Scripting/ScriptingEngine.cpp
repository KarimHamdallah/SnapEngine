#include "SnapPCH.h"
#include "ScriptingEngine.h"

#include <Snap/Core/asserts.h>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

#include "ScriptGlue.h"

namespace Scripting
{
	namespace Utils
	{
		// TODO:: Change this to file system
		static char* ReadBytes(const std::string& filepath, uint32_t* outSize)
		{
			std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

			if (!stream)
			{
				// Failed to open the file
				return nullptr;
			}

			std::streampos end = stream.tellg();
			stream.seekg(0, std::ios::beg);
			uint32_t size = end - stream.tellg();

			if (size == 0)
			{
				// File is empty
				return nullptr;
			}

			char* buffer = new char[size];
			stream.read((char*)buffer, size);
			stream.close();

			*outSize = size;
			return buffer;
		}

		static MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath)
		{
			uint32_t fileSize = 0;
			char* fileData = ReadBytes(assemblyPath, &fileSize);

			// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

			if (status != MONO_IMAGE_OK)
			{
				const char* errorMessage = mono_image_strerror(status);
				// Log some error message using the errorMessage data
				return nullptr;
			}

			MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
			mono_image_close(image);

			// Don't forget to free the file data
			delete[] fileData;

			return assembly;
		}

		void PrintAssemblyTypes(MonoAssembly* assembly)
		{
			MonoImage* image = mono_assembly_get_image(assembly);
			const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
			int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

			for (int32_t i = 0; i < numTypes; i++)
			{
				uint32_t cols[MONO_TYPEDEF_SIZE];
				mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

				const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
				const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

				printf("%s.%s\n", nameSpace, name);
			}
		}
	}








	struct ScriptEngineData
	{
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
		MonoImage* CoreAssemblyImage = nullptr;
	};


	static ScriptEngineData* s_Data = nullptr;




	
	static void InitMono()
	{
		mono_set_assemblies_path("mono/lib");

		MonoDomain* rootdomain = mono_jit_init("SnapEngineJitRunTime");
		SNAP_ASSERT_MSG(rootdomain, "Mono Jit Initialization Failed!");
		s_Data->RootDomain = rootdomain;

		s_Data->AppDomain = mono_domain_create_appdomain("SnapEngineScriptRunTime", nullptr);
		mono_domain_set(s_Data->AppDomain, true);
	}

	static void ShutdownMonon()
	{
		// TODO:: ComeBack To This
		
		s_Data->AppDomain = nullptr;
		s_Data->RootDomain = nullptr;
	}

	void ScriptingEngine::LoadAssembly(const std::filesystem::path& DllFilePath)
	{
		s_Data->CoreAssembly = Utils::LoadCSharpAssembly(DllFilePath.string().c_str());
		Utils::PrintAssemblyTypes(s_Data->CoreAssembly);

		// Get C# Class From Dll File Which is Loaded And Stored in s_Data->CoreAssembly
		s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
	}

	void ScriptingEngine::Init()
	{
		s_Data = new ScriptEngineData();

		InitMono(); // Setup MonoDomain(Root & App), MonoAssembly(CoreAssembly)
		LoadAssembly("Resources/Scripts/Snap-ScriptCore.dll"); // Load dll file

		ScriptGlue::RegisterGlue(); // Setup C# Internal Calls

		// Read Class
		MonoClass* monoClass = mono_class_from_name(s_Data->CoreAssemblyImage, "SnapEngine", "Main");

		// 1. Creat an Object and Call Constructor
		MonoObject* instance = mono_object_new(s_Data->AppDomain, monoClass);
		mono_runtime_object_init(instance);

		// 2. Call Function PrintMessage
		MonoMethod* PrintMessageFunc = mono_class_get_method_from_name(monoClass, "PrintMessage", 0);
		mono_runtime_invoke(PrintMessageFunc, instance, nullptr, nullptr);

		// 3. Call Function Printvalue
		MonoMethod* PrintvalueFunc = mono_class_get_method_from_name(monoClass, "Printvalue", 1);

		float value = 50.0f;
		void* param = &value;

		mono_runtime_invoke(PrintvalueFunc, instance, &param, nullptr);

		// 3. Call Function PrintCustomMessage
		MonoString* monoString = mono_string_new(s_Data->AppDomain, "Hello Karim From C#");
		MonoMethod* PrintCustomMessageFunc = mono_class_get_method_from_name(monoClass, "PrintCustomMessage", 1);

		void* Stringparam = monoString;

		mono_runtime_invoke(PrintCustomMessageFunc, instance, &Stringparam, nullptr);

		//SNAP_ASSERT(false);
	}

	void ScriptingEngine::ShutDown()
	{
		ShutdownMonon();
		delete s_Data;
		s_Data = nullptr;
	}
}