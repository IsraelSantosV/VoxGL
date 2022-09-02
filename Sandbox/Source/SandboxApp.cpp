#include <Vox.h>

#include <glm/gtc/matrix_transform.hpp>

class ExampleLayer : public Vox::Layer
{
public:
	ExampleLayer() : Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f),
		m_CameraPosition(0.0f)
	{
		m_TriangleVA.reset(Vox::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5, 0.0f, 0.8f, 0.8f, 0.1f, 1.0f,
			0.5f, -0.5f, 0.0f, 0.2f, 0.8f, 0.3f, 1.0f,
			0.0f, 0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f
		};

		std::shared_ptr<Vox::VertexBuffer> triangleVB;
		triangleVB.reset(Vox::VertexBuffer::Create(vertices, sizeof(vertices)));
		Vox::BufferLayout layout = {
			{ Vox::ShaderDataType::Float3, "a_Position" },
			{ Vox::ShaderDataType::Float4, "a_Color" }
		};

		triangleVB->SetLayout(layout);
		m_TriangleVA->AddVertexBuffer(triangleVB);

		uint32_t indices[3] = { 0, 1, 2 };
		std::shared_ptr<Vox::IndexBuffer> triangleIB;
		triangleIB.reset(Vox::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_TriangleVA->SetIndexBuffer(triangleIB);

		m_SquareVA.reset(Vox::VertexArray::Create());

		float squareVertices[3 * 4] = {
			-0.5f, -0.5, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.5f, 0.5f, 0.0f,
			-0.5f, 0.5f, 0.0f
		};

		std::shared_ptr<Vox::VertexBuffer> squareVB;
		squareVB.reset(Vox::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

		squareVB->SetLayout({
			{ Vox::ShaderDataType::Float3, "a_Position" }
		});
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<Vox::IndexBuffer> squareIB;
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

		m_RGBShader.reset(new Vox::Shader(vertexSrc, fragmentSrc));

		std::string blueVertexSrc = R"(
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

		std::string blueFragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			
			void main()
			{
				color = vec4(0.2, 0.3, 0.8, 1.0);
			}
		)";

		m_BlueShader.reset(new Vox::Shader(blueVertexSrc, blueFragmentSrc));
	}

	void OnUpdate(Vox::Timestep ts) override
	{
		if (Vox::Input::IsKeyPressed(IN_KEY_LEFT))
		{
			m_CameraPosition.x += m_CameraMoveSpeed * ts;
		}
		else if (Vox::Input::IsKeyPressed(IN_KEY_RIGHT))
		{
			m_CameraPosition.x -= m_CameraMoveSpeed * ts;
		}

		if (Vox::Input::IsKeyPressed(IN_KEY_UP))
		{
			m_CameraPosition.y -= m_CameraMoveSpeed * ts;
		}
		else if (Vox::Input::IsKeyPressed(IN_KEY_DOWN))
		{
			m_CameraPosition.y += m_CameraMoveSpeed * ts;
		}

		if (Vox::Input::IsKeyPressed(IN_KEY_A))
		{
			m_CameraRotation += m_CameraRotationSpeed * ts;
		}
		else if(Vox::Input::IsKeyPressed(IN_KEY_D))
		{
			m_CameraRotation -= m_CameraRotationSpeed * ts;
		}

		Vox::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Vox::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		Vox::Renderer::BeginScene(m_Camera);

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		for (int y = 0; y < 20; y++)
		{
			for (int x = 0; x < 20; x++)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Vox::Renderer::Submit(m_BlueShader, m_SquareVA, transform);
			}
		}
		
		Vox::Renderer::Submit(m_RGBShader, m_TriangleVA);

		Vox::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
	}

	void OnEvent(Vox::Event& event) override
	{
	}
private:
	std::shared_ptr<Vox::Shader> m_RGBShader;
	std::shared_ptr<Vox::VertexArray> m_TriangleVA;

	std::shared_ptr<Vox::Shader> m_BlueShader;
	std::shared_ptr<Vox::VertexArray> m_SquareVA;

	Vox::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 5.0f;

	float m_CameraRotation = 0.0f;
	float m_CameraRotationSpeed = 180.0f;
};

class Sandbox : public Vox::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{
	}
};

Vox::Application* Vox::CreateApplication()
{
	return new Sandbox();
}