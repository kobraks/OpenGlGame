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

		VertexArray();

		explicit operator IDType() const { return m_GLState->ID; }
		IDType RendererID() const { return m_GLState->ID; }

		void Bind() const;
		void Unbind() const;

		void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer);
		void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer);

		void SetLabel(const std::string& label);
		[[nodiscard]] std::string_view Label() const { return m_GLState->Label; }

		void Clear();

		const Ref<IndexBuffer>& GetIndexBuffer() const { return m_GLState->IndexBuffer; }
		const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const { return m_GLState->VertexBuffers; }

	private:
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