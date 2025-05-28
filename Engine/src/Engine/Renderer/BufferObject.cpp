#include "pch.h"
#include "BufferObject.h"

#include "Engine/Utils/OpenGlUtils.h"

#include <glad/glad.h>

namespace Engine {
	namespace Utils {
		static uint32_t CreateBuffer() {
			uint32_t name = 0;
			glCreateBuffers(1, &name);
			return name;
		}

		constexpr bool HasFlag(BufferStorageFlags value, BufferStorageFlags flag) {
			return (value & flag) != BufferStorageFlags::None;
		}

		static uint32_t ToGL(BufferAccess access) {
			switch (access) {
			case BufferAccess::ReadOnly:
				return GL_READ_ONLY;
			case BufferAccess::WriteOnly:
				return GL_WRITE_ONLY;
			case BufferAccess::ReadWrite:
				return GL_READ_WRITE;
			}

			return GL_READ_WRITE;
		}

		static uint32_t ToGL(BufferStorageFlags flags) {
			uint32_t glFlags = 0;

			if (HasFlag(flags, BufferStorageFlags::Dynamic)) glFlags |= GL_DYNAMIC_STORAGE_BIT;
			if (HasFlag(flags, BufferStorageFlags::MapRead)) glFlags |= GL_MAP_READ_BIT;
			if (HasFlag(flags, BufferStorageFlags::MapWrite)) glFlags |= GL_MAP_WRITE_BIT;
			if (HasFlag(flags, BufferStorageFlags::MapPersistent)) glFlags |= GL_MAP_PERSISTENT_BIT;
			if (HasFlag(flags, BufferStorageFlags::MapCoherent)) glFlags |= GL_MAP_COHERENT_BIT;
			if (HasFlag(flags, BufferStorageFlags::ClientStorage)) glFlags |= GL_CLIENT_STORAGE_BIT;

			return glFlags;
		}

		constexpr BufferAccess DetermineAccessFromFlags(BufferStorageFlags flags) {
			if (HasFlag(flags, BufferStorageFlags::MapRead) && HasFlag(flags, BufferStorageFlags::MapWrite))
				return BufferAccess::ReadWrite;
			if (HasFlag(flags, BufferStorageFlags::MapRead))
				return BufferAccess::ReadOnly;
			if (HasFlag(flags, BufferStorageFlags::MapWrite))
				return BufferAccess::WriteOnly;

			ENGINE_ASSERT(false, "Invalid mapping flags for determining BufferAccess.");
			throw std::runtime_error("BufferStorageFlags must include MapRead or MapWrite to determine access.");
		}
	}

	BufferObject::BufferObject(uint32_t target) : m_State(MakeRef<GLState>(target)) {
	}

	void BufferObject::Allocate(const void* data, uint32_t size, BufferStorageFlags flags) {
		PrepareAllocate(size, BufferStorageMode::Immutable);
		AllocateImmutable(data, size, flags);
	}

	void BufferObject::Allocate(const void* data, uint32_t size, BufferUsage usage) {
		PrepareAllocate(size, BufferStorageMode::Mutable);
		AllocateMutable(data, size, usage);
	}

	void BufferObject::Allocate(const void* data, uint32_t size, BufferUsage usage, BufferStorageMode mode, BufferStorageFlags flags) {
		PrepareAllocate(size, mode);

		if (mode == BufferStorageMode::Mutable)
			AllocateMutable(data, size, usage);
		else
			AllocateImmutable(data, size, flags);
	}

	void BufferObject::ForceUnMap() {
		if (!m_State->Content.expired()) {
			m_State->Content.lock().reset();
		}

		m_State->Content.reset();

		if (m_State->PersistentContent) {
			m_State->PersistentContent = nullptr;
		}
	}

	void BufferObject::PrepareAllocate(uint32_t size, BufferStorageMode mode) {
		ForceUnMap();

		m_State->Size = size;
		m_State->StorageMode = mode;
	}

	void BufferObject::AllocateMutable(const void* data, uint32_t size, BufferUsage usage) {
		m_State->Usage = usage;

		glNamedBufferData(m_State->RendererID, static_cast<GLsizeiptr>(size), data, Utils::ToGL(usage));
	}

	void BufferObject::AllocateImmutable(const void* data, uint32_t size, BufferStorageFlags flags) {
		if (Utils::HasFlag(flags, BufferStorageFlags::MapPersistent) && !Utils::HasFlag(flags, BufferStorageFlags::MapWrite)) {
			ENGINE_ASSERT(false, "Persistent mapping requires MapWrite access.");
			throw std::runtime_error("Persistent mapping requires MapWrite access.");
		}

		m_State->Flags = flags;
		glNamedBufferStorage(m_State->RendererID, static_cast<GLsizeiptr>(size), data, Utils::ToGL(flags));

		if (Utils::HasFlag(flags, BufferStorageFlags::MapPersistent)) {
			m_State->PersistentContent = GetContent(Utils::DetermineAccessFromFlags(flags));
		}
	}

	void BufferObject::Bind() const {
		glBindBuffer(m_State->Target, m_State->RendererID);
	}

	void BufferObject::BindTo(uint32_t bindingPoint) const {
		glBindBufferBase(m_State->Target, bindingPoint, m_State->RendererID);
	}

	void BufferObject::BindRange(uint32_t bindingPoint, uint32_t size, uint32_t offset) const {
		ENGINE_ASSERT(offset + size <= m_State->Size);

		glBindBufferRange(m_State->Target, bindingPoint, m_State->RendererID, offset, size);
	}

	void BufferObject::Upload(const BufferView& buffer, uint32_t offset) {
		ENGINE_ASSERT(offset + buffer.Size() <= Size());
		if (offset + buffer.Size() > Size())
			throw std::out_of_range(fmt::format("Out of bounds access: offset={}, size={}, bufferSize={}", offset, buffer.Size(), Size()));

		glNamedBufferSubData(m_State->RendererID, offset, static_cast<GLsizeiptr>(buffer.Size()), buffer.Data());
	}

	Buffer BufferObject::Download(uint32_t offset) const {
		return Download(m_State->Size - offset, offset);
	}
	
	Buffer BufferObject::Download(uint32_t size, uint32_t offset) const {
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

		return buffer;
	}

	Buffer BufferObject::DownloadSafeCopy(uint32_t offset) const {
		return DownloadSafeCopy(m_State->Size - offset, offset);
	}

	Buffer BufferObject::DownloadSafeCopy(uint32_t size, uint32_t offset) const {
		ENGINE_ASSERT(offset + size <= Size());
		if (offset + size > Size())
			throw std::out_of_range(fmt::format("Out of bounds access: offset={}, size={}, bufferSize={}", offset, size, Size()));

		auto stagingBuffer = Utils::CreateBuffer();
		glNamedBufferStorage(stagingBuffer, size, nullptr, 0);
		glCopyNamedBufferSubData(m_State->RendererID, stagingBuffer, offset, 0, m_State->Size);

		Buffer buffer(size);

		if (const void* ptr = glMapNamedBuffer(stagingBuffer, GL_MAP_READ_BIT)) {
			std::memcpy(buffer.Data(), ptr, buffer.Size());
		}

		return buffer;
	}

	Ref<BufferContent> BufferObject::GetContent(BufferAccess access) const {
		if (!m_State->Content.expired())
			return m_State->Content.lock();

		auto content = Ref<BufferContent>(new BufferContent(access, *this));
		m_State->Content = content;

		return content;
	}

	Ref<BufferContent> BufferObject::GetContent(BufferAccess access, uint32_t length, uint32_t offset) {
		if (!m_State->Content.expired())
			return m_State->Content.lock();

		auto content = Ref<BufferContent>(new BufferContent(access, *this, length, offset));
		m_State->Content = content;

		return content;
	}

	void BufferObject::SetLabel(const std::string& label) {
		if (label.empty() && m_State->Label.empty())
			return;

		glObjectLabel(GL_BUFFER, m_State->RendererID, -1, label.data());
		m_State->Label = label;
	}

	BufferObject::GLState::GLState(uint32_t type) : RendererID(Utils::CreateBuffer()), Target(type) {
	}

	BufferObject::GLState::~GLState() {
		glDeleteBuffers(1, &RendererID);
	}

	BufferContent::~BufferContent() {
		glUnmapNamedBuffer(m_Buffer.RendererID());
	}

	const std::byte* BufferContent::Get() const {
		Utils::AssertAccess(m_Access, true, false);

		return m_Data;
	}

	std::byte* BufferContent::Get() {
		Utils::AssertAccess(m_Access, true, false);

		return m_Data;
	}

	void BufferContent::Set(const std::byte* data, uint32_t size, uint32_t offset) {
		Utils::AssertAccess(m_Access, false, true);

		ENGINE_ASSERT(size + offset <= m_Size);
		if (size + offset > m_Size)
			throw std::out_of_range(fmt::format("Out of bounds access: offset={}, size={}, bufferSize={}", offset, size, m_Size));

		std::memcpy(m_Data + offset, data, size);
	}

	Buffer BufferContent::Copy(uint32_t size, uint32_t offset) const {
		Utils::AssertAccess(m_Access, true, false);

		ENGINE_ASSERT(size + offset <= m_Size);
		if (size + offset > m_Size)
			throw std::out_of_range(fmt::format("Out of bounds access: offset={}, size={}, bufferSize={}", offset, size, m_Size));

		Buffer buffer(size);
		std::memcpy(buffer.Data(), m_Data + offset, size);

		return buffer;
	}

	BufferView BufferContent::View(uint32_t size, uint32_t offset) const {
		Utils::AssertAccess(m_Access, true, false);

		ENGINE_ASSERT(size + offset <= m_Size);
		if (size + offset > m_Size)
			throw std::out_of_range(fmt::format("Out of bounds access: offset={}, size={}, bufferSize={}", offset, size, m_Size));

		return {m_Data + offset, size};
	}

	std::span<std::byte> BufferContent::AsSpan(uint32_t offset) {
		Utils::AssertAccess(m_Access, true, false);
		ENGINE_ASSERT(offset <= m_Size);
		if (offset > m_Size)
			throw std::out_of_range(fmt::format("BufferContent::AsSpan<T>(offset): Out of bounds (offset={}, size={})", offset, m_Size));

		const auto count = m_Size - offset;

		return { m_Data + offset, count };
	}

	std::span<const std::byte> BufferContent::AsSpan(uint32_t offset) const {
		Utils::AssertAccess(m_Access, true, false);
		ENGINE_ASSERT(offset <= m_Size);
		if (offset > m_Size)
			throw std::out_of_range(fmt::format("BufferContent::AsSpan<T>(offset): Out of bounds (offset={}, size={})", offset, m_Size));

		const auto count = m_Size - offset;

		return { m_Data + offset, count };
	}

	std::span<std::byte> BufferContent::AsSpan(uint32_t count, uint32_t offset) {
		Utils::AssertAccess(m_Access, true, false);
		ENGINE_ASSERT(count + offset <= m_Size);
		if (count + offset > m_Size)
			throw std::out_of_range(fmt::format("BufferContent::AsSpan<T>(count, offset): Out of bounds (count={}, offset={}, size={})", count, offset, m_Size));

		return { m_Data + offset, count };
	}

	std::span<const std::byte> BufferContent::AsSpan(uint32_t count, uint32_t offset) const {
		Utils::AssertAccess(m_Access, true, false);
		ENGINE_ASSERT(count + offset <= m_Size);
		if (count + offset > m_Size)
			throw std::out_of_range(fmt::format("BufferContent::AsSpan<T>(count, offset): Out of bounds (count={}, offset={}, size={})", count, offset, m_Size));

		return { m_Data + offset, count };
	}

	BufferContent::BufferContent(BufferAccess access, const BufferObject& buffer) : BufferContent(access, buffer, buffer.Size(), 0) {
	
	}

	BufferContent::BufferContent(BufferAccess access, const BufferObject& buffer, uint32_t size, uint32_t offset) : m_Access(access), m_Buffer(buffer), m_Size(size), m_Offset(offset) {
		ENGINE_ASSERT(buffer.Size() != 0, "Cannot map buffer (size = 0). Buffer must be allocated before mapping.");

		if (buffer.Size() == 0)
			throw std::runtime_error("Cannot map buffer (size = 0). Buffer must be allocated before mapping.");

		m_Data = static_cast<std::byte*>(glMapNamedBufferRange(buffer.RendererID(), offset, size, Utils::ToGL(m_Access)));
	}
}
