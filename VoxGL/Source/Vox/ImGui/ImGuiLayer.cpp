#include "VoxPch.h"
#include "ImGuiLayer.h"

#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"

#include "Vox/Core/Application.h"

#include "Vox/Tools/ThemePalette.h"

//Temporary
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "ImGuizmo.h"

namespace Vox
{
	ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer")
	{
	}

	void ImGuiLayer::OnAttach()
	{
		VOX_PROFILE_FUNCTION();

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		io.Fonts->AddFontFromFileTTF("Assets/Fonts/JetBrainsMono-Bold.ttf", Window::m_HighDpiScaleFactor * 18.0f);
		io.Fonts->AddFontFromFileTTF("Assets/Fonts/JetBrainsMono-Italic.ttf", Window::m_HighDpiScaleFactor * 18.0f);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("Assets/Fonts/JetBrainsMono-Regular.ttf", Window::m_HighDpiScaleFactor * 18.0f);

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		style.ScaleAllSizes(Window::m_HighDpiScaleFactor);

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		SetThemeColors();

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::SetThemeColors()
	{
		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = Vox::Theme::WindowColor;

		colors[ImGuiCol_Header] = Vox::Theme::HeaderColor;
		colors[ImGuiCol_HeaderHovered] = Vox::Theme::HeaderHoveredColor;
		colors[ImGuiCol_HeaderActive] = Vox::Theme::HeaderActiveColor;

		colors[ImGuiCol_Button] = Vox::Theme::ButtonColor;
		colors[ImGuiCol_ButtonHovered] = Vox::Theme::ButtonHoveredColor;
		colors[ImGuiCol_ButtonActive] = Vox::Theme::ButtonActiveColor;

		colors[ImGuiCol_FrameBg] = Vox::Theme::FrameColor;
		colors[ImGuiCol_FrameBgHovered] = Vox::Theme::FrameHoveredColor;
		colors[ImGuiCol_FrameBgActive] = Vox::Theme::FrameActiveColor;

		colors[ImGuiCol_Tab] = Vox::Theme::TabColor;
		colors[ImGuiCol_TabHovered] = Vox::Theme::TabHoveredColor;
		colors[ImGuiCol_TabActive] = Vox::Theme::TabActiveColor;
		colors[ImGuiCol_TabUnfocused] = Vox::Theme::TabUnfocusedColor;
		colors[ImGuiCol_TabUnfocusedActive] = Vox::Theme::TabUnfocusedActiveColor;

		colors[ImGuiCol_TitleBg] = Vox::Theme::TitleColor;
		colors[ImGuiCol_TitleBgActive] = Vox::Theme::TitleActiveColor;
		colors[ImGuiCol_TitleBgCollapsed] = Vox::Theme::TitleCollapseColor;
	}

	void ImGuiLayer::OnDetach()
	{
		VOX_PROFILE_FUNCTION();

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnEvent(Event& e)
	{
		if (m_BlockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			e.Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}

	void ImGuiLayer::Begin()
	{
		VOX_PROFILE_FUNCTION();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void ImGuiLayer::End()
	{
		VOX_PROFILE_FUNCTION();

		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

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
