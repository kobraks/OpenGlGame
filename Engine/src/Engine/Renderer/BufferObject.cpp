#include "pch.h"
#include "BufferObject.h"


#include "Engine/Renderer/MappedBufferRegion.h"

#include "Engine/Utils/Renderer/GLEnumConverters.h"
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
		glNamedBufferData(m_State->RendererID, static_cast<GLsizeiptr>(size), data, Utils::EnumToGLConstant(usage));
	}

	void BufferObject::AllocateImmutable(const void* data, uint32_t size, BufferStorageFlags flags) {
		if (Utils::HasFlag(flags, BufferStorageFlags::MapPersistent) && !Utils::HasFlag(flags, BufferStorageFlags::MapWrite)) {
			ENGINE_ASSERT(false, "Persistent mapping requires MapWrite access.");
			throw std::runtime_error("Persistent mapping requires MapWrite access.");
		}

		m_State->Flags = flags;
		LOG_GL_INFO("BufferObject ID={} allocated immutable: size={}, flags=0x{:X}", m_State->RendererID, size, static_cast<uint32_t>(flags));
		glNamedBufferStorage(m_State->RendererID, static_cast<GLsizeiptr>(size), data, Utils::EnumToGLConstant(flags));

		if (Utils::HasFlag(flags, BufferStorageFlags::MapPersistent)) {
			LOG_GL_DEBUG("BufferObject ID={} persistently mapped with access={}", m_State->RendererID, Utils::DetermineAccessFromFlags(flags));
			m_State->PersistentContent = Map(Utils::DetermineAccessFromFlags(flags));
		}
	}

	void BufferObject::Bind() const {
		glBindBuffer(Utils::EnumToGLConstant(m_State->Target), m_State->RendererID);
		LOG_GL_TRACE("BufferObject::Bind(): ID={}, target={}", m_State->RendererID, m_State->Target);
	}

	void BufferObject::BindTo(BindingPointType bindingPoint) const {
		glBindBufferBase(Utils::EnumToGLConstant(m_State->Target), bindingPoint, m_State->RendererID);
		LOG_GL_TRACE("BufferObject::BindTo(): ID={}, target={}, bindingPoint={}", m_State->RendererID, m_State->Target, bindingPoint);
	}

	void BufferObject::BindRange(BindingPointType bindingPoint, uint32_t size, uint32_t offset) const {
		ENGINE_ASSERT(offset + size <= m_State->Size);

		glBindBufferRange(Utils::EnumToGLConstant(m_State->Target), bindingPoint, m_State->RendererID, offset, size);
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
			throw std::runtime_error("Download() failed: buffer is currently mapped.");
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
		glNamedBufferStorage(stagingBuffer, size, nullptr, 0);
		glCopyNamedBufferSubData(m_State->RendererID, stagingBuffer, offset, 0, size);

		Buffer buffer(size);

		if (const void* ptr = glMapNamedBuffer(stagingBuffer, GL_MAP_READ_BIT)) {
			buffer.CopyFrom(ptr, size);
		}

		glDeleteBuffers(1, &stagingBuffer);

		LOG_GL_TRACE("BufferObject::ReadSafeCopy(): ID={}, offset={}, size={}", m_State->RendererID, offset, size);
		return buffer;
	}

	Ref<MappedBufferRegion> BufferObject::Map(BufferAccess access) const {
		if (!m_State->Content.expired())
			return m_State->Content.lock();

		auto content = Ref<MappedBufferRegion>(new MappedBufferRegion(access, *this));
		m_State->Content = content;

		LOG_GL_DEBUG("BufferObject::Map(): ID={}, access={}, full size={} mapped", m_State->RendererID, access, m_State->Size);
		return content;
	}

	Ref<MappedBufferRegion> BufferObject::Map(BufferAccess access, uint32_t length, uint32_t offset) {
		if (!m_State->Content.expired())
			return m_State->Content.lock();

		auto content = Ref<MappedBufferRegion>(new MappedBufferRegion(access, *this, length, offset));
		m_State->Content = content;

		LOG_GL_DEBUG("BufferObject::Map(): ID={}, access={}, offset={}, length={}", m_State->RendererID, access, offset, length);
		return content;
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
