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

	class ScriptClass
	{
	public:
		ScriptClass() = default;

		ScriptClass(const std::string& nameSpace, const std::string& name);

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

		static void LoadAssembly(const std::filesystem::path& DllFilePath);
		static void LoadCSharpAssemblyClasses(MonoAssembly* Assembly, MonoImage* Image);

		static std::unordered_map<std::string, SnapEngine::SnapPtr<ScriptClass>> GetEntityClassesMap();
		static bool IsEntityClassExist(std::string ClassName);

		static void OnCreatEntity(SnapEngine::Entity e);
		static void OnUpdateEntity(SnapEngine::Entity e, SnapEngine::TimeStep Time);

		static void OnRunTimeStart(SnapEngine::Scene* Scene);
		static void OnRunTimeStop();

		static SnapEngine::Scene* GetSceneContext();

		static MonoImage* GetCoreAssemblyImage();

		static SnapEngine::SnapPtr<ScriptInstance> GetScriptInstance(SnapEngine::UUID uuid);
	public:
		static MonoObject* InstantiateClass(MonoClass* monoClass);

		friend class ScriptGlue;
	};
}