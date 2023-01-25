#include "SnapPCH.h"
#include "ProjectSerializer.h"

#include <yaml-cpp/yaml.h>

#include "Project.h"

namespace SnapEngine
{
	ProjectSerializer::ProjectSerializer(const SnapPtr<Project>& Project)
		: m_Project(Project) {}
	
	bool ProjectSerializer::Serialize(const std::filesystem::path& ProjectPath)
	{
		SNAP_ASSERT(ProjectPath.extension().string() == ".SnapProj");

		ProjectConfiguration ProjectConfig = m_Project->GetConfig();
		SNAP_DEBUG("Serializing Project: {}", ProjectConfig.Name.c_str());

		YAML::Emitter out;
		out << YAML::BeginMap; // begin map
		out << YAML::Key << "Project" << YAML::Value;

		out << YAML::BeginMap; // begin map
		out << YAML::Key << "Name" << YAML::Value << ProjectConfig.Name;
		out << YAML::Key << "StartScenePath" << YAML::Value << ProjectConfig.m_StartScene.string();
		out << YAML::Key << "AssetFolerPath" << YAML::Value << ProjectConfig.m_AssetPath.string();
		out << YAML::Key << "ScriptModulePath" << YAML::Value << ProjectConfig.m_ScriptPath.string();
		out << YAML::EndMap; // end map

		out << YAML::EndMap; // end map

		std::ofstream fout(ProjectPath);
		fout << out.c_str();

		return true;
	}

	bool ProjectSerializer::DeSerialize(const std::filesystem::path& ProjectPath)
	{
		std::ifstream file(ProjectPath);
		std::stringstream strstream;
		strstream << file.rdbuf();

		YAML::Node data;
		try
		{
			data = YAML::Load(strstream.str());
		}
		catch (const YAML::ParserException& e)
		{
			SNAP_ERROR("Yaml Failed to load {0}.snapProj file, {1}", ProjectPath.string(), e.what());
			return false;
		}
		if (!data["Project"])
			return false;

		auto ProjectNode = data["Project"];

		std::string ProjectName = ProjectNode["Name"].as<std::string>();
		SNAP_DEBUG("DeSerializing Project: {}", ProjectName.c_str());

		ProjectConfiguration config;

		config.Name = ProjectNode["Name"].as<std::string>();
		config.m_StartScene = ProjectNode["StartScenePath"].as<std::string>();
		config.m_AssetPath = ProjectNode["AssetFolerPath"].as<std::string>();
		config.m_ScriptPath = ProjectNode["ScriptModulePath"].as<std::string>();

		m_Project->SetConfig(config);

		std::filesystem::path ProjectDir = ProjectPath.parent_path();
		m_Project->SetProjectDirectory(ProjectDir);

		return true;
	}
}