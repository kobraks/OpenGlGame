#include "pch.h"
#include "Engine/Devices/Cursor.h"
#include "Engine/Core/Window.h"

#include <GLFW/glfw3.h>
#include <glm/vec4.hpp>

namespace Engine {
	static constexpr int Translate(Cursor::CursorType cursor) {
		switch(cursor) {
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
			case Cursor::VerticalResize:
				return GLFW_VRESIZE_CURSOR;
			case Cursor::IBeam:
				return GLFW_IBEAM_CURSOR;
			default:
				return GLFW_ARROW_CURSOR;
		}
	}

	Vector2u Cursor::GetPosition() const {
		ENGINE_ASSERT(!IsAttached());
		if(!IsAttached())
			throw std::runtime_error("Trying to access unattached cursor");

		double x, y;

		glfwGetCursorPos(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), &x, &y);
		return Vector2u(static_cast<uint32_t>(x), static_cast<uint32_t>(y));
	}

	void Cursor::SetPosition(const Vector2u &pos) {
		ENGINE_ASSERT(!IsAttached());
		if(!IsAttached())
			throw std::runtime_error("Trying to access unattached cursor");

		glfwSetCursorPos(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), pos.X, pos.Y);
	}

	bool Cursor::IsAttached() const {
		return m_Window;
	}

	Cursor::Cursor(CursorType type) {
		Create(type);
	}

	Cursor::Cursor(Ref<Image> image, const Vector2i &hotspot) {
		auto pixels = MakeScope<uint8_t[]>(image->Width() * image->Height() * 4);

		size_t index = 0;
		for(const auto &pixel : image->GetPixels()) {
			pixels[index + 0] = pixel.R;
			pixels[index + 1] = pixel.G;
			pixels[index + 2] = pixel.B;
			pixels[index + 3] = pixel.A;

			index += 4;
		}

		GLFWimage gImage{static_cast<int>(image->Width()), static_cast<int>(image->Height()), pixels.get()};
		Create(&gImage, hotspot);
	}

	void Cursor::Attach(Window *window) {
		m_Window = window;
	}

	void Cursor::Invalidate() {
		m_Window = nullptr;
	}

	void Cursor::Create(CursorType type) {
		m_NativePointer = MakeScope<NativePtr>(type);

		ENGINE_ASSERT(m_NativePointer);
	}
	void Cursor::Create(void *img, const Vector2i &hotspot) {
		m_NativePointer = MakeScope<NativePtr>(img, hotspot);

		ENGINE_ASSERT(m_NativePointer);
	}

	Cursor::NativePtr::NativePtr(CursorType type) {
		Ptr = glfwCreateStandardCursor(Translate(type));
	}

	Cursor::NativePtr::NativePtr(const void *img, const Vector2i &hot) {
		Ptr = glfwCreateCursor(static_cast<const GLFWimage *>(img), hot.X, hot.Y);
	}

	Cursor::NativePtr::~NativePtr() {
		glfwDestroyCursor(static_cast<GLFWcursor *>(Ptr));
	}

	CustomCursor::CustomCursor(Ref<Image> image, const Vector2i &hotspot) : Cursor(image, hotspot), m_Image(image), m_Hotspot(hotspot) {}

	SystemCursor::SystemCursor(CursorType type) : Cursor(type),
	                                              m_Type(type) {}
}
