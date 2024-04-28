#pragma once
#include "Engine/Core/Assert.h"
#include "Engine/Core/Base.h"

#include <fmt/format.h>
#include <glm/vec4.hpp>

#include <algorithm>
#include <cstdint>

namespace Engine {
	class Color {
	public:
		union {
			union {
				union {
					struct {
						uint8_t A;
						uint8_t B;
						uint8_t G;
						uint8_t R;
					};

					uint8_t Table[4];
				};
			};

			uint32_t Value;
		};

		constexpr static size_t Size() { return 4; }

		constexpr Color() : Color(0x000000ff) {}

		constexpr Color(float red, float green, float blue, float alpha = 1.f) : A(
				 static_cast<uint8_t>(std::clamp(alpha, 0.f, 1.f) * 255)
				),
			B(static_cast<uint8_t>(std::clamp(blue, 0.f, 1.f) * 255)),
			G(static_cast<uint8_t>(std::clamp(green, 0.f, 1.f) * 255)),
			R(static_cast<uint8_t>(std::clamp(red, 0.f, 1.f) * 255)) {}

		constexpr Color(int red, int green, int blue, int alpha = 255) : A(
		                                                                   static_cast<uint8_t>(std::clamp(
			                                                                    alpha,
			                                                                    0,
			                                                                    255
			                                                                   ))
		                                                                  ),
		                                                                 B(
		                                                                   static_cast<uint8_t>(
			                                                                   std::clamp(blue, 0, 255))
		                                                                  ),
		                                                                 G(
		                                                                   static_cast<uint8_t>(std::clamp(
			                                                                    green,
			                                                                    0,
			                                                                    255
			                                                                   ))
		                                                                  ),
		                                                                 R(
		                                                                   static_cast<uint8_t>(std::clamp(red, 0, 255))
		                                                                  ) {}

		constexpr Color(uint32_t color) : Value(color) {}

		explicit constexpr Color(glm::vec4 color) : Color(color.r, color.g, color.b, color.a) {}

		constexpr operator glm::vec4() const { return glm::vec4(R / 255.f, G / 255.f, B / 255.f, A / 255.f); }

		constexpr Color& operator=(const glm::vec4 &vec) {
			return (*this) = Color(vec);
		}

		constexpr uint8_t &operator[](const size_t index) {
			ENGINE_ASSERT(index < Size());

			if (index >= Size())
				throw std::out_of_range("Out of range");

			return Table[index];
		}

		constexpr uint8_t operator[](const size_t index) const {
			ENGINE_ASSERT(index < Size());

			if (index >= Size())
				throw std::out_of_range("Out of range");

			return Table[index];
		}

		static const Color Black;
		static const Color White;

		static const Color Red;
		static const Color Green;
		static const Color Blue;

		static const Color Magenta;
		static const Color Cyan;
		static const Color Yellow;

		static const Color Transparent;
	};

	constexpr bool operator==(const Color &lth, const Color &rth) {
		return lth.Value == rth.Value;
	}

	constexpr bool operator!=(const Color &lth, const Color &rth) {
		return !(lth == rth);
	}
}


template<>
struct fmt::formatter<Engine::Color>: public formatter<uint32_t> {
	auto format(const Engine::Color &c, format_context &ctx) const {
		return format_to(ctx.out(), "{:#08X}", c.Value);
	}

	constexpr auto parse(format_parse_context &ctx) {
		return ctx.begin();
	}
};