#pragma once

#include <glad/glad.h>
#include <vector>

#include "Types.h"
#include "Log.h"

#include "Buffer.h"

namespace Game
{
	class VertexArray
	{
	public:
		using IdType = uint32_t;

	private:
		struct Internals
		{
			IdType Id = 0;
			
			Pointer<IndexBuffer> IndexBuffer = nullptr;
			std::vector<Pointer<VertexBuffer>> VertexBuffers;
			
			uint32_t VertexBufferIndex = 0;

			void SetIndexBuffer(const Pointer<Game::IndexBuffer> &indexBuffer);
			void AddVertexBuffer(const Pointer<VertexBuffer> &vertexBuffer);

			void Bind() const;
			void Unbind() const;

			Internals(IdType id);
			Internals();

			~Internals();
		};

		Pointer<Internals> m_Internals;

		VertexArray(IdType id);
	public:
		VertexArray();

		operator IdType() const { return m_Internals->Id; }
		IdType Id() const { return m_Internals->Id; }

		void Bind() const { m_Internals->Bind(); }
		void Unbind() const { m_Internals->Unbind(); }

		static VertexArray* GetDefault();

		void SetIndexBuffer(const Pointer<IndexBuffer> &indexBuffer) { m_Internals->SetIndexBuffer(indexBuffer); }
		void AddVertexBuffer(const Pointer<VertexBuffer> &vertexBuffer) { m_Internals->AddVertexBuffer(vertexBuffer); }

		const Pointer<IndexBuffer>& GetIndexBuffer() const { return m_Internals->IndexBuffer; }
		const std::vector<Pointer<VertexBuffer>>& GetVertexBuffers() const { return m_Internals->VertexBuffers; }
	};
}
