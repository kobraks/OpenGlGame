#pragma once
#include "Engine/Core/Base.h"

#include <vector>
#include <string>
#include <string_view>

namespace Engine {
	class VertexBuffer;
	class IndexBuffer;

	class VertexArray {
	public:
		using IDType = uint32_t;

		static Ref<VertexArray> Create(const std::string& label = "");

		explicit operator IDType() const { return m_GLState->ID; }
		IDType RendererID() const { return m_GLState->ID; }

		void Bind() const;
		void Unbind() const;

		void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer);
		void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer);

		void SetLabel(const std::string& label);
		[[nodiscard]] std::string_view Label() const { return m_GLState->Label; }

		void Clear();

		const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const { return m_GLState->VertexBuffers; }

		Ref<IndexBuffer> GetIndexBuffer() const {
			ENGINE_ASSERT(m_GLState->IndexBuffer, "Index buffer is not set!");
			return m_GLState->IndexBuffer;
		}

		Ref<VertexBuffer> GetVertexBuffer(size_t index) const {
			if (m_GLState->VertexBuffers.empty())
				throw std::runtime_error("No vertex buffers available!");

			ENGINE_ASSERT(index < m_GLState->VertexBuffers.size(), "Vertex buffer index out of range!");
			if (index >= m_GLState->VertexBuffers.size())
				throw std::out_of_range("Vertex buffer index out of range!");

			return m_GLState->VertexBuffers[index];
		}

		bool HasIndexBuffer() const {
			return m_GLState->IndexBuffer != nullptr;
		}

		bool HasVertexBuffers() const {
			return !m_GLState->VertexBuffers.empty();
		}

	private:
		VertexArray();

		struct GLState {
			IDType ID{};

			Ref<IndexBuffer> IndexBuffer = nullptr;
			std::vector<Ref<VertexBuffer>> VertexBuffers;

			uint32_t AttributeIndex = 0;
			uint32_t BindingIndex = 0;

			std::string Label;

			GLState();
			~GLState();
		};

		Ref<GLState> m_GLState;
	};

}