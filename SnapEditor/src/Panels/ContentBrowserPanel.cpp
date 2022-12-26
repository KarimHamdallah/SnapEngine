#include "SnapPCH.h"
#include "ContentBrowserPanel.h"
#include <Snap/Core/asserts.h>
#include <Snap/Core/Input.h>
#include <Snap/Core/Application.h>

#include <imgui.h>

namespace SnapEngine
{
	static const std::filesystem::path s_AssetPath = "assets";
	static float padding = 16.0f;
	static float thumbnailsize = 128.0f;
	
	ContentBrowserPanel::ContentBrowserPanel()
		: m_CurrentDirectory(s_AssetPath)
	{
		m_DirectoryIcon = SnapPtr<Texture2D>(Texture2D::Creat("assets/Editor/folder.png"));
		m_FileIcon = SnapPtr<Texture2D>(Texture2D::Creat("assets/Editor/file.png"));
		m_FontIcon = SnapPtr<Texture2D>(Texture2D::Creat("assets/Editor/fontfile.png"));
		m_ImageIcon = SnapPtr<Texture2D>(Texture2D::Creat("assets/Editor/imagefile.png"));
	}

	void ContentBrowserPanel::ImGuiRender()
	{
		ImGui::Begin("ContentBrowser");

		if (m_CurrentDirectory != s_AssetPath)
		{
			if (ImGui::Button("<"))
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
		}

		float cellsize = thumbnailsize + padding;

		float panelwidth = ImGui::GetContentRegionAvail().x;
		int columncount = (int)(panelwidth / cellsize);
		if (columncount < 1)
			columncount = 1;

		ImGui::Columns(columncount, 0, false);

		for (auto& p : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			std::filesystem::path path = p.path(); // Path = assets/Editor
			auto& RelativePath = std::filesystem::relative(path, s_AssetPath); // RelativePath = Editor
			
			SnapPtr<Texture2D> Icon;
			if (p.is_directory())
				Icon = m_DirectoryIcon;
			else
			{
				std::string extension = RelativePath.extension().string();
				if (extension == ".png" || extension == ".jpg")
					Icon = m_ImageIcon;
				else if (extension == ".ttf")
					Icon = m_FontIcon;
				else
					Icon = m_FileIcon;
			}
			
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 0.0f));
			ImGui::ImageButton((ImTextureID)Icon->getID(), { thumbnailsize, thumbnailsize }, { 0.0f, 1.0f }, { 1.0f, 0.0f });
			ImGui::PopStyleColor();

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) // Is Folder
			{
				if (p.is_directory())
					m_CurrentDirectory /= path.filename();
			}
			ImGui::TextWrapped(RelativePath.filename().string().c_str());
			ImGui::NextColumn();
		}

		ImGui::Columns(1);

		ImGui::NewLine();
		ImGui::NewLine();

		ImGui::SliderFloat("Thumbnail Size", &thumbnailsize, 16.0f, 256.0f);
		ImGui::SliderFloat("Padding", &padding, 16.0f, 256.0f);

		IsHovered = ImGui::IsWindowHovered();
		IsFocused = ImGui::IsWindowFocused();

		if (IsHovered && IsFocused && Input::IsKeyPressed(Key::LeftControl))
		{
			thumbnailsize += MouseOffset;
			MouseOffset = 0.0f;
		}


		ImGui::End();
	}

	void ContentBrowserPanel::ProcessEvents(IEvent& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.DispatchEvent<MouseScrollEvent>(SNAP_BIND_FUNCTION(ContentBrowserPanel::OnMouseScrolled));
	}

	bool ContentBrowserPanel::OnMouseScrolled(MouseScrollEvent& e)
	{
		MouseOffset += e.GetOffsetY();
		return false;
	}
}