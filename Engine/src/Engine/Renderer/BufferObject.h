#pragma once
#include "Engine/Core/Base.h"

#include "Engine/Core/Buffer.h"
#include "Engine/Core/BufferView.h"

#include "Engine/Renderer/BufferEnums.h"

#include <string>
#include <string_view>

namespace Engine {
	class MappedBufferRegion;

	class BufferObject {
	public:
		using IDType = uint32_t;
		using BindingPointType = uint32_t;

		virtual ~BufferObject() = default;

		virtual void Bind() const;
		virtual void BindTo(BindingPointType bindingPoint) const;
		virtual void BindRange(BindingPointType bindingPoint, uint32_t size, uint32_t offset = 0) const;

		void Write(const void* data, uint32_t size, uint32_t offset = 0) { Write(BufferView(data, size), offset); }
		void Write(const BufferView& buffer, uint32_t offset = 0);
		
		Buffer Read(uint32_t offset = 0) const;
		Buffer Read(uint32_t size, uint32_t offset = 0) const;

		Buffer ReadSafeCopy(uint32_t offset = 0) const;
		Buffer ReadSafeCopy(uint32_t size, uint32_t offset = 0) const;

		Ref<MappedBufferRegion> Map(BufferAccess access = BufferAccess::ReadOnly) const;
		Ref<MappedBufferRegion> Map(BufferAccess access, uint32_t length, uint32_t offset = 0);

		bool IsMapped() const { return !m_State->Content.expired() || m_State->PersistentContent != nullptr; }

		void Invalidate();
		void Invalidate(uint32_t offset, uint32_t length);

		uint32_t Size() const { return m_State->Size; }
		BufferUsage UsageHint() const { return m_State->Usage; }
		BufferStorageMode Mode() const { return m_State->StorageMode; }
		BufferStorageFlags Flags() const { return m_State->Flags; }
		BufferTarget Target() const { return m_State->Target; }

		IDType RendererID() const { return m_State->RendererID; }
		explicit operator IDType() const { return m_State->RendererID; }

		void SetLabel(const std::string& label);
		std::string_view Label() const { return m_State->Label; }
	protected:
		BufferObject(BufferTarget target);

		void Allocate(uint32_t size, BufferStorageFlags flags) { Allocate(nullptr, size, flags); }
		void Allocate(uint32_t size, BufferUsage usageHint) { Allocate(nullptr, size, usageHint); }

		void Allocate(const void* data, uint32_t size, BufferStorageFlags flags);
		void Allocate(const void* data, uint32_t size, BufferUsage usageHint);

		void ClearMapping();
	private:
		void PrepareAllocate(uint32_t size, BufferStorageMode mode);

		void AllocateMutable(const void* data, uint32_t size, BufferUsage usage);
		void AllocateImmutable(const void* data, uint32_t size, BufferStorageFlags flags);

		struct GLState {
			IDType RendererID = 0;
			BufferUsage Usage = BufferUsage::DynamicDraw;
			BufferTarget Target = BufferTarget::None;
			uint32_t Size = 0;
			BufferStorageMode StorageMode = BufferStorageMode::Mutable;
			BufferStorageFlags Flags = BufferStorageFlags::None;
			std::weak_ptr<MappedBufferRegion> Content;

			Ref<MappedBufferRegion> PersistentContent = nullptr;

			std::string Label;

			GLState(BufferTarget target);
			~GLState();
		};

		Ref<GLState> m_State;
	};
}