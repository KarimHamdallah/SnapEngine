#include <filesystem>
#include <Snap/Core/Core.h>
#include <Snap/Scene/Entity.h>

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoImage MonoImage;
}

namespace Scripting
{
	class ScriptClass
	{
	public:
		ScriptClass() = default;

		ScriptClass(const std::string& nameSpace, const std::string& name);

		MonoObject* Instantiate();

		MonoMethod* GetMethod(const std::string& name, int paramCount);

		MonoObject* InvokeMethod(MonoMethod* method, MonoObject* instance, void** params = nullptr);

		operator MonoClass*() { return m_MonoClass; }

	private:
		std::string m_NameSpace = "", mName = "";
		MonoClass* m_MonoClass = nullptr;
	};

	class ScriptInstance
	{
	public:
		ScriptInstance(const SnapEngine::SnapPtr<ScriptClass>& ScriptClass);

		void InvokeOnCreatMethod();
		void InvokeOnUpdateMethod(float TimeStep);

	private:
		SnapEngine::SnapPtr<ScriptClass> m_ScriptClass;

		MonoObject* m_Instance = nullptr;
		MonoMethod* m_OnCreatMethod = nullptr;
		MonoMethod* m_OnUpdateMethod = nullptr;
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
	public:
		static MonoObject* InstantiateClass(MonoClass* monoClass);
	};
}