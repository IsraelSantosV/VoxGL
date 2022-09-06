#pragma once

#include "Vox.h"

class Sandbox2D : public Vox::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(Vox::Timestep ts) override;
	virtual void OnImGuiRender() override;
	void OnEvent(Vox::Event& e) override;
private:
	Vox::OrthoCameraController m_CameraController;

	// Temporary
	Vox::Ref<Vox::VertexArray> m_SquareVA;
	Vox::Ref<Vox::Shader> m_FlatColorShader;

	Vox::Ref<Vox::Texture2D> m_CheckerboardTexture;

	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.7f, 1.0f };
};