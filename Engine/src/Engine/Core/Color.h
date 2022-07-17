#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Assert.h"

#include <glm/vec4.hpp>

#include <imgui.h>

namespace Game
{
	class Color
	{
	public:
		union
		{
			union
			{
				struct
				{
					uint8_t R;
					uint8_t G;
					uint8_t B;
					uint8_t A;
				};


				uint8_t Table[4];
			};

			int32_t Value;
		};

		constexpr static size_t Size() { return 4; }

		constexpr Color() : Color(0.f, 0.f, 0.f, 0.f) {}

		constexpr Color(float red, float green, float blue, float alpha = 1.f) : R(static_cast<uint8_t>(red * 255)),
		                                                                         G(static_cast<uint8_t>(green * 255)),
		                                                                         B(static_cast<uint8_t>(blue * 255)),
		                                                                         A(static_cast<uint8_t>(alpha * 255)) {}

		constexpr Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255) : R(red),
		                                                                                 G(green),
		                                                                                 B(blue),
		                                                                                 A(alpha) {}

		constexpr Color(int red, int green, int blue, int alpha = 255) : R(static_cast<uint8_t>(red)),
		                                                                 G(static_cast<uint8_t>(green)),
		                                                                 B(static_cast<uint8_t>(blue)),
		                                                                 A(static_cast<uint8_t>(alpha)) {}

		constexpr Color(uint32_t color) : R((color & 0xff000000) >> 24),
		                                  G((color & 0x00ff0000) >> 16),
		                                  B((color & 0x0000ff00) >> 8),
		                                  A((color & 0x000000ff) >> 0) {}

		Color(glm::vec4 color) : Color(color.x, color.y, color.z, color.w) {}

		operator glm::vec4() const { return glm::vec4(R / 255.f, G / 255.f, B / 255.f, A / 255.f); }
		operator ImVec4() const { return ImVec4{R / 255.f, G / 255.f, B / 255.f, A / 255.f}; }

		uint8_t& operator[](const size_t index)
		{
			ASSERT(index < Size(), "Out of range");

			if(index >= Size())
				throw std::out_of_range("Out of range");

			return Table[index];
		}

		uint8_t operator[](const size_t index) const
		{
			ASSERT(index < Size(), "Out of range");

			if(index >= Size())
				throw std::out_of_range("Out of range");

			return Table[index];
		}

		uint32_t ToInteger() const;

		static const Color Black;
		static const Color White;
		static const Color Red;
		static const Color Green;
		static const Color Blue;
		static const Color Yellow;
		static const Color Magenta;
		static const Color Cyan;
		static const Color Transparent;
	};

	constexpr bool operator ==(const Color &left, const Color &right)
	{
		return left.R == right.R && left.G == right.G && left.B == right.B && left.A == right.A;
	}

	constexpr bool operator !=(const Color &left, const Color &right)
	{
		return !(left == right);
	}
}
