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
			uint32_t Code;

			struct {
				//TODO Fix warning here
				uint8_t A;
				uint8_t B;
				uint8_t G;
				uint8_t R;
			};
		};

		consteval static size_t Size() { return 4; }

		constexpr Color() : Color(0xff) {}

		constexpr Color(float red, float green, float blue, float alpha = 1.f) : A(Translate(alpha)),
			B(Translate(blue)),
			G(Translate(green)),
			R(Translate(red)) {}

		constexpr Color(int red, int green, int blue, int alpha = 255) : A(Translate(alpha)),
		                                                                 B(Translate(blue)),
		                                                                 G(Translate(green)),
		                                                                 R(Translate(red)) {}

		constexpr Color(uint32_t color) : Code(color) {}

		explicit constexpr Color(glm::vec4 color) : Color(color.r, color.g, color.b, color.a) {}

		constexpr operator glm::vec4() const { return TranslateToFloat(Code); }

		constexpr Color& operator=(const glm::vec4 &vec) {
			return (*this) = Color(vec);
		}

		constexpr uint8_t& operator[](const std::ptrdiff_t i) {
			ENGINE_ASSERT(i >= 0 && i < static_cast<std::ptrdiff_t>(Size()));

			switch(i) {
				case 0:
					return R;
				case 1:
					return G;
				case 2:
					return B;
				case 3:
					return A;
				default:
					throw std::out_of_range("Out of range");
			}
		}

		constexpr uint8_t const& operator[](std::ptrdiff_t i) const {
			ENGINE_ASSERT(i >= 0 && i < static_cast<std::ptrdiff_t>(Size()));

			switch(i) {
				case 0:
					return R;
				case 1:
					return G;
				case 2:
					return B;
				case 3:
					return A;
				default:
					throw std::out_of_range("Out of range");
			}
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

		static constexpr uint32_t AlphaFlag = 0xff;
		static constexpr uint32_t RedFlag = 0xff000000;
		static constexpr uint32_t GreenFlag = 0xff0000;
		static constexpr uint32_t BlueFlag = 0xff00;

		static constexpr uint32_t RedBit = 24;
		static constexpr uint32_t GreenBit = 16;
		static constexpr uint32_t BlueBit = 8;
		static constexpr uint32_t AlphaBit = 0;

	private:
		constexpr static uint8_t Translate(float color) {
			return static_cast<uint8_t>(std::clamp(color, 0.f, 1.f) * 255);
		}

		constexpr static uint8_t Translate(int32_t color) {
			return static_cast<uint8_t>(std::clamp(color, 0, 255));
		}

		constexpr static auto GetPart(uint32_t color, uint32_t flag, uint32_t bits) {
			return (color & flag) >> bits;
		}

		constexpr static glm::vec4 TranslateToFloat(uint32_t color) {
			constexpr auto GetR = [](uint32_t color){ return static_cast<float>(GetPart(color, RedFlag, RedBit)) / 255.f; };
			constexpr auto GetG = [](uint32_t color){ return static_cast<float>(GetPart(color, GreenFlag, GreenBit)) / 255.f; };
			constexpr auto GetB = [](uint32_t color){ return static_cast<float>(GetPart(color, BlueFlag, BlueBit)) / 255.f; };
			constexpr auto GetA = [](uint32_t color){ return static_cast<float>(GetPart(color, AlphaFlag, AlphaBit)) / 255.f; };

			return glm::vec4(GetR(color), GetG(color), GetB(color), GetA(color));
		}

	public:
		constexpr auto ToFloat() const {
			return TranslateToFloat(Code);
		}
	};

	constexpr bool operator==(const Color &lth, const Color &rth) {
		return lth.Code == rth.Code;
	}

	constexpr bool operator!=(const Color &lth, const Color &rth) {
		return !(lth == rth);
	}
}


template <>
struct fmt::formatter<Engine::Color>: formatter<uint32_t> {
	auto format(const Engine::Color &c, format_context &ctx) const {
		return format_to(ctx.out(), "{:#010X}", c.Code);
	}

	constexpr auto parse(format_parse_context &ctx) {
		return ctx.begin();
	}
};
