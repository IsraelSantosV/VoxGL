#include <Vox.h>
#include <Vox/Core/EntryPoint.h>

#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Sandbox2D.h"

class ExampleLayer : public Vox::Layer
{
public:
	ExampleLayer() : Layer("Example"), m_CameraController(1280.0f / 720.0f)
	{
		m_TriangleVA = Vox::VertexArray::Create();

		float vertices[3 * 7] = {
			-0.5f, -0.5, 0.0f, 0.8f, 0.8f, 0.1f, 1.0f,
			0.5f, -0.5f, 0.0f, 0.2f, 0.8f, 0.3f, 1.0f,
			0.0f, 0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f
		};

		Vox::Ref<Vox::VertexBuffer> triangleVB;
		triangleVB.reset(Vox::VertexBuffer::Create(vertices, sizeof(vertices)));
		Vox::BufferLayout layout = {
			{ Vox::ShaderDataType::Float3, "a_Position" },
			{ Vox::ShaderDataType::Float4, "a_Color" }
		};

		triangleVB->SetLayout(layout);
		m_TriangleVA->AddVertexBuffer(triangleVB);

		uint32_t indices[3] = { 0, 1, 2 };
		Vox::Ref<Vox::IndexBuffer> triangleIB;
		triangleIB.reset(Vox::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_TriangleVA->SetIndexBuffer(triangleIB);

		m_SquareVA = Vox::VertexArray::Create();

		float squareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};

		Vox::Ref<Vox::VertexBuffer> squareVB;
		squareVB.reset(Vox::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

		squareVB->SetLayout({
			{ Vox::ShaderDataType::Float3, "a_Position" },
			{ Vox::ShaderDataType::Float2, "a_TexCoord" }
		});
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		Vox::Ref<Vox::IndexBuffer> squareIB;
		squareIB.reset(Vox::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;
			
			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;
			
			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		)";

		m_RGBShader = Vox::Shader::Create("VertexPosColor", vertexSrc, fragmentSrc);

		std::string flatColorShaderVertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			
			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string flatColorShaderFragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			uniform vec3 u_Color;
			
			void main()
			{
				color = vec4(u_Color, 1.0);
			}
		)";

		m_FlatColorShader = Vox::Shader::Create("FlatColor", flatColorShaderVertexSrc, flatColorShaderFragmentSrc);
		
		auto textureShader = m_ShaderLibrary.Load("Assets/Shaders/Texture.glsl");
		
		m_Texture = Vox::Texture2D::Create("Assets/Textures/Checkerboard.png");
		m_BlendingTexture = Vox::Texture2D::Create("Assets/Textures/LogoTest.png");
	
		std::dynamic_pointer_cast<Vox::OpenGLShader>(textureShader)->Bind();
		std::dynamic_pointer_cast<Vox::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);
	}

	void OnUpdate(Vox::Timestep ts) override
	{
		m_CameraController.OnUpdate(ts);

		Vox::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Vox::RenderCommand::Clear();

		Vox::Renderer::BeginScene(m_CameraController.GetCamera());

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		std::dynamic_pointer_cast<Vox::OpenGLShader>(m_FlatColorShader)->Bind();
		std::dynamic_pointer_cast<Vox::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);

		for (int y = 0; y < 20; y++)
		{
			for (int x = 0; x < 20; x++)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Vox::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
			}
		}

		auto textureShader = m_ShaderLibrary.Get("Texture");
		
		m_Texture->Bind();
		Vox::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		m_BlendingTexture->Bind();
		Vox::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)));

		//Vox::Renderer::Submit(m_RGBShader, m_TriangleVA);

		Vox::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");

		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));

		ImGui::End();
	}

	void OnEvent(Vox::Event& e) override
	{
		m_CameraController.OnEvent(e);
	}
private:
	Vox::ShaderLibrary m_ShaderLibrary;
	Vox::Ref<Vox::Shader> m_RGBShader;
	Vox::Ref<Vox::VertexArray> m_TriangleVA;

	Vox::Ref<Vox::Shader> m_FlatColorShader;
	Vox::Ref<Vox::VertexArray> m_SquareVA;

	Vox::Ref<Vox::Texture2D> m_Texture;
	Vox::Ref<Vox::Texture2D> m_BlendingTexture;

	Vox::OrthoCameraController m_CameraController;

	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.7f };
};

class Sandbox : public Vox::Application
{
public:
	Sandbox()
	{
		//PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
	}

	~Sandbox()
	{
	}
};

Vox::Application* Vox::CreateApplication()
{
	return new Sandbox();
}