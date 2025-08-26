#include "pch.h"
#include "MappedBufferRegion.h"

#include "Engine/Renderer/BufferObject.h"
#include "Engine/Utils/Renderer/EnumStringConverters.h"
#include "Engine/Utils/Renderer/GLEnumConverters.h"

#include <glad/glad.h>

namespace Engine {
	MappedBufferRegion::~MappedBufferRegion() {
		LOG_GL_DEBUG("MappedBufferRegion destroyed: buffer={}, bufferTarget={}", m_Buffer.RendererID(), m_Buffer.Target());
		glUnmapNamedBuffer(m_Buffer.RendererID());
	}

	const std::byte* MappedBufferRegion::Read(uint32_t offset) const {
		Utils::AssertAccess(m_Access, true, false);

		ENGINE_ASSERT(offset <= m_Size);
		if (offset > m_Size)
			throw std::out_of_range(fmt::format("Out of bounds access: offset={}, bufferSize={}", offset, m_Size));

		return m_Data + offset;
	}

	std::byte* MappedBufferRegion::Read(uint32_t offset) {
		Utils::AssertAccess(m_Access, true, false);

		ENGINE_ASSERT(offset <= m_Size);
		if (offset > m_Size)
			throw std::out_of_range(fmt::format("Out of bounds access: offset={}, bufferSize={}", offset, m_Size));

		return m_Data + offset;
	}

	void MappedBufferRegion::Write(const std::byte* data, uint32_t size, uint32_t offset) {
		Utils::AssertAccess(m_Access, false, true);

		ENGINE_ASSERT(size + offset <= m_Size);
		if (size + offset > m_Size)
			throw std::out_of_range(fmt::format("Out of bounds access: offset={}, size={}, bufferSize={}", offset, size, m_Size));

		std::memcpy(m_Data + offset, data, size);
	}

	Buffer MappedBufferRegion::ToBuffer() const {
		Utils::AssertAccess(m_Access, true, false);
		Buffer buffer(m_Size);
		std::memcpy(buffer.Data(), m_Data, m_Size);
		LOG_GL_TRACE("MappedBufferRegion::ToBuffer(): {} bytes copied from mapped region to buffer", m_Size);
		return buffer;
	}

	Buffer MappedBufferRegion::Copy(uint32_t size, uint32_t offset) const {
		Buffer buffer(size);
		CopyTo(buffer, offset);

		return buffer;
	}

	void MappedBufferRegion::CopyTo(Buffer& buffer, uint32_t offset, uint32_t size) const {
		if (size == 0) {
			size = static_cast<uint32_t>(buffer.Size());
		}

		ENGINE_ASSERT(size > 0);

		ENGINE_ASSERT(size <= buffer.Size());
		if (size > buffer.Size())
			throw std::out_of_range(fmt::format("Destination buffer too small: required={}, available={}", size, buffer.Size()));

		ENGINE_ASSERT(offset + size <= m_Size);
		if (offset + size > m_Size)
			throw std::out_of_range(fmt::format("Out of bounds access: offset={}, size={}, bufferSize={}", offset, size, m_Size));

		buffer.Write(Read(offset), size);
		LOG_GL_TRACE("MappedBufferRegion::CopyTo(): {} bytes from offset {} to buffer", size, offset);
	}

	void MappedBufferRegion::CopyTo(MappedBufferRegion& buffer, uint32_t offset, uint32_t size) const {
		ENGINE_ASSERT(m_Buffer.RendererID() != buffer.m_Buffer.RendererID());

		if (size == 0) {
			size = m_Size - offset;
		}

		buffer.Write(View(offset, size));
		LOG_GL_TRACE("MappedBufferRegion::CopyTo(): {} bytes from offset {} to GPU buffer {}", size, offset, buffer.m_Buffer.RendererID());
	}

	void MappedBufferRegion::CopyTo(MappedBufferRegion& dstBuffer, uint32_t srcOffset, uint32_t dstOffset,
		uint32_t size) const {
		ENGINE_ASSERT(m_Buffer.RendererID() != dstBuffer.m_Buffer.RendererID());

		if (size == 0) {
			size = std::min(m_Size - srcOffset, dstBuffer.m_Size - dstOffset);
		}

		dstBuffer.Write(View(srcOffset, size), dstOffset);
		LOG_GL_TRACE("MappedBufferRegion::CopyTo(): {} bytes copied from buffer {} at offset {} -> buffer {} at offset {}", size, m_Buffer.RendererID(), srcOffset, dstBuffer.m_Buffer.RendererID(), dstOffset);
	}

	void MappedBufferRegion::CopyFrom(const BufferView& buffer, uint32_t offset, uint32_t size) {
		if (size == 0) {
			size = static_cast<uint32_t>(buffer.Size());
		}

		Write(buffer.Slice(0, size), offset);

		LOG_GL_TRACE("MappedBufferRegion::CopyFrom(): {} bytes written at offset {} from buffer", size, offset);
	}

	void MappedBufferRegion::CopyFrom(MappedBufferRegion& buffer, uint32_t offset, uint32_t size) {
		ENGINE_ASSERT(m_Buffer.RendererID() != buffer.m_Buffer.RendererID());

		if (size == 0) {
			size = m_Size - offset;
		}

		Write(buffer.View(0, size), offset);
		LOG_GL_TRACE("MappedBufferRegion::CopyFrom(): {} bytes copied from buffer {} -> buffer {} offset {}", size, buffer.m_Buffer.RendererID(), m_Buffer.RendererID(), offset);
	}

	void MappedBufferRegion::CopyFrom(MappedBufferRegion& srcBuffer, uint32_t srcOffset, uint32_t dstOffset, uint32_t size) {
		ENGINE_ASSERT(m_Buffer.RendererID() != srcBuffer.m_Buffer.RendererID());

		if (size == 0) {
			size = std::min(m_Size - dstOffset, srcBuffer.m_Size - srcOffset);
		}

		CopyFrom(srcBuffer.View(srcOffset, size), dstOffset, size);
		LOG_GL_TRACE("MappedBufferRegion::CopyFrom(): {} bytes copied from buffer {} offset {} -> buffer {} offset {}", size, srcBuffer.m_Buffer.RendererID(), srcOffset, m_Buffer.RendererID(), dstOffset);
	}

	BufferView MappedBufferRegion::View(uint32_t offset, uint32_t size) const {
		Utils::AssertAccess(m_Access, true, false);

		if (size == 0) {
			size = m_Size - offset;
		}

		ENGINE_ASSERT(size + offset <= m_Size);
		if (size + offset > m_Size)
			throw std::out_of_range(fmt::format("Out of bounds access: offset={}, size={}, bufferSize={}", offset, size, m_Size));

		LOG_GL_TRACE("MappedBufferRegion::View(): view from offset {} with size {} from buffer {}", offset, size, m_Buffer.RendererID());
		return { m_Data + offset, size };
	}

	void MappedBufferRegion::Flush(uint32_t offset, uint32_t size) {
		if (!(Utils::HasFlag(m_Buffer.Flags(), BufferStorageFlags::MapWrite)) ||
			!(Utils::HasFlag(m_Buffer.Flags(), BufferStorageFlags::MapPersistent))) {
			ENGINE_ASSERT(false, "Flush requires persistent mapped wirteable buffer.");
			throw std::runtime_error("Flush requires persistent mapped wirteable buffer.");
		}

		if (!Utils::HasFlag(m_Buffer.Flags(), BufferStorageFlags::MapCoherent)) {
			if (size == 0)
				size = m_Size;
		}

		ENGINE_ASSERT(offset + size <= m_Size);
		if (offset + size > m_Size)
			throw std::out_of_range(fmt::format("Flush out of bounds: offset={}, size={}, bufferSize={}", offset, size, m_Size));

		glFlushMappedBufferRange(m_Buffer.RendererID(), m_Offset + offset, size);
		LOG_GL_TRACE("MappedBufferRegion::Flush(): {} bytes flushed from offset {} in buffer {}", size, offset, m_Buffer.RendererID());
	}

	std::span<std::byte> MappedBufferRegion::AsSpan(uint32_t offset) {
		Utils::AssertAccess(m_Access, true, false);
		ENGINE_ASSERT(offset <= m_Size);
		if (offset > m_Size)
			throw std::out_of_range(fmt::format("MappedBufferRegion::AsSpan<T>(offset): Out of bounds (offset={}, size={})", offset, m_Size));

		const auto count = m_Size - offset;

		return { m_Data + offset, count };
	}

	std::span<const std::byte> MappedBufferRegion::AsSpan(uint32_t offset) const {
		Utils::AssertAccess(m_Access, true, false);
		ENGINE_ASSERT(offset <= m_Size);
		if (offset > m_Size)
			throw std::out_of_range(fmt::format("MappedBufferRegion::AsSpan<T>(offset): Out of bounds (offset={}, size={})", offset, m_Size));

		const auto count = m_Size - offset;

		return { m_Data + offset, count };
	}

	std::span<std::byte> MappedBufferRegion::AsSpan(uint32_t count, uint32_t offset) {
		Utils::AssertAccess(m_Access, true, false);
		ENGINE_ASSERT(count + offset <= m_Size);
		if (count + offset > m_Size)
			throw std::out_of_range(fmt::format("MappedBufferRegion::AsSpan<T>(count, offset): Out of bounds (count={}, offset={}, size={})", count, offset, m_Size));

		return { m_Data + offset, count };
	}

	std::span<const std::byte> MappedBufferRegion::AsSpan(uint32_t count, uint32_t offset) const {
		Utils::AssertAccess(m_Access, true, false);
		ENGINE_ASSERT(count + offset <= m_Size);
		if (count + offset > m_Size)
			throw std::out_of_range(fmt::format("MappedBufferRegion::AsSpan<T>(count, offset): Out of bounds (count={}, offset={}, size={})", count, offset, m_Size));

		return { m_Data + offset, count };
	}

	MappedBufferRegion::MappedBufferRegion(BufferAccess access, const BufferObject& buffer) : MappedBufferRegion(access, buffer, buffer.Size(), 0) {

	}

	MappedBufferRegion::MappedBufferRegion(BufferAccess access, const BufferObject& buffer, uint32_t size, uint32_t offset) : m_Access(access), m_Buffer(buffer), m_Size(size), m_Offset(offset) {
		LOG_GL_DEBUG("MappedBufferRegion created: buffer={}, bufferTarget={}, access={}, offset={}, size={}", buffer.RendererID(), Utils::ToString(buffer.Target()), Utils::ToString(access), offset, size);

		ENGINE_ASSERT(buffer.Size() != 0, "Cannot map buffer (size = 0). Buffer must be allocated before mapping.");

		if (buffer.Size() == 0)
			throw std::runtime_error("Cannot map buffer (size = 0). Buffer must be allocated before mapping.");

		m_Data = static_cast<std::byte*>(glMapNamedBufferRange(buffer.RendererID(), offset, size, Utils::EnumToGLConstant(m_Access)));

		ENGINE_ASSERT(m_Data != nullptr, "Mapping buffer failed. Buffer may not be allocated or OpenGL context is not current.");

		if (!m_Data) {
			LOG_GL_CRITICAL("Failed to map buffer {} (offset={}, size={})", buffer.RendererID(), offset, size);
		}
	}
}