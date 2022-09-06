#pragma once

#include "Vox.h"

class ExampleLayer : public Vox::Layer
{
public:
	ExampleLayer();
	virtual ~ExampleLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(Vox::Timestep ts) override;
	virtual void OnImGuiRender() override;
	void OnEvent(Vox::Event& e) override;
private:
	Vox::ShaderLibrary m_ShaderLibrary;
	Vox::Ref<Vox::Shader> m_Shader;
	Vox::Ref<Vox::VertexArray> m_VertexArray;

	Vox::Ref<Vox::Shader> m_FlatColorShader;
	Vox::Ref<Vox::VertexArray> m_SquareVA;

	Vox::Ref<Vox::Texture2D> m_Texture, m_ChernoLogoTexture;

	Vox::OrthoCameraController m_CameraController;
	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };
};