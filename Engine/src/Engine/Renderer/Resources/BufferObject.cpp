#include "pch.h"
#include "BufferObject.h"


#include "Engine/Renderer/Resources/MappedBufferRegion.h"

#include "Engine/Renderer/OpenGL/GLEnumConverters.h"
#include "Engine/Utils/Renderer/EnumStringConverters.h"

#include <glad/glad.h>

namespace Engine {
	namespace Utils {
		static uint32_t CreateBuffer() {
			uint32_t name = 0;
			glCreateBuffers(1, &name);
			return name;
		}
	}

	BufferObject::BufferObject(BufferTarget target) : m_State(MakeRef<GLState>(target)) {
		LOG_GL_DEBUG("BufferObject created: ID={}, target={}", m_State->RendererID, m_State->Target);
	}

	void BufferObject::Allocate(const void* data, uint32_t size, BufferStorageFlags flags) {
		PrepareAllocate(size, BufferStorageMode::Immutable);
		AllocateImmutable(data, size, flags);
	}

	void BufferObject::Allocate(const void* data, uint32_t size, BufferUsage usageHint) {
		PrepareAllocate(size, BufferStorageMode::Mutable);
		AllocateMutable(data, size, usageHint);
	}

	void BufferObject::ClearMapping() {
		if (!m_State->Content.expired()) {
			m_State->Content.lock().reset();
		}

		m_State->Content.reset();

		if (m_State->PersistentContent) {
			m_State->PersistentContent = nullptr;
		}

		LOG_GL_DEBUG("BufferObject ID={} mapping cleared", m_State->RendererID);
	}

	void BufferObject::PrepareAllocate(uint32_t size, BufferStorageMode mode) {
		LOG_GL_TRACE("BufferObject::PrepareAllocate(): ID={}, size={}, mode={}", m_State->RendererID, size, mode);
		ClearMapping();

		m_State->Size = size;
		m_State->StorageMode = mode;
	}

	void BufferObject::AllocateMutable(const void* data, uint32_t size, BufferUsage usage) {
		m_State->Usage = usage;

		LOG_GL_INFO("BufferObject ID={} allocated mutable: size={}, usage={}", m_State->RendererID, size, Utils::ToString(usage));
		glNamedBufferData(m_State->RendererID, static_cast<GLsizeiptr>(size), data, OpenGL::EnumToGLConstant(usage));
	}

	void BufferObject::AllocateImmutable(const void* data, uint32_t size, BufferStorageFlags flags) {
		const bool presitent = Utils::HasFlag(flags, BufferStorageFlags::MapPersistent);
		const bool readable = Utils::HasFlag(flags, BufferStorageFlags::MapRead);
		const bool writable = Utils::HasFlag(flags, BufferStorageFlags::MapWrite);
		const bool coherent = Utils::HasFlag(flags, BufferStorageFlags::MapCoherent);

		if (presitent && !readable && !writable) {
			ENGINE_ASSERT(false, "Persistent mapping requires read or write access.");
			throw std::invalid_argument("Persistent mapping requires read or write access.");
		}

		if (coherent && !presitent) {
			ENGINE_ASSERT(false, "Coherent storage requires persistent storage.");
			throw std::invalid_argument("Coherent storage requires persistent storage.");
		}

		m_State->Flags = flags;
		LOG_GL_INFO("BufferObject ID={} allocated immutable: size={}, flags=0x{:X}", m_State->RendererID, size, static_cast<uint32_t>(flags));
		glNamedBufferStorage(m_State->RendererID, static_cast<GLsizeiptr>(size), data, OpenGL::EnumToGLConstant(flags));

		if (presitent) {
			const auto mappingFlags = Utils::GetPersistentMappingFlags(flags);
			LOG_GL_DEBUG("BufferObject ID={} persistently mapped with access={} and flags={}", m_State->RendererID, Utils::DetermineAccessFromFlags(flags), mappingFlags);
			m_State->PersistentContent = Map(mappingFlags);
		}
	}

	void BufferObject::Bind() const {
		glBindBuffer(OpenGL::EnumToGLConstant(m_State->Target), m_State->RendererID);
		LOG_GL_TRACE("BufferObject::Bind(): ID={}, target={}", m_State->RendererID, m_State->Target);
	}

	void BufferObject::BindTo(BindingPointType bindingPoint) const {
		glBindBufferBase(OpenGL::EnumToGLConstant(m_State->Target), bindingPoint, m_State->RendererID);
		LOG_GL_TRACE("BufferObject::BindTo(): ID={}, target={}, bindingPoint={}", m_State->RendererID, m_State->Target, bindingPoint);
	}

	void BufferObject::BindRange(BindingPointType bindingPoint, uint32_t size, uint32_t offset) const {
		ENGINE_ASSERT(offset + size <= m_State->Size);

		glBindBufferRange(OpenGL::EnumToGLConstant(m_State->Target), bindingPoint, m_State->RendererID, offset, size);
		LOG_GL_TRACE("BufferObject::BindRange(): ID={}, target={}, bindingPoint={}, size={}, offset={}", m_State->RendererID, m_State->Target, bindingPoint, size, offset);
	}

	void BufferObject::Write(const BufferView& buffer, uint32_t offset) {
		ENGINE_ASSERT(offset + buffer.Size() <= Size());
		if (offset + buffer.Size() > Size())
			throw std::out_of_range(fmt::format("Out of bounds access: offset={}, size={}, bufferSize={}", offset, buffer.Size(), Size()));

		glNamedBufferSubData(m_State->RendererID, offset, static_cast<GLsizeiptr>(buffer.Size()), buffer.Data());
		LOG_GL_TRACE("BufferObject::Write(): ID={}, offset={}, size={}", m_State->RendererID, offset, buffer.Size());
	}

	Buffer BufferObject::Read(uint32_t offset) const {
		return Read(m_State->Size - offset, offset);
	}
	
	Buffer BufferObject::Read(uint32_t size, uint32_t offset) const {
		if (IsMapped()) {
			ENGINE_ASSERT(false, "failed: Buffer is currently mapped.");
			throw std::runtime_error("BufferObject::Read() failed: buffer is currently mapped.");
		}

		ENGINE_ASSERT(offset + size <= Size());
		if (offset + size > Size())
			throw std::out_of_range(fmt::format("Out of bounds access: offset={}, size={}, bufferSize={}", offset, size, Size()));

		Buffer buffer(size);

		if (const void* ptr = glMapNamedBufferRange(m_State->RendererID, offset, size, GL_MAP_READ_BIT)) {
			std::memcpy(buffer.Data(), ptr, buffer.Size());
			glUnmapNamedBuffer(m_State->RendererID);
		}

		LOG_GL_TRACE("BufferObject::Read(): ID={}, offset={}, size={}", m_State->RendererID, offset, size);
		return buffer;
	}

	Buffer BufferObject::ReadSafeCopy(uint32_t offset) const {
		return ReadSafeCopy(m_State->Size - offset, offset);
	}

	Buffer BufferObject::ReadSafeCopy(uint32_t size, uint32_t offset) const {
		ENGINE_ASSERT(offset + size <= Size());
		if (offset + size > Size())
			throw std::out_of_range(fmt::format("Out of bounds access: offset={}, size={}, bufferSize={}", offset, size, Size()));

		auto stagingBuffer = Utils::CreateBuffer();
		glNamedBufferStorage(stagingBuffer, static_cast<GLsizeiptr>(size), nullptr, GL_MAP_READ_BIT);
		glCopyNamedBufferSubData(m_State->RendererID, stagingBuffer, offset, 0, size);

		Buffer buffer(size);

		if (const void* ptr = glMapNamedBuffer(stagingBuffer, GL_READ_ONLY)) {
			buffer.CopyFrom(ptr, size);
		}

		glUnmapNamedBuffer(stagingBuffer);
		glDeleteBuffers(1, &stagingBuffer);

		LOG_GL_TRACE("BufferObject::ReadSafeCopy(): ID={}, offset={}, size={}", m_State->RendererID, offset, size);
		return buffer;
	}

	Ref<MappedBufferRegion> BufferObject::Map(BufferMapFlags flags) const {
		if (!m_State->Content.expired()) {
			return m_State->Content.lock();
		}

		auto content = Ref<MappedBufferRegion>(new MappedBufferRegion(flags, *this));
		m_State->Content = content;

		LOG_GL_DEBUG("BufferObject::Map(): ID={}, flags={}, full size={} mapped", m_State->RendererID, flags, m_State->Size);
		return content;
	}

	Ref<MappedBufferRegion> BufferObject::Map(BufferMapFlags flags, uint32_t length, uint32_t offset) {
		if (!m_State->Content.expired())
			return m_State->Content.lock();

		auto content = Ref<MappedBufferRegion>(new MappedBufferRegion(flags, *this, length, offset));
		m_State->Content = content;

		LOG_GL_DEBUG("BufferObject::Map(): ID={}, flags={}, offset={}, length={}", m_State->RendererID, flags, offset, length);
		return content;
	}

	Ref<MappedBufferRegion> BufferObject::Map(BufferAccess access) const {
		switch (access) {
		case BufferAccess::ReadOnly:
			return Map(BufferMapFlags::Read);

		case BufferAccess::WriteOnly:
			return Map(BufferMapFlags::Write);

		case BufferAccess::ReadWrite:
			return Map(BufferMapFlags::Read | BufferMapFlags::Write);
		}

		throw std::invalid_argument("Invalid BufferAccess value");
	}

	Ref<MappedBufferRegion> BufferObject::Map(BufferAccess access, uint32_t length, uint32_t offset) {
		switch (access) {
		case BufferAccess::ReadOnly:
			return Map(BufferMapFlags::Read, length, offset);

		case BufferAccess::WriteOnly:
			return Map(BufferMapFlags::Write, length, offset);

		case BufferAccess::ReadWrite:
			return Map(BufferMapFlags::Read | BufferMapFlags::Write, length, offset);
		}

		throw std::invalid_argument("Invalid BufferAccess value");
	}

	void BufferObject::Invalidate() {
		glInvalidateBufferData(m_State->RendererID);
		LOG_GL_TRACE("BufferObject::Invalidate(): ID={} entire buffer invalidated", m_State->RendererID);
	}

	void BufferObject::Invalidate(uint32_t offset, uint32_t length) {
		ENGINE_ASSERT(offset + length <= Size());
		if (offset + length > Size())
			throw std::out_of_range("Out of bounds");

		glInvalidateBufferSubData(m_State->RendererID, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(length));
		LOG_GL_TRACE("BufferObject::Invalidate(): ID={}, offset={}, length={} invalidated", m_State->RendererID, offset, length);
	}

	void BufferObject::SetLabel(const std::string& label) {
		if (label.empty() && m_State->Label.empty())
			return;

		glObjectLabel(GL_BUFFER, m_State->RendererID, -1, label.data());
		m_State->Label = label;

		LOG_GL_DEBUG("BufferObject::SetLabel(): ID={}, label={}", m_State->RendererID, label);
	}

	BufferObject::GLState::GLState(BufferTarget target) : RendererID(Utils::CreateBuffer()), Target(target) {
	}

	BufferObject::GLState::~GLState() {
		LOG_GL_DEBUG("BufferObject destroyed: ID={}, target={}", RendererID, Utils::ToString(Target));
		glDeleteBuffers(1, &RendererID);
	}
}
