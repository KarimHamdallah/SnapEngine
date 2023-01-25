#pragma once
#include "ProjectSerializer.h"

namespace SnapEngine
{
	class ProjectSystem
	{
	public:
		inline static ProjectConfiguration GetConfig() { SNAP_ASSERT(s_ActiveProject); return s_ActiveProject->GetConfig(); }
		inline static SnapPtr<Project> GetActiveProject() { SNAP_ASSERT(s_ActiveProject); return s_ActiveProject; }

		inline static const std::filesystem::path& GetProjectDirectory()
		{
			SNAP_ASSERT(s_ActiveProject);
			return s_ActiveProject->GetProjectDirectory();
		}

		inline static std::filesystem::path GetAssetDirectory()
		{
			SNAP_ASSERT(s_ActiveProject);
			return s_ActiveProject->GetProjectDirectory() / s_ActiveProject->GetConfig().m_AssetPath;
		}

		static SnapPtr<Project> New();
		static bool Load(const std::filesystem::path& ProjectPath);
		static bool SaveActiveProject(const std::filesystem::path& ProjectPath);

	private:
		inline static SnapPtr<Project> s_ActiveProject;
	};
}