#pragma once
#include "Engine/Core/Base.h"

#include "Engine/Core/Buffer.h"
#include "Engine/Core/BufferView.h"
#include "Engine/Renderer/GraphicEnums.h"

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

	enum class BufferStorageMode {
		Mutable,
		Immutable,
	};

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

		IDType RendererID() const { return m_State->RendererID; }
		operator IDType() const { return m_State->RendererID; }

		void SetLabel(const std::string& label);
		std::string_view Label() const { return m_State->Label; }
	protected:
		BufferObject(uint32_t target, BufferUsage usageHint);
		virtual ~BufferObject() = default;

		void Allocate(const void* data, uint32_t size, BufferUsage usage, BufferStorageMode mode, BufferStorageFlags flags);

		void ForceUnMap();
	private:
		void AllocateMutable(const void* data, uint32_t size, BufferUsage usage);
		void AllocateImmutable(const void* data, uint32_t size, BufferStorageFlags flags);

		struct GLState {
			IDType RendererID = 0;
			BufferUsage Usage = BufferUsage::DynamicDraw;
			uint32_t Target = 0;
			uint32_t Size = 0;
			BufferStorageMode StorageMode = BufferStorageMode::Mutable;
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
			ENGINE_ASSERT(offset + sizeof(T) <= m_Buffer.Size());

			if (offset + sizeof(T) > m_Buffer.Size())
				throw std::out_of_range("");

			T value;
			std::memcpy(&value, static_cast<const std::byte*>(data) + offset, sizeof(T));

			return value;
		}

		BufferAccess Access() const { return m_Access; }

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
}