#include "SceneHierarchyPanel.h"
#include <Snap/Scene/Comps/Components.h>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>
#include <imgui.h>
#include <imgui_internal.h>

#include <Snap/Scripting/ScriptingEngine.h>

namespace SnapEngine
{
	extern const std::filesystem::path g_AssetPath;

	static void DrawVec3Control(const std::string& lable, glm::vec3& value, float resetvalue = 0.0f,float Speed = 0.01f, float colwidth = 80.0f)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImFont* BoldFont = io.Fonts->Fonts[0];

		ImGui::PushID(lable.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, colwidth);
		ImGui::Text(lable.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.0f, 0.0f });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 ButtonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.7f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.9f, 0.4f, 0.4f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(BoldFont);
		if (ImGui::Button("X", ButtonSize))
			value.x = resetvalue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &value.x, Speed);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.15f, 0.8f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.4f, 0.9f, 0.4f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.1f, 0.8f, 0.15f, 1.0f });
		ImGui::PushFont(BoldFont);
		if (ImGui::Button("Y", ButtonSize))
			value.y = resetvalue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &value.y, Speed);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.15f, 0.1f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.4f, 0.4f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.1f, 0.1f, 0.8f, 1.0f });
		ImGui::PushFont(BoldFont);
		if (ImGui::Button("Z", ButtonSize))
			value.z = resetvalue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &value.z, Speed);
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);
		ImGui::PopID();
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction UIfunction)
	{
		auto ComponentNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_DefaultOpen;

		if (entity.HasComponent<T>())
		{
			T& component = entity.GetComponent<T>(); // Get Component
			ImVec2 ContentRegionAvail = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4, 4 });
			float LineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();// Separator Between each 2 components

			// creat Componnet Node With (ComponnetNodeFlags) Props and Name (name)
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), ComponentNodeFlags, name.c_str());
			ImGui::PopStyleVar();
			
			// Draw Component Properites Menu
			ImGui::SameLine(ContentRegionAvail.x - LineHeight * 0.5f);
			if (ImGui::Button("+", { LineHeight, LineHeight }))
				ImGui::OpenPopup("Component Settings");
			

			// Component Properties
			bool removed = false;
			if (ImGui::BeginPopup("Component Settings"))
			{
				if (ImGui::MenuItem("Remove Component"))
					removed = true;

				ImGui::EndPopup();
			}

			// Draw Component Features On Node Open
			if (open)
			{
				UIfunction(component); // Draw Component UI

				ImGui::TreePop(); // PopTree On Node Open
			}

			// Remove Component On Component Removed
			if (removed)
				entity.RemoveComponent<T>();
		}
	}

	SceneHierarchyPanel::SceneHierarchyPanel(const SnapPtr<Scene>& scene)
	{
		m_Scene = scene;
	}

	void SceneHierarchyPanel::SetScene(const SnapPtr<Scene>& scene)
	{
		m_Scene = scene;
		m_SelectedEntity = {};
	}

	void SceneHierarchyPanel::ImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		m_Scene->GetRegistry().each([=](auto entityID)
			{
				Entity entity = { entityID, m_Scene.get() };
				DrawEntityNode(entity);
			});

		// Left Click on Balnk Space
		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_SelectedEntity = {};

		// Right Click on Balnk Space
		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::MenuItem("Creat Empty"))
				m_Scene->CreatEntity("Empty");
			
			ImGui::EndPopup();
		}

		ImGui::End();


		ImGui::Begin("Inspector");
		
		if (m_SelectedEntity)
			DrawComponents(m_SelectedEntity);

		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(const Entity& entity)
	{
		auto& tag = m_Scene->GetRegistry().get<TagComponent>(entity).m_Tag;

		ImGuiTreeNodeFlags flags =  ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked())
			m_SelectedEntity = entity;

		// Right Click on Entity Node
		bool entitydestroyed = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
				entitydestroyed = true;

			ImGui::EndPopup();
		}

		if (opened)
			ImGui::TreePop();

		if (entitydestroyed)
		{
			m_Scene->DestroyEntity(entity);
			if (m_SelectedEntity == entity)
				m_SelectedEntity = {};
		}
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			auto& Tag = entity.GetComponent<TagComponent>().m_Tag; // Get Tag
			
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), Tag.c_str()); // Copy Tag Content To Buffer
			
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer))) // Draw Buffer Into InputText
				Tag = std::string(buffer);
		}


		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		
		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("Add Component");

		if (ImGui::BeginPopup("Add Component"))
		{
			if (ImGui::MenuItem("Tag Component"))
				if (!m_SelectedEntity.HasComponent<TagComponent>())
					m_SelectedEntity.AddComponent<TagComponent>();
				else
					SNAP_WARN("Entity With Tag {0}, Already Has Tag Component!", m_SelectedEntity.GetComponent<TagComponent>().m_Tag);

			if (ImGui::MenuItem("Transform Component"))
				if (!m_SelectedEntity.HasComponent<TransformComponent>())
					m_SelectedEntity.AddComponent<TransformComponent>();
				else
					SNAP_WARN("Entity With Tag {0}, Already Has Transform Component!", m_SelectedEntity.GetComponent<TagComponent>().m_Tag);

			if (ImGui::MenuItem("Camera Component"))
				if (!m_SelectedEntity.HasComponent<CameraComponent>())
					m_SelectedEntity.AddComponent<CameraComponent>(10.0f, -100.0f, 100.0f).m_IsMain = true;
				else
					SNAP_WARN("Entity With Tag {0}, Already Has Camera Component!", m_SelectedEntity.GetComponent<TagComponent>().m_Tag);

			if (ImGui::MenuItem("SpriteRenderer Component"))
				if (!m_SelectedEntity.HasComponent<SpriteRendererComponent>())
					m_SelectedEntity.AddComponent<SpriteRendererComponent>();
				else
					SNAP_WARN("Entity With Tag {0}, Already Has SpriteRenderer Component!", m_SelectedEntity.GetComponent<TagComponent>().m_Tag);

			if (ImGui::MenuItem("CircleRenderer Component"))
				if (!m_SelectedEntity.HasComponent<CircleRendererComponent>())
					m_SelectedEntity.AddComponent<CircleRendererComponent>();
				else
					SNAP_WARN("Entity With Tag {0}, Already Has CircleRenderer Component!", m_SelectedEntity.GetComponent<TagComponent>().m_Tag);


			if (ImGui::MenuItem("RigidBody2D Component"))
				if (!m_SelectedEntity.HasComponent<RigidBody2DComponent>())
					m_SelectedEntity.AddComponent<RigidBody2DComponent>();
				else
					SNAP_WARN("Entity With Tag {0}, Already Has RigidBody2D Component!", m_SelectedEntity.GetComponent<TagComponent>().m_Tag);
			
			if (ImGui::MenuItem("BoxCollider2D Component"))
				if (!m_SelectedEntity.HasComponent<BoxCollider2DComponent>())
					m_SelectedEntity.AddComponent<BoxCollider2DComponent>();
				else
					SNAP_WARN("Entity With Tag {0}, Already Has BoxCollider2D Component!", m_SelectedEntity.GetComponent<TagComponent>().m_Tag);
			
			if (ImGui::MenuItem("CircleCollider2D Component"))
				if (!m_SelectedEntity.HasComponent<CircleCollider2DComponent>())
					m_SelectedEntity.AddComponent<CircleCollider2DComponent>();
				else
					SNAP_WARN("Entity With Tag {0}, Already Has CircleCollider2D Component!", m_SelectedEntity.GetComponent<TagComponent>().m_Tag);
			
			if (ImGui::MenuItem("Script Component"))
				if (!m_SelectedEntity.HasComponent<ScriptComponent>())
					m_SelectedEntity.AddComponent<ScriptComponent>();
				else
					SNAP_WARN("Entity With Tag {0}, Already Has Script Component!", m_SelectedEntity.GetComponent<TagComponent>().m_Tag);

			ImGui::EndPopup();
		}
		ImGui::PopItemWidth();


		DrawComponent<TransformComponent>("Transform", entity, [&](TransformComponent& Component)
			{
				DrawVec3Control("Position", Component.m_Position);
				DrawVec3Control("Rotation", Component.m_Rotation, 0.0f, 0.1f);
				DrawVec3Control("Scale", Component.m_Scale);
			});

		DrawComponent<CameraComponent>("Camera", entity, [&](CameraComponent& Component)
			{
				bool Changed = false;

				int CamType = (int)Component.m_Camera.GetProjectionType();
				const char* CameraProjectionTypeString[2] = { "Perspective", "OrthoGraphic" };
				const char* CurrentProjectionTypeString = CameraProjectionTypeString[CamType];

				if (ImGui::BeginCombo("Camera Projection", CurrentProjectionTypeString))
				{
					for (size_t i = 0; i < 2; i++)
					{
						bool IsSelected = CurrentProjectionTypeString == CameraProjectionTypeString[i];
						if (ImGui::Selectable(CameraProjectionTypeString[i], IsSelected)
							&& CurrentProjectionTypeString != CameraProjectionTypeString[i])
						{
							// Change Projection To new Projection
							CurrentProjectionTypeString = CameraProjectionTypeString[i];
							Component.m_Camera.SetProjectionType((SceneCamera::ProjectionType)i);
							Changed = true;
						}

						if (IsSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

				// Draw Camera UI Parameters
				if (Component.m_Camera.GetProjectionType() == SceneCamera::ProjectionType::OrthoGraphic)
				{
					float Size = Component.m_Camera.GetOrthoGraphicSize();
					float NearClipPlane = Component.m_Camera.GetOrthoGraphicNear();
					float FarClipPlane = Component.m_Camera.GetOrthoGraphicFar();

					if (Changed)
					{
						Component.m_Camera.SetOrthoGraphic(Size, NearClipPlane, FarClipPlane);
						Changed = false;
					}

					if (ImGui::DragFloat("Size", &Size, 0.01f))
						Component.m_Camera.SetOrthoGraphicSize(Size);
					if (ImGui::DragFloat("NearClip", &NearClipPlane, 0.01f))
						Component.m_Camera.SetOrthoGraphicNear(NearClipPlane);
					if (ImGui::DragFloat("FarClip", &FarClipPlane, 0.01f))
						Component.m_Camera.SetOrthoGraphicFar(FarClipPlane);
				}
				else if (Component.m_Camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					float Fov = glm::degrees(Component.m_Camera.GetPerspectiveFov());
					float NearClipPlane = Component.m_Camera.GetPerspectiveNear();
					float FarClipPlane = Component.m_Camera.GetPerspectiveFar();

					if (Changed)
					{
						Component.m_Camera.SetPerspective(glm::radians(Fov), NearClipPlane, FarClipPlane);
						Changed = false;
					}

					if (ImGui::DragFloat("Fov", &Fov, 0.01f))
						Component.m_Camera.SetPerspectiveFov(glm::radians(Fov));
					if (ImGui::DragFloat("NearClip", &NearClipPlane, 0.01f))
						Component.m_Camera.SetPerspectiveNear(NearClipPlane);
					if (ImGui::DragFloat("FarClip", &FarClipPlane, 0.01f))
						Component.m_Camera.SetPerspectiveFar(FarClipPlane);
				}
			});

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [&](SpriteRendererComponent& Component)
			{
				glm::vec4& Color = Component.m_Color;
				ImGui::ColorEdit4("Color", glm::value_ptr(Color));
				
				ImGui::Button("Texture", { 100.0f, 0.0f });
				
				if (ImGui::BeginDragDropTarget())
				{
					if (const auto* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						auto filepath = std::filesystem::path(g_AssetPath) / path;

						if (filepath.extension().string() == ".png" || filepath.extension().string() == ".jpg")
						{
							SnapPtr<Texture2D> Tex = SnapPtr<Texture2D>(Texture2D::Creat(filepath.string()));
							Component.m_Texture = Tex;
						}
					}
					ImGui::EndDragDropTarget();
				}


				ImGui::DragFloat("TilingFactor", &Component.m_TilingFactor, 0.01f, 1.0f, 10.0f);
			});

		DrawComponent<CircleRendererComponent>("CircleRenderer", entity, [&](CircleRendererComponent& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.m_Color));
				ImGui::DragFloat("Thickness", &component.m_Thickness, 0.001f, 0.0f, 1.0f);
				ImGui::DragFloat("Fade", &component.m_Fade, 0.001f, 0.0f, 1.0f);
			});

		
		DrawComponent<RigidBody2DComponent>("RigidBody2D", entity, [&](RigidBody2DComponent& component)
		{
				bool Changed = false;

				int BodyType = (int)component.m_Type;
				const char* BodyTypeStrings[3] = { "Static", "Dynamic", "Kinematic" };
				const char* CurrentBodyTypeString = BodyTypeStrings[BodyType];

				if (ImGui::BeginCombo("Body Type", CurrentBodyTypeString))
				{
					for (size_t i = 0; i < 3; i++)
					{
						bool IsSelected = CurrentBodyTypeString == BodyTypeStrings[i];
						if (ImGui::Selectable(BodyTypeStrings[i], IsSelected))
						{
							// Change Type To new Type
							CurrentBodyTypeString = BodyTypeStrings[i];
							component.m_Type = (RigidBody2DComponent::RigidBodyType)i;
							Changed = true;
						}

						if (IsSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

				ImGui::Checkbox("Fixed Rotation", &component.m_FixedRotation);
		});

		DrawComponent<BoxCollider2DComponent>("BoxCollider2D", entity, [&](BoxCollider2DComponent& component)
			{
				ImGui::DragFloat2("Size", glm::value_ptr(component.m_Size), 0.01f);
				ImGui::DragFloat2("Offset", glm::value_ptr(component.m_Offset), 0.01f);

				ImGui::DragFloat("Density", &component.m_Density, 0.01f);
				ImGui::DragFloat("Friction", &component.m_Friction, 0.01f);
				ImGui::DragFloat("Restitution", &component.m_Restitution, 0.01f);

			});

		DrawComponent<CircleCollider2DComponent>("CircleCollider2D", entity, [&](CircleCollider2DComponent& component)
			{
				ImGui::DragFloat2("Offset", glm::value_ptr(component.m_Offset), 0.01f);
				ImGui::DragFloat("Raduis", &component.m_Raduis, 0.01f);

				ImGui::DragFloat("Density", &component.m_Density, 0.01f);
				ImGui::DragFloat("Friction", &component.m_Friction, 0.01f);
				ImGui::DragFloat("Restitution", &component.m_Restitution, 0.01f);
			});
		
		DrawComponent<ScriptComponent>("Script", entity, [entity, this](ScriptComponent& component) mutable
			{

				bool ScriptClassExist = false;

				auto& EnittyClasses = Scripting::ScriptingEngine::GetEntityClassesMap();

				if (Scripting::ScriptingEngine::IsEntityClassExist(component.ClassName))
					ScriptClassExist = true;


				char buffer[64];
				strcpy(buffer, component.ClassName.c_str());
				if (!ScriptClassExist)
					ImGui::PushStyleColor(ImGuiCol_Text, { 0.9f, 0.2f, 0.3f, 1.0f });
				else
					ImGui::PushStyleColor(ImGuiCol_Text, { 0.2f, 0.9f, 0.3f, 1.0f });
				
				if (ImGui::InputText("ClassName", buffer, sizeof(buffer)))
				{
					component.ClassName = std::string(buffer);
				}

				ImGui::PopStyleColor();

				// Draw Script Fields
				// Check Again May InputText Field Changed
				bool SceneRunning = m_Scene->IsRunning();
				if (SceneRunning)
				{
					if (Scripting::ScriptingEngine::IsEntityClassExist(component.ClassName))
					{
						auto& ScriptInstance = Scripting::ScriptingEngine::GetScriptInstance(entity.GetComponent<IDComponent>().ID);
						if (!ScriptInstance)
							return;

						auto& FieldsMap = ScriptInstance->GetScriptClass()->GetFieldsMap();
						for (auto& Field : FieldsMap)
						{
							switch (Field.second.m_Type)
							{
							case Scripting::ScriptFieldDataType::Int:
							{
								int value = ScriptInstance->GetFieldValue<int>(Field.first);
								if (ImGui::DragInt(Field.first.c_str(), &value))
								{
									ScriptInstance->SetFieldValue<int>(Field.first, value);
								}
							}
							break;
							case Scripting::ScriptFieldDataType::Float:
							{
								float value = ScriptInstance->GetFieldValue<float>(Field.first);
								if (ImGui::DragFloat(Field.first.c_str(), &value))
								{
									ScriptInstance->SetFieldValue<float>(Field.first, value);
								}
							}
							break;
							case Scripting::ScriptFieldDataType::Bool:
							{
								bool value = ScriptInstance->GetFieldValue<bool>(Field.first);
								if (ImGui::Checkbox(Field.first.c_str(), &value))
								{
									ScriptInstance->SetFieldValue<bool>(Field.first, value);
								}
							}
							break;
							case Scripting::ScriptFieldDataType::Vec2:
							{
								glm::vec2 value = ScriptInstance->GetFieldValue<glm::vec2>(Field.first);
								glm::vec2 x = { 5.0f, 1.0f };
								if (ImGui::DragFloat2(Field.first.c_str(), glm::value_ptr(value)))
								{
									ScriptInstance->SetFieldValue<glm::vec2>(Field.first, value);
								}
							}
							break;
							case Scripting::ScriptFieldDataType::Vec3:
							{
								glm::vec3 x = { 7.0f, 2.0f, 13.0f };
								ImGui::DragFloat3(Field.first.c_str(), glm::value_ptr(x));
							}
							break;
							default:
								break;
							}
						}
					}
				}
				else
				{
					if (Scripting::ScriptingEngine::IsEntityClassExist(component.ClassName))
					{
						auto& ScriptFieldMap = Scripting::ScriptingEngine::GetScriptFieldMap(entity);
						auto& ScriptClass = Scripting::ScriptingEngine::GetScriptClass(component.ClassName);
						auto& FieldsMap = ScriptClass->GetFieldsMap();

						for (auto& [name, Field] : FieldsMap)
						{
							if (ScriptFieldMap.find(name) != ScriptFieldMap.end())
							{
								auto& FieldInstacne = ScriptFieldMap.at(name);
								if (Field.m_Type == Scripting::ScriptFieldDataType::Float)
								{
									float data = FieldInstacne.GetData<float>();
									if (ImGui::DragFloat(name.c_str(), &data))
									{
										FieldInstacne.SetData(&data);
									}
								}
							}
							else
							{
								if (Field.m_Type == Scripting::ScriptFieldDataType::Float)
								{
									float data = 0;
									if (ImGui::DragFloat(name.c_str(), &data))
									{
										Scripting::ScriptFieldInstance ScriptFieldInstance;
										ScriptFieldInstance.SetData(&data);
										ScriptFieldInstance.m_Field = Field;
										ScriptFieldMap[name] = ScriptFieldInstance;
									}
								}
							}
						}
					}
				}
			});
	}
}