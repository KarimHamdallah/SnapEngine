#pragma once
#include <Snap/Core/Core.h>
#include <Snap/Events/Event.h>
#include <Snap/Renderer/Texture.h>
#include <Snap/Events/MouseEvent.h>

namespace SnapEngine
{
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void ImGuiRender();
		void ProcessEvents(IEvent& e);
		bool OnMouseScrolled(MouseScrollEvent& e);

		bool IsWindowHovered() { return IsHovered; }
		bool IsWindowFocused() { return IsFocused; }
	private:
		std::filesystem::path m_AssetDirectory;
		std::filesystem::path m_CurrentDirectory;
		SnapPtr<Texture2D> m_DirectoryIcon;
		SnapPtr<Texture2D> m_FileIcon;
		SnapPtr<Texture2D> m_FontIcon;
		SnapPtr<Texture2D> m_ImageIcon;

		float MouseOffset = 0.0f;
		bool IsHovered = false;
		bool IsFocused = false;
	};
}