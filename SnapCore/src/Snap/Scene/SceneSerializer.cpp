#include "SnapPCH.h"
#include "SceneSerializer.h"

#include <yaml-cpp/yaml.h>


namespace YAML
{
	template<>
	struct convert<glm::vec3>
	{
		static YAML::Node encode(const glm::vec3& rhs)
		{
			YAML::Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const YAML::Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};


	template<>
	struct convert<glm::vec4>
	{
		static YAML::Node encode(const glm::vec4& rhs)
		{
			YAML::Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const YAML::Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;


			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}

namespace SnapEngine
{
	YAML::Emitter& operator <<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow; // [x, y, z]
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator <<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow; // [x, y, z, w]
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	SceneSerializer::SceneSerializer(const SnapPtr<Scene>& scene)
		: m_Scene(scene)
	{

	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		out << YAML::BeginMap; // EntityID
		out << YAML::Key << "EntityID" << YAML::Value << std::to_string(entity.GetID());
		
		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap; // TagComponent
			auto& tag = entity.GetComponent<TagComponent>().m_Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag.c_str();
			out << YAML::EndMap; // TagComponent
		}

		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap; // TransformComponent
			auto& transform = entity.GetComponent<TransformComponent>();

			out << YAML::Key << "Position" << YAML::Value << transform.m_Position;
			out << YAML::Key << "Rotation" << YAML::Value << transform.m_Rotation;
			out << YAML::Key << "Scale" << YAML::Value << transform.m_Scale;

			out << YAML::EndMap; // TransformComponent
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap; // SpriteRendererComponent
			auto& sprite_renderer = entity.GetComponent<SpriteRendererComponent>();

			out << YAML::Key << "Color" << YAML::Value << sprite_renderer.m_Color;

			out << YAML::EndMap; // SpriteRendererComponent
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap; // CameraComponent
			auto& cam = entity.GetComponent<CameraComponent>();

			out << YAML::Key << "ProjectionType" << YAML::Value << (int)cam.m_Camera.GetProjectionType();
			out << YAML::Key << "OrthoGraphicSize" << YAML::Value << cam.m_Camera.GetOrthoGraphicSize();
			out << YAML::Key << "OrthoGraphicNear" << YAML::Value << cam.m_Camera.GetOrthoGraphicNear();
			out << YAML::Key << "OrthoGraphicFar" << YAML::Value << cam.m_Camera.GetOrthoGraphicFar();
			out << YAML::Key << "PerspectiveFov" << YAML::Value << cam.m_Camera.GetPerspectiveFov();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << cam.m_Camera.GetPerspectiveNear();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << cam.m_Camera.GetPerspectiveFar();

			out << YAML::Key << "IsMain" << YAML::Value << cam.m_IsMain;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cam.m_FixedAspectRatio;

			out << YAML::EndMap; // CameraComponent
		}
		
		out << YAML::EndMap; // EntityID
	}

	void SceneSerializer::SerializeScene(const std::string& filepath)
	{
		/*
		* Scene: Example Scene                          // Key << value
		* Entities List:                                // Key << seq
		*               - EntityID: 0                   // Key << value
		*                      - TagComponent:          // Key
		*                             - Tag: RedSprite  // Key << value
		*					   - TransformComponent:
		*					         - Position : 0.0f, 0.0f, 0.0f
		*							 - Rotation : 0.0f, 0.0f, 0.0f
		*							 - Scale : 1.0f, 1.0f, 1.0f
		*					   - SpriteRendererComponent:
		*					         - Color : 1.0f, 0.0f, 0.0f, 1.0f
		*               - EntityID: 1
		*                      - TagComponent:
		*                             - Tag: WhiteSprite
		*					   - TransformComponent:
		*					         - Position : 0.0f, 0.0f, 0.0f
		*							 - Rotation : 0.0f, 0.0f, 0.0f
		*							 - Scale : 1.0f, 1.0f, 1.0f
		*					   - SpriteRendererComponent:
		*					         - Color : 1.0f, 1.0f, 1.0f, 1.0f
		* 
		*/


		YAML::Emitter out;
		
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Example Scene";
		out << YAML::Key << "Entities List" << YAML::BeginSeq;
		{
			m_Scene->GetRegistry().each([&](auto entityID)
				{
					Entity entity = { entityID, m_Scene.get() };
		
					if (!entity)
						return;
					SerializeEntity(out, entity);
				});
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;
		
		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	bool SceneSerializer::DeSerializeScene(const std::string& filepath)
	{
		std::ifstream file(filepath);
		std::stringstream strstream;
		strstream << file.rdbuf();

		YAML::Node data = YAML::Load(strstream.str());
		if (!data["Scene"])
			return false;

		std::string SceneName = data["Scene"].as<std::string>();
		SNAP_DEBUG("DeSerializing Scene: {}", SceneName);

		auto& Entities = data["Entities List"];
		if (Entities)
		{
			for (auto& entity : Entities)
			{
				uint64_t EntityID = entity["EntityID"].as<uint64_t>();
				std::string tag;
				auto TagComp = entity["TagComponent"];
				if (TagComp)
					tag = TagComp["Tag"].as<std::string>();
				Entity e = m_Scene->CreatEntity(tag);

				if (entity["TransformComponent"])
				{
					auto TransformComponentNode = entity["TransformComponent"];
					auto& transformcomp = e.GetComponent<TransformComponent>();
					
					transformcomp.m_Position = TransformComponentNode["Position"].as<glm::vec3>();
					transformcomp.m_Rotation = TransformComponentNode["Rotation"].as<glm::vec3>();
					transformcomp.m_Scale = TransformComponentNode["Scale"].as<glm::vec3>();
				}

				if(entity["SpriteRendererComponent"])
				{
					auto& SpriteRendererComponentNode = entity["SpriteRendererComponent"];
					glm::vec4 Color = SpriteRendererComponentNode["Color"].as<glm::vec4>();

					e.AddComponent<SpriteRendererComponent>(Color);
				}

				if (entity["CameraComponent"])
				{
					auto& CameraComponentNode = entity["CameraComponent"];
					int ProjectionType = CameraComponentNode["ProjectionType"].as<int>();
					float OrthoGraphicSize = CameraComponentNode["OrthoGraphicSize"].as<float>();
					float OrthoGraphicNear = CameraComponentNode["OrthoGraphicNear"].as<float>();
					float OrthoGraphicFar = CameraComponentNode["OrthoGraphicFar"].as<float>();
					float PerspectiveFov = CameraComponentNode["PerspectiveFov"].as<float>();
					float PerspectiveNear = CameraComponentNode["PerspectiveNear"].as<float>();
					float PerspectiveFar = CameraComponentNode["PerspectiveFar"].as<float>();

					bool IsMain = CameraComponentNode["IsMain"].as<bool>();
					bool FixedAspectRatio = CameraComponentNode["FixedAspectRatio"].as<bool>();

					CameraComponent& CamComp = e.AddComponent<CameraComponent>();
					if (ProjectionType == static_cast<int>(SceneCamera::ProjectionType::OrthoGraphic))
					{
						CamComp.m_Camera.SetPerspective(PerspectiveFov, PerspectiveNear, PerspectiveFar);
						CamComp.m_Camera.SetOrthoGraphic(OrthoGraphicSize, OrthoGraphicNear, OrthoGraphicFar);
						CamComp.m_IsMain = IsMain;
						CamComp.m_FixedAspectRatio;
					}
					else if (ProjectionType == static_cast<int>(SceneCamera::ProjectionType::Perspective))
					{
						CamComp.m_Camera.SetOrthoGraphic(OrthoGraphicSize, OrthoGraphicNear, OrthoGraphicFar);
						CamComp.m_Camera.SetPerspective(PerspectiveFov, PerspectiveNear, PerspectiveFar);
						CamComp.m_IsMain = IsMain;
						CamComp.m_FixedAspectRatio;
					}
				}
			}
		}
	}
}