#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/Clock.h"
#include "Engine/Core/Image.h"

namespace Engine {
	class Window;
	enum class CursorKind {
		System,
		Custom
	};

	class Cursor {
	public:
		enum CursorType {
			Arrow,
			Center,
			Crosshair,
			Hand,
			HorizontalResize,
			VerticalResize,
			IBeam
		};

	public:
		Cursor(const Cursor &) = delete;
		Cursor(Cursor &&) = delete;

		Cursor& operator=(const Cursor&) = delete;
		Cursor& operator=(Cursor &&) = delete;

		virtual ~Cursor() = default;

		Vector2u GetPosition() const;
		void SetPosition(const Vector2u &pos);

		Window* GetWindow() const { return m_Window; }

		bool IsAttached() const;
		bool IsActive() const;
		virtual bool IsAnimated() const { return false; }

		CursorKind GetKind() const { return m_Kind; }

		void* GetNativePointer() const { return m_NativePointer.get()->Ptr; }

		template<typename T>
		T* GetNativePointer() const { return static_cast<T*>(m_NativePointer.get()->Ptr); }
	protected:
		Cursor(CursorType type);
		Cursor(Ref<Image> image, const Vector2i &hotspot);

		virtual void Attach(Window * window);
		virtual void Invalidate();
		virtual void Apply();
	private:
		void Create(CursorType type);
		void Create(void *img, const Vector2i &hotspot);

		class NativePtr {
		public:
			NativePtr(CursorType type);
			NativePtr(const void *img, const Vector2i &hot);
			~NativePtr();

			NativePtr(const NativePtr& ptr) = delete;
			NativePtr(NativePtr&& ptr) noexcept = delete;

			NativePtr& operator=(const NativePtr& ptr) = delete;
			NativePtr& operator=(NativePtr&& ptr) noexcept = delete;


			void *Ptr;
		};

		Scope<NativePtr> m_NativePointer = nullptr;

		Window *m_Window = nullptr;
		CursorKind m_Kind = CursorKind::System;

		friend class Window;
	};

	class CustomCursor : public Cursor {
	public:
		explicit CustomCursor(Ref<Image> image, const Vector2i &hotspot = {});

		~CustomCursor() override = default;

		Ref<Image> GetImage() const { return m_Image; }
		Vector2i GetHotspot() const { return m_Hotspot; }

	private:
		Ref<Image> m_Image;
		Vector2i m_Hotspot;

	};

	class SystemCursor : public Cursor {
	public:
		explicit SystemCursor(CursorType type = Arrow);

		~SystemCursor() override = default;

		CursorType GetType() const { return m_Type; }
	private:
		CursorType m_Type;	
	};
}