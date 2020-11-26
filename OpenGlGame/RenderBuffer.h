#pragma once

#include "Types.h"

#include "Texture.h"

namespace Game
{
	class RenderBuffer
	{
	public:
		using IdType = uint32_t;
	private:
		Pointer<IdType> m_RenderBuffer;

	protected:
		explicit RenderBuffer(IdType id);
	public:
		RenderBuffer();
		RenderBuffer(uint32_t width, uint32_t height, InternalFormat format);

		operator IdType() const { return *m_RenderBuffer; }
		IdType ID() const { return *this; };

		void Bind() const;
		
		void Storage(uint32_t width, uint32_t height, InternalFormat format);

		static RenderBuffer* GetDefault();
	};
}
