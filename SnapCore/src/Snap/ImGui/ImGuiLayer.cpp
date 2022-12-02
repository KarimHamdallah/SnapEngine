#include "SnapPCH.h"
#include "ImGuiLayer.h"

#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <Snap/Core/Application.h>
#include <Snap/Events/Event.h>

#define BIND_FUNCTION(x) std::bind(&x, this, std::placeholders::_1)

namespace SnapEngine
{
    ImGuiLayer::ImGuiLayer()
        : Layer("ImGuiLayer")
    {
    }

    ImGuiLayer::~ImGuiLayer()
    {
        Destroy();
    }

    void ImGuiLayer::Start()
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer backends

        Application& app = Application::Get();

        ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)app.GetWindow().GetNativeWindow(), true);
        ImGui_ImplOpenGL3_Init("#version 460");
    }

    void ImGuiLayer::ProcessEvent(IEvent& e)
    {
        if (m_BlockEvents)
        {
            ImGuiIO& io = ImGui::GetIO();

            e.m_Handeled |= e.IsInCategory(EventCategory::EventCategoryMouse) & io.WantCaptureMouse;
            e.m_Handeled |= e.IsInCategory(EventCategory::EventCategoryKeyBoard) & io.WantCaptureKeyboard;
        }
    }

    void ImGuiLayer::Destroy()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiLayer::Begin()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiLayer::End()
    {
        ImGuiIO& io = ImGui::GetIO();

        // Set Size
        Application& app = Application::Get();
        io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }
}