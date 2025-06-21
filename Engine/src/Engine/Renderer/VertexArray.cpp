#include "pch.h"
#include "VertexArray.h"

#include "glad/glad.h"

#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/IndexBuffer.h"

namespace Engine {
	namespace Utils {
		static constexpr uint32_t CreateVertexArray() {
			uint32_t name;
			glCreateVertexArrays(1, &name);

			return name;
		}

		static constexpr GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type) {
			switch (type) {
				case ShaderDataType::Float:    return GL_FLOAT;
				case ShaderDataType::Float2:   return GL_FLOAT;
				case ShaderDataType::Float3:   return GL_FLOAT;
				case ShaderDataType::Float4:   return GL_FLOAT;
				case ShaderDataType::Mat3:     return GL_FLOAT;
				case ShaderDataType::Mat4:     return GL_FLOAT;
				case ShaderDataType::Int:      return GL_INT;
				case ShaderDataType::Int2:     return GL_INT;
				case ShaderDataType::Int3:     return GL_INT;
				case ShaderDataType::Int4:     return GL_INT;
				case ShaderDataType::Bool:     return GL_BOOL;
			}
			ENGINE_ASSERT(false, "Unknown shader data type!");
			return 0;
		}

		inline GLboolean Convert(bool value) {
			return value ? GL_TRUE : GL_FALSE;
		}

		static void SetUpFloatAttribute(uint32_t vao, uint32_t attribIndex, uint32_t bindingIndex, const BufferElement& element) {
			LOG_GL_TRACE("Enabling vertex attribute with ID: {}", attribIndex);
			glEnableVertexArrayAttrib(vao, attribIndex);
			LOG_GL_DEBUG("Adding vertex attribute with ID: {} (type: Float)", attribIndex);
			glVertexArrayAttribFormat(vao, attribIndex, static_cast<GLint>(element.GetComponentCount()), ShaderDataTypeToOpenGLBaseType(element.Type), Convert(element.Normalized), static_cast<GLuint>(element.Offset));
			glVertexArrayAttribBinding(vao, attribIndex, bindingIndex);
		}

		static void SetUpIntAttribute(uint32_t vao, uint32_t attribIndex, uint32_t bindingIndex, const BufferElement& element) {
			LOG_GL_TRACE("Enabling vertex attribute with ID: {}", attribIndex);
			glEnableVertexArrayAttrib(vao, attribIndex);
			LOG_GL_DEBUG("Adding vertex attribute with ID: {} (type: Int)", attribIndex);
			glVertexArrayAttribIFormat(vao, attribIndex, static_cast<GLint>(element.GetComponentCount()), ShaderDataTypeToOpenGLBaseType(element.Type), static_cast<GLuint>(element.Offset));
			glVertexArrayAttribBinding(vao, attribIndex, bindingIndex);
		}

		static void SetUpMatrixAttribute(uint32_t vao, uint32_t& attribIndex, uint32_t bindingIndex, const BufferElement& element) {
			uint32_t count = element.GetComponentCount();

			for (uint32_t i = 0; i < count; ++i) {
				LOG_GL_TRACE("Enabling vertex attribute with ID: {}", attribIndex);
				glEnableVertexArrayAttrib(vao, attribIndex);

				LOG_GL_DEBUG("Adding vertex attribute with ID: {} (type: Float)", attribIndex);
				glVertexArrayAttribFormat(vao, attribIndex, static_cast<GLint>(count), ShaderDataTypeToOpenGLBaseType(element.Type), Convert(element.Normalized), static_cast<GLuint>(element.Offset + sizeof(float) * count * i));
				glVertexArrayAttribBinding(vao, attribIndex, bindingIndex);
				++attribIndex;
			}

			if (element.UseInstancing) {
				if (element.Divisor == 0) {
					LOG_GL_WARN("Matrix attribute with ID: {} is marked for instancing but divisor is 0. Setting to 1.", attribIndex);
					glVertexArrayBindingDivisor(vao, bindingIndex, 1); // Set divisor to 1 for matrix attributes
				} else {
					glVertexArrayBindingDivisor(vao, bindingIndex, element.Divisor);
				}
			}

		}
	}

	VertexArray::VertexArray() : m_GLState(MakeRef<GLState>()) {

	}

	Ref<VertexArray> VertexArray::Create(const std::string& label) {
		auto vertexArray = MakeRef<VertexArray>();
		vertexArray->SetLabel(label);
		LOG_GL_DEBUG("Creating vertex array with ID: {}, Label: {}", vertexArray->RendererID());
		return vertexArray;
	}

	void VertexArray::Bind() const {
		glBindVertexArray(static_cast<IDType>(*this));
	}

	void VertexArray::Unbind() const {
		glBindVertexArray(0);
	}

	void VertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) {
		ENGINE_ASSERT(indexBuffer, "Index buffer cannot be null!");
		if (!indexBuffer)
			throw std::runtime_error("Index buffer cannot be null!");

		m_GLState->IndexBuffer = indexBuffer;

		glVertexArrayElementBuffer(static_cast<IDType>(*this), indexBuffer->RendererID());

		LOG_GL_DEBUG("Setting index buffer with ID: {}", indexBuffer->RendererID());
	}

	void VertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) {
		ENGINE_ASSERT(vertexBuffer, "Vertex buffer cannot be null!");
		ENGINE_ASSERT(vertexBuffer->HasLayout(), "Vertex buffer layout cannot be empty!");

		if (!vertexBuffer)
			throw std::runtime_error("Vertex buffer cannot be null!");

		if (!vertexBuffer->HasLayout())
			throw std::runtime_error("Vertex buffer layout cannot be empty!");

		m_GLState->VertexBuffers.push_back(vertexBuffer);
		const auto& layout = vertexBuffer->GetLayout();
		glVertexArrayVertexBuffer(static_cast<IDType>(*this),  m_GLState->BindingIndex, vertexBuffer->RendererID(), 0, static_cast<GLsizei>(layout.GetStride()));

		for (const auto& element : layout) {
			const auto type = Utils::ShaderDataTypeToOpenGLBaseType(element.Type);

			switch (element.Type) {
			case ShaderDataType::Float:
			case ShaderDataType::Float2:
			case ShaderDataType::Float3:
			case ShaderDataType::Float4: {
				Utils::SetUpFloatAttribute(static_cast<IDType>(*this), m_GLState->AttributeIndex, m_GLState->BindingIndex, element);
				m_GLState->AttributeIndex++;
				break;
			}

			case ShaderDataType::Int:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4:
			case ShaderDataType::Bool: {
				Utils::SetUpIntAttribute(static_cast<IDType>(*this), m_GLState->AttributeIndex, m_GLState->BindingIndex, element);
				m_GLState->AttributeIndex++;
				break;
			}

			case ShaderDataType::Mat3:
			case ShaderDataType::Mat4: {
				Utils::SetUpMatrixAttribute(static_cast<IDType>(*this), m_GLState->AttributeIndex, m_GLState->BindingIndex, element);
				break;
			}
			default :
				ENGINE_ASSERT(false, "Unknown ShaderDataType");
				throw std::exception("Unknown shaderDataType");
			}
		}

		m_GLState->BindingIndex++;


		LOG_GL_DEBUG("Adding vertex buffer with ID: {}", vertexBuffer->RendererID());
	}

	void VertexArray::SetLabel(const std::string& label) {
		if (label.empty() && m_GLState->Label.empty())
			return;

		glObjectLabel(GL_VERTEX_ARRAY, static_cast<IDType>(*this), -1, label.c_str());
		m_GLState->Label = label;
	}

	void VertexArray::Clear() {
		m_GLState->VertexBuffers.clear();
		m_GLState->IndexBuffer.reset();

		m_GLState->AttributeIndex = 0;
		m_GLState->BindingIndex = 0;

		glDeleteVertexArrays(1, &m_GLState->ID);
		m_GLState->ID = Utils::CreateVertexArray();
		auto label = m_GLState->Label;
		m_GLState->Label.clear();
		SetLabel(label);

		LOG_GL_DEBUG("Clearing vertex array with ID: {}", m_GLState->ID);
	}

	VertexArray::GLState::GLState() {
		ID = Utils::CreateVertexArray();
		LOG_GL_DEBUG("Creating vertex array with ID: {}", ID);
	}

	VertexArray::GLState::~GLState() {
		if (ID != 0) {
			LOG_GL_DEBUG("Removing vertex array with ID: {}", ID);
			glDeleteVertexArrays(1, &ID);
		}
	}
}
