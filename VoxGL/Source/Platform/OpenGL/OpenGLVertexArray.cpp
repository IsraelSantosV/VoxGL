#include "VoxPch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Vox
{
    static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
    {
        switch (type)
        {
            case Vox::ShaderDataType::Float:	return GL_FLOAT;
            case Vox::ShaderDataType::Float2:	return GL_FLOAT;
            case Vox::ShaderDataType::Float3:	return GL_FLOAT;
            case Vox::ShaderDataType::Float4:	return GL_FLOAT;
            case Vox::ShaderDataType::Mat3:		return GL_FLOAT;
            case Vox::ShaderDataType::Mat4:		return GL_FLOAT;
            case Vox::ShaderDataType::Int:		return GL_INT;
            case Vox::ShaderDataType::Int2:		return GL_INT;
            case Vox::ShaderDataType::Int3:		return GL_INT;
            case Vox::ShaderDataType::Int4:		return GL_INT;
            case Vox::ShaderDataType::Bool:		return GL_BOOL;
        }

        VOX_CORE_ASSERT(false, "Unknown ShaderDataType!");
        return 0;
    }

    OpenGLVertexArray::OpenGLVertexArray()
    {
        glCreateVertexArrays(1, &m_RendererId);
    }

    OpenGLVertexArray::~OpenGLVertexArray()
    {
        glDeleteVertexArrays(1, &m_RendererId);
    }

    void OpenGLVertexArray::Bind() const
    {
        glBindVertexArray(m_RendererId);
    }

    void OpenGLVertexArray::Unbind() const
    {
        glBindVertexArray(0);
    }

    void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
    {
        VOX_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");
        glBindVertexArray(m_RendererId);
        vertexBuffer->Bind();


        uint32_t index = 0;
        const auto& layout = vertexBuffer->GetLayout();
        for (const auto& element : layout)
        {
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index, element.GetComponentCount(),
                ShaderDataTypeToOpenGLBaseType(element.Type),
                element.Normalized ? GL_TRUE : GL_FALSE,
                layout.GetStride(),
                (const void*)element.Offset);

            index++;
        }

        m_VertexBuffers.push_back(vertexBuffer);
    }

    void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
    {
        glBindVertexArray(m_RendererId);
        indexBuffer->Bind();
        m_IndexBuffer = indexBuffer;
    }
}