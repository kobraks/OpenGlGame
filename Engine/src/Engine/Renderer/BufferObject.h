#pragma once
#include "Engine/Core/Base.h"

#include "Engine/Core/Buffer.h"
#include "Engine/Core/BufferView.h"

#include "Engine/Renderer/GraphicEnums.h"

#include "Engine/Utils/CoreUtility.h"

#include <string>
#include <string_view>

namespace Engine {
	enum class BufferStorageFlags : uint32_t {
		None = 0,
		Dynamic = BIT(0),
		MapRead = BIT(1),
		MapWrite = BIT(2),
		MapPersistent = BIT(3),
		MapCoherent = BIT(4),
		ClientStorage = BIT(5)
	};

	enum class BufferAccess : uint32_t {
		ReadOnly,
		WriteOnly,
		ReadWrite
	};

	enum class BufferStorageMode {
		Mutable,
		Immutable,
	};

	namespace Utils {
		inline void AssertAccess(BufferAccess access, bool requiresRead, bool requiresWrite) {
			if (requiresRead && access == BufferAccess::WriteOnly) {
				ENGINE_ASSERT(false, "Attempted to read form buffer mapped as WriteOnly");
				throw std::runtime_error("Attempted to read from buffer mapped as WriteOnly");
			}

			if (requiresWrite && access == BufferAccess::ReadOnly) {
				ENGINE_ASSERT(false, "Attempted to write into buffer mapped as ReadOnly");
				throw std::runtime_error("Attempted to write into buffer mapped as ReadOnly");
			}
		}
	}

	class BufferContent;

	class BufferObject {
	public:
		using IDType = uint32_t;

		void Bind() const;
		void BindTo(uint32_t bindingPoint) const;
		void BindRange(uint32_t bindingPoint, uint32_t size, uint32_t offset = 0) const;

		void Upload(const BufferView& buffer, uint32_t offset = 0);
		
		Buffer Download(uint32_t offset = 0) const;
		Buffer Download(uint32_t size, uint32_t offset = 0) const;

		Buffer DownloadSafeCopy(uint32_t offset = 0) const;
		Buffer DownloadSafeCopy(uint32_t size, uint32_t offset = 0) const;

		Ref<BufferContent> GetContent(BufferAccess access) const;
		Ref<BufferContent> GetContent(BufferAccess access, uint32_t length, uint32_t offset = 0);

		bool IsMapped() const { return !m_State->Content.expired() || m_State->PersistentContent != nullptr; }

		uint32_t Size() const { return m_State->Size; }
		BufferUsage UsageHint() const { return m_State->Usage; }
		BufferStorageMode Mode() const { return m_State->StorageMode; }
		BufferStorageFlags Flags() const { return m_State->Flags; }

		IDType RendererID() const { return m_State->RendererID; }
		operator IDType() const { return m_State->RendererID; }

		void SetLabel(const std::string& label);
		std::string_view Label() const { return m_State->Label; }
	protected:
		BufferObject(uint32_t target);
		virtual ~BufferObject() = default;

		void Allocate(const void* data, uint32_t size, BufferStorageFlags flags);
		void Allocate(const void* data, uint32_t size, BufferUsage usage);

		void Allocate(const void* data, uint32_t size, BufferUsage usage, BufferStorageMode mode, BufferStorageFlags flags);

		void ForceUnMap();
	private:
		void PrepareAllocate(uint32_t size, BufferStorageMode mode);

		void AllocateMutable(const void* data, uint32_t size, BufferUsage usage);
		void AllocateImmutable(const void* data, uint32_t size, BufferStorageFlags flags);

		struct GLState {
			IDType RendererID = 0;
			BufferUsage Usage = BufferUsage::DynamicDraw;
			uint32_t Target = 0;
			uint32_t Size = 0;
			BufferStorageMode StorageMode = BufferStorageMode::Mutable;
			BufferStorageFlags Flags = BufferStorageFlags::None;
			std::weak_ptr<BufferContent> Content;

			Ref<BufferContent> PersistentContent = nullptr;

			std::string Label;

			GLState(uint32_t type);
			~GLState();
		};

		Ref<GLState> m_State;
	};

	class BufferContent {
		friend class BufferObject;
	public:
		~BufferContent();

		bool IsValid() const { return m_Data != nullptr && m_Size > 0; }
		explicit operator bool() const { return IsValid(); }

		const std::byte* Get() const;
		std::byte* Get();

		void Set(const std::byte* data, uint32_t size, uint32_t offset = 0);

		template<typename T>
		void Set(const T& value, uint32_t offset = 0) {
			return Set(&value, sizeof(T), offset);
		}

		template<typename T>
		T Get(const uint32_t offset = 0) const {
			const std::byte* data = Get();
			ENGINE_ASSERT(offset + sizeof(T) <= m_Size);

			if (offset + sizeof(T) > m_Size)
				throw std::out_of_range(fmt::format("Out of bounds access: offset={}, size={}, bufferSize={}", offset, sizeof(T), m_Size));

			T value;
			std::memcpy(&value, data + offset, sizeof(T));

			return value;
		}

		void Set(const BufferView& buffer, uint32_t offset = 0) { return Set(buffer.Data(), static_cast<uint32_t>(buffer.Size()), offset); }
		void Set(std::span<std::byte> bytes, uint32_t offset = 0) { return Set(bytes.data(), static_cast<uint32_t>(bytes.size_bytes()), offset); }

		template<typename T>
		void Set(std::span<const T> span, uint32_t offset = 0) { return Set(span.data(), static_cast<uint32_t>(span.size_bytes()), offset); }

		Buffer CopyTo(uint32_t size, uint32_t offset = 0) const;
		BufferView View(uint32_t size, uint32_t offset = 0) const;

		uint32_t Size() const { return m_Size; }

		BufferAccess Access() const { return m_Access; }

		std::span<std::byte> AsSpan(uint32_t offset = 0);
		std::span<const std::byte> AsSpan(uint32_t offset = 0) const;

		std::span<std::byte> AsSpan(uint32_t count, uint32_t offset);
		std::span<const std::byte> AsSpan(uint32_t count, uint32_t offset) const;

		template<typename T>
		std::span<T> AsSpan(uint32_t offset = 0);

		template<typename T>
		std::span<const T> AsSpan(uint32_t offset = 0) const;

		template<typename T>
		std::span<T> AsSpan(uint32_t elementCount, uint32_t offset);

		template<typename T>
		std::span<const T> AsSpan(uint32_t elementCount, uint32_t offset) const;

		template<typename T>
		bool IsAligned(uint32_t offset) const;

		BufferContent(const BufferContent&) = delete;
		BufferContent(BufferContent&&) noexcept = delete;

		BufferContent& operator=(const BufferContent&) = delete;
		BufferContent& operator=(BufferContent&&) noexcept = delete;
	protected:
		BufferContent(BufferAccess access, const BufferObject& buffer);
		BufferContent(BufferAccess access, const BufferObject& buffer, uint32_t size, uint32_t offset);

	private:
		std::byte* m_Data = nullptr;
		uint32_t m_Size = 0;
		uint32_t m_Offset = 0;

		const BufferAccess m_Access;
		const BufferObject& m_Buffer;
	};

	template <typename T>
	std::span<T> BufferContent::AsSpan(uint32_t offset) {
		Utils::AssertAccess(m_Access, true, false);

		ENGINE_ASSERT(offset <= m_Size);
		if (offset > m_Size)
			throw std::out_of_range(fmt::format("BufferContent::AsSpan<T>(offset): Out of bounds (offset={}, size={})", offset, m_Size));

		ENGINE_ASSERT(IsAligned<T>(offset), "BufferContent::AsSpan<T>(): Misaligned data access.");

		const auto count = m_Size - offset;
		ENGINE_ASSERT(count % sizeof(T) == 0, "BufferContent::AsSpan<T>(): Size is not a multiple of T.");

		return { reinterpret_cast<T*>(m_Data + offset), count / sizeof(T) };
	}

	template <typename T>
	std::span<const T> BufferContent::AsSpan(uint32_t offset) const {
		Utils::AssertAccess(m_Access, true, false);

		ENGINE_ASSERT(offset <= m_Size);
		if (offset > m_Size)
			throw std::out_of_range(fmt::format("BufferContent::AsSpan<T>(offset): Out of bounds (offset={}, size={})", offset, m_Size));

		ENGINE_ASSERT(IsAligned<T>(offset), "BufferContent::AsSpan<T>(): Misaligned data access.");

		const auto count = m_Size - offset;

		ENGINE_ASSERT(count % sizeof(T) == 0, "BufferContent::AsSpan<T>(): Size is not a multiple of T.");


		return { reinterpret_cast<const T*>(m_Data + offset), count / sizeof(T) };
	}

	template <typename T>
	std::span<T> BufferContent::AsSpan(uint32_t elementCount, uint32_t offset) {
		Utils::AssertAccess(m_Access, true, false);

		ENGINE_ASSERT(IsAligned<T>(offset), "BufferContent::AsSpan<T>(): Misaligned data access.");

		ENGINE_ASSERT((elementCount * sizeof(T)) + offset <= m_Size, "BufferContent::AsSpan<T>(): Size is not a multiple of T.");
		if ((elementCount * sizeof(T)) + offset > m_Size)
			throw std::out_of_range(fmt::format("BufferContent::AsSpan<T>(count, offset): Out of bounds (count={}, offset={}, size={})", elementCount, offset, m_Size));

		return { reinterpret_cast<T*>(m_Data + offset), elementCount };
	}

	template <typename T>
	std::span<const T> BufferContent::AsSpan(uint32_t elementCount, uint32_t offset) const {
		Utils::AssertAccess(m_Access, true, false);

		ENGINE_ASSERT(IsAligned<T>(offset), "BufferContent::AsSpan<T>(): Misaligned data access.");

		ENGINE_ASSERT((elementCount * sizeof(T)) + offset <= m_Size, "BufferContent::AsSpan<T>(): Size is not a multiple of T.");
		if ((elementCount * sizeof(T)) + offset > m_Size)
			throw std::out_of_range(fmt::format("BufferContent::AsSpan<T>(count, offset): Out of bounds (count={}, offset={}, size={})", elementCount, offset, m_Size));

		return { reinterpret_cast<const T*>(m_Data + offset), elementCount };
	}

	template <typename T>
	bool BufferContent::IsAligned(uint32_t offset) const {
		ENGINE_ASSERT(offset <= m_Size);
		if (offset > m_Size)
			throw std::out_of_range(fmt::format("BufferContent::IsAligned<T>(): Offset {} exceeds buffer size {}", offset, m_Size));

		return Utils::IsAlignTo<T>(m_Data, offset);
	}

	inline constexpr BufferStorageFlags operator|(BufferStorageFlags a, BufferStorageFlags b) {
		return static_cast<BufferStorageFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}

	inline constexpr BufferStorageFlags operator|=(BufferStorageFlags& a, BufferStorageFlags b) {
		return a = (a | b);
	}

	inline constexpr BufferStorageFlags operator & (BufferStorageFlags a, BufferStorageFlags b) {
		return static_cast<BufferStorageFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
	}

	inline constexpr BufferStorageFlags& operator &= (BufferStorageFlags& a, BufferStorageFlags b) {
		return a = a & b;
	}
}