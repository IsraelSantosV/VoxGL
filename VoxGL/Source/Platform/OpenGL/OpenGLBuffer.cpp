#include "VoxPch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace Vox
{
	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
	{
		VOX_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_RendererId);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
	{
		VOX_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_RendererId);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		VOX_PROFILE_FUNCTION();

		glDeleteBuffers(1, &m_RendererId);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		VOX_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		VOX_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::SetData(const void* data, uint32_t size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	// Index Buffer |||||||||||||||||||||||||||||||||||||||||||||||||||||||

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count) : m_Count(count)
	{
		VOX_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_RendererId);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		VOX_PROFILE_FUNCTION();

		glDeleteBuffers(1, &m_RendererId);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		VOX_PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererId);
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		VOX_PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}