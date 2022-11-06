#include "VoxPch.h"
#include "Vox/Renderer/Renderer2D.h"

#include "Vox/Renderer/VertexArray.h"
#include "Vox/Renderer/Shader.h"
#include "Vox/Renderer/UniformBuffer.h"
#include "Vox/Renderer/RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Vox
{
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
		float TilingFactor;

		int _EntityId;
	};

	struct CircleVertex
	{
		glm::vec3 WorldPosition;
		glm::vec3 LocalPosition;
		glm::vec4 Color;
		float Thickness;
		float Fade;

		int _EntityId;
	};

	struct LineVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;

		int _EntityId;
	};

	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 20000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32;

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;

		Ref<Shader> QuadShader;
		Ref<Texture2D> WhiteTexture;

		Ref<VertexArray> CircleVertexArray;
		Ref<VertexBuffer> CircleVertexBuffer;
		Ref<Shader> CircleShader;

		Ref<VertexArray> LineVertexArray;
		Ref<VertexBuffer> LineVertexBuffer;
		Ref<Shader> LineShader;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		uint32_t CircleIndexCount = 0;
		CircleVertex* CircleVertexBufferBase = nullptr;
		CircleVertex* CircleVertexBufferPtr = nullptr;

		uint32_t LineVertexCount = 0;
		LineVertex* LineVertexBufferBase = nullptr;
		LineVertex* LineVertexBufferPtr = nullptr;

		float LineWidth = 2.0f;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1;

		glm::vec4 QuadVertexPositions[4];

		Renderer2D::Statistics Stats;

		struct CameraData
		{
			glm::mat4 ViewProjection;
		};

		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;
	};

	static Renderer2DData m_Data;

	void Renderer2D::Init()
	{
		VOX_PROFILE_FUNCTION();

		m_Data.QuadVertexArray = VertexArray::Create();

		m_Data.QuadVertexBuffer = VertexBuffer::Create(m_Data.MaxVertices * sizeof(QuadVertex));
		m_Data.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float, "a_TexIndex" },
			{ ShaderDataType::Float, "a_TilingFactor" },
			{ ShaderDataType::Int, "a_EntityId" },
		});
		m_Data.QuadVertexArray->AddVertexBuffer(m_Data.QuadVertexBuffer);

		m_Data.QuadVertexBufferBase = new QuadVertex[m_Data.MaxVertices];

		uint32_t* quadIndices = new uint32_t[m_Data.MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < m_Data.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, m_Data.MaxIndices);
		m_Data.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		m_Data.CircleVertexArray = VertexArray::Create();

		m_Data.CircleVertexBuffer = VertexBuffer::Create(m_Data.MaxVertices * sizeof(CircleVertex));
		m_Data.CircleVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_WorldPosition" },
			{ ShaderDataType::Float3, "a_LocalPosition" },
			{ ShaderDataType::Float4, "a_Color"         },
			{ ShaderDataType::Float,  "a_Thickness"     },
			{ ShaderDataType::Float,  "a_Fade"          },
			{ ShaderDataType::Int,    "a_EntityId"      }
		});
		m_Data.CircleVertexArray->AddVertexBuffer(m_Data.CircleVertexBuffer);
		m_Data.CircleVertexArray->SetIndexBuffer(quadIB);
		m_Data.CircleVertexBufferBase = new CircleVertex[m_Data.MaxVertices];

		// Lines
		m_Data.LineVertexArray = VertexArray::Create();

		m_Data.LineVertexBuffer = VertexBuffer::Create(m_Data.MaxVertices * sizeof(LineVertex));
		m_Data.LineVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color"    },
			{ ShaderDataType::Int,    "a_EntityId" }
			});
		m_Data.LineVertexArray->AddVertexBuffer(m_Data.LineVertexBuffer);
		m_Data.LineVertexBufferBase = new LineVertex[m_Data.MaxVertices];

		m_Data.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		m_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		int32_t samplers[m_Data.MaxTextureSlots];
		for (uint32_t i = 0; i < m_Data.MaxTextureSlots; i++)
		{
			samplers[i] = i;
		}
			
		m_Data.QuadShader = Shader::Create("Resources/Shaders/Renderer2D_Quad.glsl");
		m_Data.CircleShader = Shader::Create("Resources/Shaders/Renderer2D_Circle.glsl");
		m_Data.LineShader = Shader::Create("Resources/Shaders/Renderer2D_Line.glsl");
		m_Data.TextureSlots[0] = m_Data.WhiteTexture;

		m_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		m_Data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		m_Data.QuadVertexPositions[2] = { 0.5f, 0.5f, 0.0f, 1.0f };
		m_Data.QuadVertexPositions[3] = { -0.5f, 0.5f, 0.0f, 1.0f };
		
		m_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);
	}

	void Renderer2D::Shutdown()
	{
		VOX_PROFILE_FUNCTION();
		delete[] m_Data.QuadVertexBufferBase;
	}

	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		VOX_PROFILE_FUNCTION();

		m_Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
		m_Data.CameraUniformBuffer->SetData(&m_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		VOX_PROFILE_FUNCTION();

		m_Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
		m_Data.CameraUniformBuffer->SetData(&m_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		VOX_PROFILE_FUNCTION();

		m_Data.CameraBuffer.ViewProjection = camera.GetViewProjectionMatrix();
		m_Data.CameraUniformBuffer->SetData(&m_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));
	
		StartBatch();
	}

	void Renderer2D::EndScene()
	{
		VOX_PROFILE_FUNCTION();
		Flush();
	}

	void Renderer2D::StartBatch()
	{
		m_Data.QuadIndexCount = 0;
		m_Data.QuadVertexBufferPtr = m_Data.QuadVertexBufferBase;

		m_Data.CircleIndexCount = 0;
		m_Data.CircleVertexBufferPtr = m_Data.CircleVertexBufferBase;

		m_Data.LineVertexCount = 0;
		m_Data.LineVertexBufferPtr = m_Data.LineVertexBufferBase;

		m_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::Flush()
	{
		if (m_Data.QuadIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)m_Data.QuadVertexBufferPtr - (uint8_t*)m_Data.QuadVertexBufferBase);
			m_Data.QuadVertexBuffer->SetData(m_Data.QuadVertexBufferBase, dataSize);

			// Bind textures
			for (uint32_t i = 0; i < m_Data.TextureSlotIndex; i++)
				m_Data.TextureSlots[i]->Bind(i);

			m_Data.QuadShader->Bind();
			RenderCommand::DrawIndexed(m_Data.QuadVertexArray, m_Data.QuadIndexCount);
			m_Data.Stats.DrawCalls++;
		}

		if (m_Data.CircleIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)m_Data.CircleVertexBufferPtr - (uint8_t*)m_Data.CircleVertexBufferBase);
			m_Data.CircleVertexBuffer->SetData(m_Data.CircleVertexBufferBase, dataSize);

			m_Data.CircleShader->Bind();
			RenderCommand::DrawIndexed(m_Data.CircleVertexArray, m_Data.CircleIndexCount);
			m_Data.Stats.DrawCalls++;
		}

		if (m_Data.LineVertexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)m_Data.LineVertexBufferPtr - (uint8_t*)m_Data.LineVertexBufferBase);
			m_Data.LineVertexBuffer->SetData(m_Data.LineVertexBufferBase, dataSize);

			m_Data.LineShader->Bind();
			RenderCommand::SetLineWidth(m_Data.LineWidth);
			RenderCommand::DrawLines(m_Data.LineVertexArray, m_Data.LineVertexCount);
			m_Data.Stats.DrawCalls++;
		}
	}

	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		VOX_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, color);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		VOX_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityId)
	{
		VOX_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		const float textureIndex = 0.0f; // White Texture
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		const float tilingFactor = 1.0f;

		if (m_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			m_Data.QuadVertexBufferPtr->Position = transform * m_Data.QuadVertexPositions[i];
			m_Data.QuadVertexBufferPtr->Color = color;
			m_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			m_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			m_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			m_Data.QuadVertexBufferPtr->_EntityId = entityId;
			m_Data.QuadVertexBufferPtr++;
		}

		m_Data.QuadIndexCount += 6;

		m_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, 
		const glm::vec4& tintColor, int entityId)
	{
		VOX_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		if (m_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < m_Data.TextureSlotIndex; i++)
		{
			if (*m_Data.TextureSlots[i] == *texture)
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (m_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
				NextBatch();

			textureIndex = (float)m_Data.TextureSlotIndex;
			m_Data.TextureSlots[m_Data.TextureSlotIndex] = texture;
			m_Data.TextureSlotIndex++;
		}

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			m_Data.QuadVertexBufferPtr->Position = transform * m_Data.QuadVertexPositions[i];
			m_Data.QuadVertexBufferPtr->Color = tintColor;
			m_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			m_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			m_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			m_Data.QuadVertexBufferPtr->_EntityId = entityId;
			m_Data.QuadVertexBufferPtr++;
		}

		m_Data.QuadIndexCount += 6;

		m_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		VOX_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		VOX_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness, float fade, int entityId)
	{
		VOX_PROFILE_FUNCTION();

		for (size_t i = 0; i < 4; i++)
		{
			m_Data.CircleVertexBufferPtr->WorldPosition = transform * m_Data.QuadVertexPositions[i];
			m_Data.CircleVertexBufferPtr->LocalPosition = m_Data.QuadVertexPositions[i] * 2.0f;
			m_Data.CircleVertexBufferPtr->Color = color;
			m_Data.CircleVertexBufferPtr->Thickness = thickness;
			m_Data.CircleVertexBufferPtr->Fade = fade;
			m_Data.CircleVertexBufferPtr->_EntityId = entityId;
			m_Data.CircleVertexBufferPtr++;
		}

		m_Data.CircleIndexCount += 6;

		m_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawLine(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color, int entityId)
	{
		m_Data.LineVertexBufferPtr->Position = p0;
		m_Data.LineVertexBufferPtr->Color = color;
		m_Data.LineVertexBufferPtr->_EntityId = entityId;
		m_Data.LineVertexBufferPtr++;

		m_Data.LineVertexBufferPtr->Position = p1;
		m_Data.LineVertexBufferPtr->Color = color;
		m_Data.LineVertexBufferPtr->_EntityId = entityId;
		m_Data.LineVertexBufferPtr++;

		m_Data.LineVertexCount += 2;
	}

	void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityId)
	{
		glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
		glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

		DrawLine(p0, p1, color, entityId);
		DrawLine(p1, p2, color, entityId);
		DrawLine(p2, p3, color, entityId);
		DrawLine(p3, p0, color, entityId);
	}

	void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityId)
	{
		glm::vec3 lineVertices[4];
		for (size_t i = 0; i < 4; i++)
			lineVertices[i] = transform * m_Data.QuadVertexPositions[i];

		DrawLine(lineVertices[0], lineVertices[1], color, entityId);
		DrawLine(lineVertices[1], lineVertices[2], color, entityId);
		DrawLine(lineVertices[2], lineVertices[3], color, entityId);
		DrawLine(lineVertices[3], lineVertices[0], color, entityId);
	}

	void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteRendererComponent& component, int entityId)
	{
		if (component.Texture)
		{
			DrawQuad(transform, component.Texture, component.TilingFactor, component.Color, entityId);
		}
		else
		{
			DrawQuad(transform, component.Color, entityId);
		}
	}

	float Renderer2D::GetLineWidth()
	{
		return m_Data.LineWidth;
	}

	void Renderer2D::SetLineWidth(float width)
	{
		m_Data.LineWidth = width;
	}

	void Renderer2D::ResetStats()
	{
		memset(&m_Data.Stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return m_Data.Stats;
	}
}
