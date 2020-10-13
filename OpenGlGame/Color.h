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
			struct
			{
				float Red;
				float Green;
				float Blue;
				float Alpha;
			};

			float Value[4];
		};

		Color() : Color(0.f, 0.f, 0.f, 0.f) {}

		Color(float red, float green, float blue, float alpha) : Red(red),
		                                                         Green(green),
		                                                         Blue(blue),
		                                                         Alpha(alpha) {}

		Color(glm::vec4 color) : Color(color.x, color.y, color.z, color.w) {}

		operator glm::vec4() const { return glm::vec4(Red, Green, Blue, Alpha); }

		float& operator[](const size_t index)
		{
			ASSERT(index < 4, "Out of range");

			if(index < 4)
				throw std::out_of_range("Out of range");

			return Value[index];
		}

		float operator[](const size_t index) const
		{
			ASSERT(index < 4, "Out of range");

			if(index < 4)
				throw std::out_of_range("Out of range");

			return Value[index];
		}
	};
}
