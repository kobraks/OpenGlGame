#pragma once

#include <glm/vec4.hpp>

#include "Assert.h"

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
					float Red;
					float Green;
					float Blue;
					float Alpha;
				};

				struct
				{
					float R;
					float G;
					float B;
					float A;
				};
			};


			float Value[4];
		};

		constexpr static size_t Size() { return 4; }


		constexpr Color() : Color(0.f, 0.f, 0.f, 0.f) {}
		constexpr Color(float red, float green, float blue, float alpha = 1.f) : Red(red),
		                                                         Green(green),
		                                                         Blue(blue),
		                                                         Alpha(alpha) {}

		constexpr Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255) : Red(red / 255.f), Green(green / 255.f), Blue(blue / 255.f), Alpha(alpha / 255.f){}
		constexpr Color(int red, int green, int blue, int alpha = 255) : Red(red / 255.f), Green(green / 255.f), Blue(blue / 255.f), Alpha(alpha / 255.f){}
		
		Color(glm::vec4 color) : Color(color.x, color.y, color.z, color.w) {}

		operator glm::vec4() const { return glm::vec4(Red, Green, Blue, Alpha); }

		float& operator[](const size_t index)
		{
			ASSERT(index < Size(), "Out of range");

			if(index >= Size())
				throw std::out_of_range("Out of range");

			return Value[index];
		}

		float operator[](const size_t index) const
		{
			ASSERT(index < Size(), "Out of range");

			if(index >= Size())
				throw std::out_of_range("Out of range");

			return Value[index];
		}
	};

	constexpr bool operator == (const Color& left, const Color& right)
	{
		return left.R == right.R && left.G == right.G && left.B == right.B && left.A == right.A;
	}
	
	constexpr bool operator != (const Color& left, const Color& right)
	{
		return !(left == right);
	}
}
