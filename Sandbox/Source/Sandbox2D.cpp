#include "Sandbox2D.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{
}

void Sandbox2D::OnAttach()
{
	VOX_PROFILE_FUNCTION();

	m_CheckerboardTexture = Vox::Texture2D::Create("Assets/Textures/Checkerboard.png");
}

void Sandbox2D::OnDetach()
{
	VOX_PROFILE_FUNCTION();
}

void Sandbox2D::OnUpdate(Vox::Timestep ts)
{
	VOX_PROFILE_FUNCTION();

	m_CameraController.OnUpdate(ts);

	{
		VOX_PROFILE_SCOPE("Render Init");
		Vox::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Vox::RenderCommand::Clear();
	}
	
	{
		VOX_PROFILE_SCOPE("Render Draw");
		Vox::Renderer2D::BeginScene(m_CameraController.GetCamera());
		// Hazel::Renderer2D::DrawRotatedQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, glm::radians(-45.0f), { 0.8f, 0.2f, 0.3f, 1.0f });
		Vox::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		Vox::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
		Vox::Renderer2D::DrawQuad({ -5.0f, -5.0f, -0.1f }, { 10.0f, 10.0f }, m_CheckerboardTexture, 10.0f);
		Vox::Renderer2D::DrawQuad({ -0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f }, m_CheckerboardTexture, 20.0f);

		Vox::Renderer2D::EndScene();
	}
}

void Sandbox2D::OnImGuiRender()
{
	VOX_PROFILE_FUNCTION();
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox2D::OnEvent(Vox::Event& e)
{
	m_CameraController.OnEvent(e);
}
