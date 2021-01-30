#include "pch.h"
#include "Cursor.h"

#include <GLFW/glfw3.h>
#include <glm/vec4.hpp>

#include "Window.h"

namespace Game
{
	static constexpr int Translate(Cursor::CursorType cursor)
	{
		switch(cursor)
		{
			case Cursor::Arrow: return GLFW_ARROW_CURSOR;
			case Cursor::Center: return GLFW_CENTER_CURSOR;
			case Cursor::Crosshair: return GLFW_CROSSHAIR_CURSOR;
			case Cursor::Hand: return GLFW_HAND_CURSOR;
			case Cursor::HorizontalResize: return GLFW_HRESIZE_CURSOR;
			case Cursor::VerdicalResize: return GLFW_VRESIZE_CURSOR;
			case Cursor::IBeam: return GLFW_IBEAM_CURSOR;
			default: return GLFW_ARROW_CURSOR;
		}
	}
	
	Cursor::Cursor() : Cursor(Cursor::Arrow)
	{
	}

	Cursor::Cursor(CursorType cursor)
	{
		m_NativePointer = glfwCreateStandardCursor(Translate(cursor));

		if (m_NativePointer)
			m_Created = true;
	}

	Cursor::Cursor(const Image &image, Vector2i hotSpot) : m_Image(image), m_HotSpot(hotSpot) 
	{
		Create();
	}

	Cursor::~Cursor()
	{
		Destroy();
	}

	Vector2u Cursor::GetPosition() const
	{
		if(!m_Created || !m_Window)
			return Vector2u();

		double x, y;

		glfwGetCursorPos(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), &x, &y);
		return Vector2u(static_cast<uint32_t>(x), static_cast<uint32_t>(y));
	}

	void Cursor::SetPosition(const Vector2u &position)
	{
		if(!m_Created || !m_Window)
			return;

		glfwSetCursorPos(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), static_cast<double>(position.X), static_cast<double>(position.Y));
	}

	const Image& Cursor::GetImage() const
	{
		return m_Image;
	}

	void Cursor::SetImage(const Image &image)
	{
		m_Image = image;
		m_SystemCursor = false;
		Update();
	}

	bool Cursor::IsAttached() const
	{
		return m_Window != nullptr;
	}

	void Cursor::Create()
	{
		uint8_t *pixels = new uint8_t[m_Image.Width() * m_Image.Height() * 4];

		size_t index = 0;
		for(Color &pixel : m_Image)
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
		image.pixels = pixels;

		m_NativePointer = glfwCreateCursor(&image, m_HotSpot.X, m_HotSpot.Y);

		if(m_NativePointer)
			m_Created = true;
		else
			m_Created = false;

		delete [] pixels;
	}

	void Cursor::Destroy()
	{
		if (!m_Created || m_SystemCursor)
			return;
		
		m_Created = false;
		glfwDestroyCursor(static_cast<GLFWcursor*>(m_NativePointer));
		m_NativePointer = nullptr;
	}

	void Cursor::Update()
	{
		if (m_Created && !m_SystemCursor)
			Destroy();

		if (!m_SystemCursor)
			Create();
	}

	void Cursor::Attach(Window *window)
	{
		m_Window = window;
		Update();
		if(m_Created)
			glfwSetCursor(static_cast<GLFWwindow*>(window->GetNativeWindow()), static_cast<GLFWcursor*>(m_NativePointer));
	}

	void Cursor::Invalidate()
	{
		m_Window = nullptr;
	}
}
