#pragma once
#include <Snap/Core/Core.h>
#include <Snap/Core/asserts.h>
#include <filesystem>

namespace SnapEngine
{
	struct ProjectConfiguration
	{
		std::string Name = "Untiteld";

		std::filesystem::path m_StartScene;
		std::filesystem::path m_AssetPath;
		std::filesystem::path m_ScriptPath;
	};

	class Project
	{
	public:
		Project() = default;
		Project(const ProjectConfiguration& config)
			: m_Config(config) {}

		inline void SetConfig(const ProjectConfiguration& config) { m_Config = config; }
		inline ProjectConfiguration GetConfig() { return m_Config; }
		inline const std::filesystem::path& GetProjectDirectory() { return m_ProjectDirectory; }
		inline void SetProjectDirectory(const std::filesystem::path& ProjDirectory) { m_ProjectDirectory = ProjDirectory; }

	private:
		std::filesystem::path m_ProjectDirectory;
		ProjectConfiguration m_Config;
	};
}