#include "pch.h"
#include "Cursor.h"

#include "Engine/Core/Window.h"

#include <GLFW/glfw3.h>
#include <glm/vec4.hpp>

namespace Game
{
	static constexpr int Translate(Cursor::CursorType cursor)
	{
		switch(cursor)
		{
			case Cursor::Arrow:
				return GLFW_ARROW_CURSOR;
			case Cursor::Center:
				return GLFW_CENTER_CURSOR;
			case Cursor::Crosshair:
				return GLFW_CROSSHAIR_CURSOR;
			case Cursor::Hand:
				return GLFW_HAND_CURSOR;
			case Cursor::HorizontalResize:
				return GLFW_HRESIZE_CURSOR;
			case Cursor::VerdicalResize:
				return GLFW_VRESIZE_CURSOR;
			case Cursor::IBeam:
				return GLFW_IBEAM_CURSOR;
			default:
				return GLFW_ARROW_CURSOR;
		}
	}

	Cursor::Cursor() : Cursor(Arrow) { }

	Cursor::Cursor(const Cursor &cursor)
	{
		++(*cursor.m_RefCout);

		m_RefCout       = cursor.m_RefCout;
		m_Window        = cursor.m_Window;
		m_NativePointer = cursor.m_NativePointer;

		m_Image = cursor.m_Image;

		m_Created      = cursor.m_Created;
		m_SystemCursor = cursor.m_SystemCursor;

		m_HotSpot = cursor.m_HotSpot;
	}

	Cursor::Cursor(Cursor &&cursor) noexcept
	{
		std::swap(m_RefCout, cursor.m_RefCout);
		std::swap(m_Window, cursor.m_Window);
		std::swap(m_Image, cursor.m_Image);

		std::swap(m_Created, cursor.m_Created);
		std::swap(m_SystemCursor, cursor.m_SystemCursor);

		m_HotSpot = cursor.m_HotSpot;
	}

	Cursor& Cursor::operator=(const Cursor &cursor) noexcept
	{
		Destructor();

		++(*cursor.m_RefCout);

		m_RefCout       = cursor.m_RefCout;
		m_Window        = cursor.m_Window;
		m_NativePointer = cursor.m_NativePointer;

		m_Image = cursor.m_Image;

		m_Created      = cursor.m_Created;
		m_SystemCursor = cursor.m_SystemCursor;

		m_HotSpot = cursor.m_HotSpot;

		return *this;
	}

	Cursor& Cursor::operator=(Cursor &&cursor) noexcept
	{
		Destructor();

		std::swap(m_RefCout, cursor.m_RefCout);
		std::swap(m_Window, cursor.m_Window);
		std::swap(m_Image, cursor.m_Image);

		std::swap(m_Created, cursor.m_Created);
		std::swap(m_SystemCursor, cursor.m_SystemCursor);

		m_HotSpot = cursor.m_HotSpot;

		return *this;
	}

	Cursor::Cursor(CursorType cursor)
	{
		m_RefCout = new size_t(0);

		m_NativePointer = glfwCreateStandardCursor(Translate(cursor));

		if(m_NativePointer)
			m_Created = true;
	}

	Cursor::Cursor(const Image &image, Vector2i hotSpot) : m_Image(image),
	                                                       m_HotSpot(hotSpot)
	{
		m_RefCout = new size_t(0);
		Create();
	}

	Cursor::~Cursor()
	{
		Destructor();
	}

	Vector2u Cursor::GetPosition() const
	{
		if(!m_Created || !m_Window)
			return {};

		double x, y;

		glfwGetCursorPos(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), &x, &y);
		return Vector2u(static_cast<uint32_t>(x), static_cast<uint32_t>(y));
	}

	void Cursor::SetPosition(const Vector2u &position)
	{
		if(!m_Created || !m_Window)
			return;

		glfwSetCursorPos(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), position.X, position.Y);
	}

	const Image& Cursor::GetImage() const
	{
		return m_Image;
	}

	void Cursor::SetImage(const Image &image)
	{
		m_Image        = image;
		m_SystemCursor = false;
		Update();
	}

	bool Cursor::IsAttached() const
	{
		return m_Window != nullptr;
	}

	void Cursor::Create()
	{
		auto pixels = MakeScope<uint8_t[]>(m_Image.Width() * m_Image.Height() * 4);
		//auto pixels = new uint8_t[m_Image.Width() * m_Image.Height() * 4];

		size_t index = 0;
		for(const Color &pixel : m_Image)
		{
			pixels[index + 0] = pixel.R;
			pixels[index + 1] = pixel.G;
			pixels[index + 2] = pixel.B;
			pixels[index + 3] = pixel.A;

			index += 4;
		}

		GLFWimage image;
		image.height = static_cast<int>(m_Image.Height());
		image.width  = static_cast<int>(m_Image.Width());
		image.pixels = pixels.get();

		m_NativePointer = glfwCreateCursor(&image, m_HotSpot.X, m_HotSpot.Y);

		if(m_NativePointer)
			m_Created = true;
		else
			m_Created = false;
	}

	void Cursor::Destroy()
	{
		if ((*m_RefCout) > 1)
		{
			--(*m_RefCout);
			return;
		}

		if(!m_Created || m_SystemCursor)
			return;

		m_Created = false;
		glfwDestroyCursor(static_cast<GLFWcursor*>(m_NativePointer));
		m_NativePointer = nullptr;
	}

	void Cursor::Destructor()
	{
		Destroy();

		if ((*m_RefCout) == 0)
			delete m_RefCout;
	}

	void Cursor::Update()
	{
		if(m_Created && !m_SystemCursor)
			Destroy();

		if(!m_SystemCursor)
			Create();
	}

	void Cursor::Attach(Window *window)
	{
		m_Window = window;
		Update();
		if(m_Created)
			glfwSetCursor(
			              static_cast<GLFWwindow*>(window->GetNativeWindow()),
			              static_cast<GLFWcursor*>(m_NativePointer)
			             );
	}

	void Cursor::Invalidate()
	{
		m_Window = nullptr;
	}
}
