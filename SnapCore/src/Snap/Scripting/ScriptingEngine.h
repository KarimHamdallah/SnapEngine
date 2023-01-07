#include <filesystem>

namespace Scripting
{
	class ScriptingEngine
	{
	public:
		static void Init();
		static void ShutDown();

		static void LoadAssembly(const std::filesystem::path& DllFilePath);
	};
}