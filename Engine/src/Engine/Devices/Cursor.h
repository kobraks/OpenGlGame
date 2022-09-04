#pragma once
#include "Engine/Core/Image.h"

namespace Game
{
	class Window;
	class Cursor
	{
		friend class Window;
	public:
		enum CursorType
		{
			Arrow,
			Center,
			Crosshair,
			Hand,
			HorizontalResize,
			VerdicalResize,
			IBeam
		};
	private:		
		Window* m_Window = nullptr;
		Image m_Image;

		Vector2i m_HotSpot;

		void* m_NativePointer = nullptr;

		bool m_Created = false;
		bool m_SystemCursor = false;
	public:
		Cursor();
		explicit Cursor(CursorType cursor);
		Cursor(const Image& image, Vector2i hotSpot);
		~Cursor();

		Vector2u GetPosition() const;
		void SetPosition(const Vector2u& position);

		const Image& GetImage() const;
		void SetImage(const Image& image);

		bool IsAttached() const;
	private:
		void Create();
		void Destroy();

		void Update();

		void Attach(Window* window);
		void Invalidate();
		bool IsCreated() const { return m_Created; };
	};
}
