#include "SnapPCH.h"
#include "ProjectSystem.h"


namespace SnapEngine
{
    SnapPtr<Project> SnapEngine::ProjectSystem::New()
    {
        ProjectConfiguration config; // Empty Config
        s_ActiveProject = CreatSnapPtr<Project>(config);
        return s_ActiveProject;
    }

    bool ProjectSystem::Load(const std::filesystem::path& ProjectPath)
    {
        SnapPtr<Project> Proj = CreatSnapPtr<Project>(); // Empty Project With Empty Config
        ProjectSerializer Serializer(Proj);
        if (Serializer.DeSerialize(ProjectPath)) // Fill Project Config By Deserialize File At ProjectPath
        {
            s_ActiveProject = Proj;
            return true;
        }

        return false;
    }

    bool ProjectSystem::SaveActiveProject(const std::filesystem::path& ProjectPath)
    {
        ProjectSerializer Serializer(s_ActiveProject);
        return Serializer.Serialize(ProjectPath); // Fill File At ProjectPath by Serialized Data From ActiveScene
    }
}