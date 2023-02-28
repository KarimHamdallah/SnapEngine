#include "SnapPCH.h"
#include "SceneSerializer.h"

#include <yaml-cpp/yaml.h>
#include <Snap/Scene/Comps/Components.h>
#include <Snap/Scripting/ScriptingEngine.h>

static std::string ScriptFieldDataTypeToString(Scripting::ScriptFieldDataType Type)
{
	switch (Type)
	{
	    case Scripting::ScriptFieldDataType::Invalid: return "None";
	    case Scripting::ScriptFieldDataType::Float:  return "Float"; 
	    case Scripting::ScriptFieldDataType::Int:    return "Int";   
	    case Scripting::ScriptFieldDataType::Double: return "Double";
	    case Scripting::ScriptFieldDataType::UInt64: return "UInt64";
	    case Scripting::ScriptFieldDataType::Bool:   return "Bool";  
	    case Scripting::ScriptFieldDataType::Vec2:   return "Vec2";  
	    case Scripting::ScriptFieldDataType::Vec3:   return "Vec3";  
	    case Scripting::ScriptFieldDataType::Vec4:   return "Vec4";  
	    case Scripting::ScriptFieldDataType::Entity: return "Entity";

		SNAP_ASSERT_MSG(false, "Unkown Type!");
		return "None";
	}
}

static Scripting::ScriptFieldDataType ScriptFieldDataTypeFromString(std::string Type)
{
	if(Type == "None")   return Scripting::ScriptFieldDataType::Invalid;
	if(Type == "Float")  return Scripting::ScriptFieldDataType::Float;  
	if(Type == "Int")    return Scripting::ScriptFieldDataType::Int;    
	if(Type == "Double") return Scripting::ScriptFieldDataType::Double; 
	if(Type == "UInt64") return Scripting::ScriptFieldDataType::UInt64; 
	if(Type == "Bool")   return Scripting::ScriptFieldDataType::Bool;   
	if(Type == "Vec2")   return Scripting::ScriptFieldDataType::Vec2;   
	if(Type == "Vec3")   return Scripting::ScriptFieldDataType::Vec3;   
	if(Type == "Vec4")   return Scripting::ScriptFieldDataType::Vec4;   
	if(Type == "Entity") return Scripting::ScriptFieldDataType::Entity; 


	SNAP_ASSERT_MSG(false, "Unkown Type!");
	return Scripting::ScriptFieldDataType::Invalid;
}

static std::string Rigidbody2DTypeToString(SnapEngine::RigidBody2DComponent::RigidBodyType Type)
{
	switch (Type)
	{
	    case SnapEngine::RigidBody2DComponent::RigidBodyType::STATIC: return "Static";
	    case SnapEngine::RigidBody2DComponent::RigidBodyType::DYNAMIC: return "Dynamic";
	    case SnapEngine::RigidBody2DComponent::RigidBodyType::KINEMATIC: return "Kinematic";

		SNAP_ASSERT_MSG(false, "Unkown Type!");
		return "";
	}
}

static SnapEngine::RigidBody2DComponent::RigidBodyType Rigidbody2DTypeFromString(const std::string& Type)
{
	if (Type == "Static")     { return SnapEngine::RigidBody2DComponent::RigidBodyType::STATIC; }
	if (Type == "Dynamic")    { return SnapEngine::RigidBody2DComponent::RigidBodyType::DYNAMIC; }
	if (Type == "Kinematic")  { return SnapEngine::RigidBody2DComponent::RigidBodyType::KINEMATIC; }

	return SnapEngine::RigidBody2DComponent::RigidBodyType::STATIC;
}

namespace YAML
{
	template<>
	struct convert<glm::vec2>
	{
		static YAML::Node encode(const glm::vec2& rhs)
		{
			YAML::Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			return node;
		}

		static bool decode(const YAML::Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

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
	YAML::Emitter& operator <<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow; // [x, y]
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

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
		SNAP_ASSERT_MSG(entity.HasComponent< IDComponent>(), "Entity dosen't have UUIDComponent!");

		//SNAP_DEBUG("ID = {0}, UUID = {1}",entity.GetID(), entity.GetComponent<IDComponent>().ID);

		out << YAML::BeginMap; // EntityID
		out << YAML::Key << "EntityID" << YAML::Value << entity.GetComponent<IDComponent>().ID;
		
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
			if(sprite_renderer.m_Texture)
				out << YAML::Key << "TexturePath" << YAML::Value << sprite_renderer.m_Texture->getFilePath().c_str();
			out << YAML::Key << "TilingFactor" << YAML::Value << sprite_renderer.m_TilingFactor;

			out << YAML::EndMap; // SpriteRendererComponent
		}

		if (entity.HasComponent<CircleRendererComponent>())
		{
			out << YAML::Key << "CircleRendererComponent";
			out << YAML::BeginMap; // CircleRendererComponent
			auto& circle_renderer = entity.GetComponent<CircleRendererComponent>();

			out << YAML::Key << "Color" << YAML::Value << circle_renderer.m_Color;
			out << YAML::Key << "Fade" << YAML::Value << circle_renderer.m_Fade;
			out << YAML::Key << "Thickness" << YAML::Value << circle_renderer.m_Thickness;

			out << YAML::EndMap; // CircleRendererComponent
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

		if (entity.HasComponent<RigidBody2DComponent>())
		{
			out << YAML::Key << "RigidBody2DComponent";
			out << YAML::BeginMap; // RigidBody2DComponent
			auto& rb2d = entity.GetComponent<RigidBody2DComponent>();

			out << YAML::Key << "RigidBody2DType" << YAML::Value << Rigidbody2DTypeToString(rb2d.m_Type);
			out << YAML::Key << "FixedRotation" << YAML::Value << rb2d.m_FixedRotation;

			out << YAML::EndMap; // RigidBody2DComponent
		}

		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			out << YAML::Key << "BoxCollider2DComponent";
			out << YAML::BeginMap; // BoxCollider2DComponent
			auto& collider = entity.GetComponent<BoxCollider2DComponent>();

			out << YAML::Key << "Size" << YAML::Value << collider.m_Size;
			out << YAML::Key << "Offset" << YAML::Value << collider.m_Offset;

			out << YAML::Key << "Density" << YAML::Value << collider.m_Density;
			out << YAML::Key << "Friction" << YAML::Value << collider.m_Friction;
			out << YAML::Key << "Restitution" << YAML::Value << collider.m_Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << collider.m_RestitutionThreshold;


			out << YAML::EndMap; // BoxCollider2DComponent
		}

		if (entity.HasComponent<CircleCollider2DComponent>())
		{
			out << YAML::Key << "CircleCollider2DComponent";
			out << YAML::BeginMap; // CircleCollider2DComponent
			auto& collider = entity.GetComponent<CircleCollider2DComponent>();

			out << YAML::Key << "Offset" << YAML::Value << collider.m_Offset;
			out << YAML::Key << "Raduis" << YAML::Value << collider.m_Raduis;

			out << YAML::Key << "Density" << YAML::Value << collider.m_Density;
			out << YAML::Key << "Friction" << YAML::Value << collider.m_Friction;
			out << YAML::Key << "Restitution" << YAML::Value << collider.m_Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << collider.m_RestitutionThreshold;


			out << YAML::EndMap; // CircleCollider2DComponent
		}

		if (entity.HasComponent<TextRendererComponent>())
		{
			out << YAML::Key << "TextRendererComponent";
			out << YAML::BeginMap; // TextRendererComponent
			auto& TextRenderer = entity.GetComponent<TextRendererComponent>();

			out << YAML::Key << "TextString" << YAML::Value << TextRenderer.m_TextString;
			out << YAML::Key << "TextColor" << YAML::Value << TextRenderer.m_Color;
			out << YAML::Key << "KerningOffset" << YAML::Value << TextRenderer.KerningOffset;
			out << YAML::Key << "LineSpacing" << YAML::Value << TextRenderer.LineSpacing;


			out << YAML::EndMap; // TextRendererComponent
		}

		if (entity.HasComponent<ScriptComponent>())
		{
			out << YAML::Key << "ScriptComponent";
			out << YAML::BeginMap; // CircleCollider2DComponent
			auto& sc = entity.GetComponent<ScriptComponent>();

			if (!sc.ClassName.empty())
				out << YAML::Key << "ClassName" << YAML::Value << sc.ClassName;

			// Field Map
			out << YAML::Key << "FieldMap" << YAML::Value;
			out << YAML::BeginSeq;

			auto& FieldMap = Scripting::ScriptingEngine::GetScriptFieldMap(entity);

			for (auto& [name, FieldInstances] : FieldMap)
			{
				out << YAML::BeginMap;
				out << YAML::Key << "Name" << YAML::Value << name;
				out << YAML::Key << "Type" << YAML::Value << ScriptFieldDataTypeToString(FieldInstances.m_Field.m_Type);
				
				switch (FieldInstances.m_Field.m_Type)
				{
				  case Scripting::ScriptFieldDataType::Invalid:
				  {
					  SNAP_ASSERT(false, "Unkown Type!");
				  }
				  break;
				  case Scripting::ScriptFieldDataType::Float:
				  {
					  float data = FieldInstances.GetData<float>();
					  out << YAML::Key << "Data" << YAML::Value << data;
				  }
				  break;
				  case Scripting::ScriptFieldDataType::Int:
				  {
					  int data = FieldInstances.GetData<int>();
					  out << YAML::Key << "Data" << YAML::Value << data;
				  }
				  break;
				  case Scripting::ScriptFieldDataType::Double:
				  {
					  double data = FieldInstances.GetData<double>();
					  out << YAML::Key << "Data" << YAML::Value << data;
				  }
				  break;
				  case Scripting::ScriptFieldDataType::UInt64:
				  {
					  uint64_t data = FieldInstances.GetData<uint64_t>();
					  out << YAML::Key << "Data" << YAML::Value << data;
				  }
				  break;
				  case Scripting::ScriptFieldDataType::Bool:
				  {
					  bool data = FieldInstances.GetData<bool>();
					  out << YAML::Key << "Data" << YAML::Value << data;
				  }
				  break;
				  case Scripting::ScriptFieldDataType::Vec2:
				  {
					  glm::vec2 data = FieldInstances.GetData<glm::vec2>();
					  out << YAML::Key << "Data" << YAML::Value << data;
				  }
				  break;
				  case Scripting::ScriptFieldDataType::Vec3:
				  {
					  glm::vec3 data = FieldInstances.GetData<glm::vec3>();
					  out << YAML::Key << "Data" << YAML::Value << data;
				  }
				  break;
				  case Scripting::ScriptFieldDataType::Vec4:
				  {
					  glm::vec4 data = FieldInstances.GetData<glm::vec4>();
					  out << YAML::Key << "Data" << YAML::Value << data;
				  }
				  break;
				  case Scripting::ScriptFieldDataType::Entity:
				  {
					  UUID data = FieldInstances.GetData<UUID>();
					  out << YAML::Key << "Data" << YAML::Value << data;
				  }
				  break;
				}
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;

			out << YAML::EndMap; // ScriptComponent
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

		YAML::Node data;
		try
		{
			data = YAML::Load(strstream.str());
		}
		catch (const YAML::ParserException& e)
		{
			SNAP_ERROR("Yaml Failed to load {0}.snap file, {1}", filepath, e.what());
			return false;
		}
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
				Entity e = m_Scene->CreatEntityWithUUID(EntityID, tag);

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
					
					auto& comp = e.AddComponent<SpriteRendererComponent>(Color);
					if (SpriteRendererComponentNode["TexturePath"])
					{
						std::string TextureFilePath = SpriteRendererComponentNode["TexturePath"].as<std::string>();
						comp.m_Texture = SnapPtr<Texture2D>(Texture2D::Creat(TextureFilePath));
					}
					comp.m_TilingFactor = SpriteRendererComponentNode["TilingFactor"].as<float>();
				}

				if (entity["CircleRendererComponent"])
				{
					auto& CircleRendererComponentNode = entity["CircleRendererComponent"];
					glm::vec4 Color = CircleRendererComponentNode["Color"].as<glm::vec4>();
					float Thickness = CircleRendererComponentNode["Thickness"].as<float>();
					float Fade = CircleRendererComponentNode["Fade"].as<float>();

					e.AddComponent<CircleRendererComponent>(Color, Thickness, Fade);
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

				if (entity["RigidBody2DComponent"])
				{
					auto& RigidBody2DComponentNode = entity["RigidBody2DComponent"];

					auto Type = RigidBody2DComponentNode["RigidBody2DType"].as<std::string>();
					auto FixedRotation = RigidBody2DComponentNode["FixedRotation"].as<bool>();

				    auto& Comp = e.AddComponent<RigidBody2DComponent>();
					Comp.m_Type = Rigidbody2DTypeFromString(Type);
					Comp.m_FixedRotation = FixedRotation;
				}

				if (entity["BoxCollider2DComponent"])
				{
					auto& RigidBody2DComponentNode = entity["BoxCollider2DComponent"];

					auto Size = RigidBody2DComponentNode["Size"].as<glm::vec2>();
					auto Offset = RigidBody2DComponentNode["Offset"].as<glm::vec2>();

					auto Density = RigidBody2DComponentNode["Density"].as<float>();
					auto Friction = RigidBody2DComponentNode["Friction"].as<float>();
					auto Restitution = RigidBody2DComponentNode["Restitution"].as<float>();
					auto RestitutionThreshold = RigidBody2DComponentNode["RestitutionThreshold"].as<float>();

					auto& Comp = e.AddComponent<BoxCollider2DComponent>();
					Comp.m_Size = Size;
					Comp.m_Offset = Offset;
					Comp.m_Density = Density;
					Comp.m_Friction = Friction;
					Comp.m_Restitution = Restitution;
					Comp.m_RestitutionThreshold = RestitutionThreshold;
				}

				if (entity["CircleCollider2DComponent"])
				{
					auto& CircleCollider2DComponentNode = entity["CircleCollider2DComponent"];

					auto Raduis = CircleCollider2DComponentNode["Raduis"].as<float>();
					auto Offset = CircleCollider2DComponentNode["Offset"].as<glm::vec2>();

					auto Density = CircleCollider2DComponentNode["Density"].as<float>();
					auto Friction = CircleCollider2DComponentNode["Friction"].as<float>();
					auto Restitution = CircleCollider2DComponentNode["Restitution"].as<float>();
					auto RestitutionThreshold = CircleCollider2DComponentNode["RestitutionThreshold"].as<float>();

					auto& Comp = e.AddComponent<CircleCollider2DComponent>();
					Comp.m_Raduis = Raduis;
					Comp.m_Offset = Offset;
					Comp.m_Density = Density;
					Comp.m_Friction = Friction;
					Comp.m_Restitution = Restitution;
					Comp.m_RestitutionThreshold = RestitutionThreshold;
				}

				if (entity["TextRendererComponent"])
				{
					auto& TextRendererComponentNode = entity["TextRendererComponent"];
					
					auto TextString = TextRendererComponentNode["TextString"].as<std::string>();
					auto TextColor = TextRendererComponentNode["TextColor"].as<glm::vec4>();
					auto KerningOffset = TextRendererComponentNode["KerningOffset"].as<float>();
					auto LineSpacing = TextRendererComponentNode["LineSpacing"].as<float>();

					auto& Comp = e.AddComponent<TextRendererComponent>();
					Comp.m_TextString = TextString;
					Comp.m_Color = TextColor;
					Comp.KerningOffset = KerningOffset;
					Comp.LineSpacing = LineSpacing;
				}

				if (entity["ScriptComponent"])
				{
					auto& ScriptComponentNode = entity["ScriptComponent"];

					std::string ClassName = "";
					if (ScriptComponentNode["ClassName"])
						ClassName = ScriptComponentNode["ClassName"].as<std::string>();

					auto& ScriptFields = ScriptComponentNode["FieldMap"];

					if (ScriptFields)
					{
						auto& FieldMap = Scripting::ScriptingEngine::GetScriptFieldMap(e);

						for (auto& ScriptField : ScriptFields)
						{
							std::string Name = ScriptField["Name"].as<std::string>();
							std::string TypeString = ScriptField["Type"].as<std::string>();
							Scripting::ScriptFieldDataType Type = ScriptFieldDataTypeFromString(TypeString);

							switch (Type)
							{
							case Scripting::ScriptFieldDataType::Invalid:
							{
								SNAP_ASSERT(false, "Unkown Type!");
							}
							break;
							case Scripting::ScriptFieldDataType::Float:
							{
								float data = ScriptField["Data"].as<float>();
								FieldMap[Name].SetData(&data);
							}
							break;
							case Scripting::ScriptFieldDataType::Int:
							{
								int data = ScriptField["Data"].as<int>();
								FieldMap[Name].SetData(&data);
							}
							break;
							case Scripting::ScriptFieldDataType::Double:
							{
								double data = ScriptField["Data"].as<double>();
								FieldMap[Name].SetData(&data);
							}
							break;
							case Scripting::ScriptFieldDataType::UInt64:
							{
								uint64_t data = ScriptField["Data"].as<uint64_t>();
								FieldMap[Name].SetData(&data);
							}
							break;
							case Scripting::ScriptFieldDataType::Bool:
							{
								bool data = ScriptField["Data"].as<bool>();
								FieldMap[Name].SetData(&data);
							}
							break;
							case Scripting::ScriptFieldDataType::Vec2:
							{
								glm::vec2 data = ScriptField["Data"].as<glm::vec2>();
								FieldMap[Name].SetData(&data);
							}
							break;
							case Scripting::ScriptFieldDataType::Vec3:
							{
								glm::vec3 data = ScriptField["Data"].as<glm::vec3>();
								FieldMap[Name].SetData(&data);
							}
							break;
							case Scripting::ScriptFieldDataType::Vec4:
							{
								glm::vec4 data = ScriptField["Data"].as<glm::vec4>();
								FieldMap[Name].SetData(&data);
							}
							break;
							case Scripting::ScriptFieldDataType::Entity:
							{
								UUID data = ScriptField["Data"].as<uint64_t>();
								FieldMap[Name].SetData(&data);
							}
							break;
							}

							FieldMap[Name].m_Field.m_Name = Name;
							FieldMap[Name].m_Field.m_Type = Type;
						}
					}

					auto& Comp = e.AddComponent<ScriptComponent>();
					Comp.ClassName = ClassName;
				}
			}
		}
	}
}