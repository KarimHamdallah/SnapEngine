#include <filesystem>
#include <Snap/Core/Core.h>
#include <Snap/Scene/Entity.h>

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoImage MonoImage;
	typedef struct _MonoClassField MonoClassField;
}

namespace Scripting
{
	enum class ScriptFieldDataType
	{
		Invalid = 0,
		Float, Int, Double, UInt64, Bool,
		Vec2, Vec3, Vec4,
		Entity
	};

	struct ScriptField
	{
		std::string m_Name;
		ScriptFieldDataType m_Type;
		MonoClassField* m_MonoField;
	};

	struct ScriptFieldInstance
	{
		ScriptFieldInstance()
		{
			memset(m_Data, 0, sizeof(m_Data));
		}

		ScriptField m_Field;
		uint8_t m_Data[8];

		template<typename T>
		inline T GetData()
		{
			return *(T*)m_Data;
		}

		inline void SetData(void* data)
		{
			memcpy(m_Data, data, sizeof(data));
		}
	};

	class ScriptClass
	{
	public:
		ScriptClass() = default;

		ScriptClass(const std::string& nameSpace, const std::string& name, bool IsCore = true);

		MonoObject* Instantiate();

		MonoMethod* GetMethod(const std::string& name, int paramCount);

		MonoObject* InvokeMethod(MonoMethod* method, MonoObject* instance, void** params = nullptr);

		operator MonoClass*() { return m_MonoClass; }

		inline std::map<std::string, ScriptField>& GetFieldsMap() { return m_ScriptFields; }

	private:
		std::string m_NameSpace = "", mName = "";
		MonoClass* m_MonoClass = nullptr;

		std::map<std::string, ScriptField> m_ScriptFields;

		friend class ScriptingEngine;
	};

	using ScriptFieldMap = std::unordered_map<std::string, ScriptFieldInstance>;

	class ScriptInstance
	{
	public:
		ScriptInstance(const SnapEngine::SnapPtr<ScriptClass>& ScriptClass, SnapEngine::UUID EntityID);

		void InvokeOnCreatMethod();
		void InvokeOnUpdateMethod(float TimeStep);

		operator MonoObject* () { return m_Instance; }

		inline SnapEngine::SnapPtr<ScriptClass>& GetScriptClass() { return m_ScriptClass; }

		template<typename T>
		inline T GetFieldValue(const std::string& FieldName)
		{
			bool success = GetFieldValueInternal(FieldName, s_FieldValuesBuffer);
			if (!success)
				return T();

			return *(T*)s_FieldValuesBuffer;
		}

		template<typename T>
		inline void SetFieldValue(const std::string& FieldName, const T& value)
		{
			SetFieldValueInternal(FieldName, &value);
		}

	private:
		bool GetFieldValueInternal(const std::string& FieldName, void* Buffer);
		bool SetFieldValueInternal(const std::string& FieldName, const void* Buffer);

		SnapEngine::SnapPtr<ScriptClass> m_ScriptClass;

		MonoObject* m_Instance = nullptr;
		MonoMethod* m_OnCreatMethod = nullptr;
		MonoMethod* m_OnUpdateMethod = nullptr;

		inline static char s_FieldValuesBuffer[8];

		friend class ScriptingEngine;
	};

	class ScriptingEngine
	{
	public:
		static void Init();
		static void ShutDown();

		static void LoadAppAssembly(const std::filesystem::path& DllFilePath);
		static void LoadAssembly(const std::filesystem::path& DllFilePath);
		static void LoadCSharpAssemblyClasses();

		static std::unordered_map<std::string, SnapEngine::SnapPtr<ScriptClass>> GetEntityClassesMap();
		static bool IsEntityClassExist(std::string ClassName);

		static void OnCreatEntity(SnapEngine::Entity e);
		static void OnUpdateEntity(SnapEngine::Entity e, SnapEngine::TimeStep Time);

		static void OnRunTimeStart(SnapEngine::Scene* Scene);
		static void OnRunTimeStop();

		static SnapEngine::Scene* GetSceneContext();

		static MonoImage* GetCoreAssemblyImage();

		static SnapEngine::SnapPtr<ScriptClass> GetScriptClass(const std::string& name);
		static SnapEngine::SnapPtr<ScriptInstance> GetScriptInstance(SnapEngine::UUID uuid);
		
		static ScriptFieldMap& GetScriptFieldMap(SnapEngine::Entity entity);

		static MonoObject* GetManagedObject(SnapEngine::UUID uuid);

		static void ReloadAssemby();
	public:
		static MonoObject* InstantiateClass(MonoClass* monoClass);

		friend class ScriptGlue;
	};
}