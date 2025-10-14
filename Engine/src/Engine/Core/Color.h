#pragma once
#include "Engine/Core/Assert.h"
#include "Engine/Core/Base.h"

#include <fmt/format.h>
#include <glm/vec4.hpp>
#include <glm/common.hpp>
#include <glm/exponential.hpp>

#include <algorithm>
#include <cstdint>

#include "KeyCode.h"

namespace Engine {
	class Color {
	public:
		using ValueType = uint8_t;

		enum class Channel { Red, Green, Blue, Alpha };

#pragma warning(push)
#pragma warning(disable : 4201)

		union {
			uint32_t Code;

			struct {
				uint8_t R;
				uint8_t G;
				uint8_t B;
				uint8_t A;
			};
		};
#pragma warning(pop)

		[[nodiscard]] consteval static size_t Size() noexcept { return 4; }

		[[nodiscard]] constexpr static Color FromRGBA(uint32_t rgba) noexcept {
			const uint8_t r = (rgba >> 24) & 0xFF;
			const uint8_t g = (rgba >> 16) & 0xFF;
			const uint8_t b = (rgba >> 8) & 0xFF;
			const uint8_t a = (rgba >> 0) & 0xFF;

			return {r, g, b, a};
		}

		constexpr Color() : Code(0) { A = 0xff; }

		constexpr Color(float red, float green, float blue, float alpha = 1.f) noexcept : R(Translate(red)),
			G(Translate(green)),
			B(Translate(blue)),
			A(Translate(alpha)) {
		}

		constexpr Color(int red, int green, int blue, int alpha = 255) noexcept : R(Translate(red)),
			G(Translate(green)),
			B(Translate(blue)),
			A(Translate(alpha)) {
		}

		explicit constexpr Color(glm::vec4 color) noexcept : Color(color.r, color.g, color.b, color.a) {
		}

		[[nodiscard]] constexpr operator glm::vec4() const noexcept { return TranslateToFloat(Code); }

		[[nodiscard]] constexpr Color& operator=(const glm::vec4& vec) noexcept {
			return (*this) = Color(vec);
		}

		constexpr bool operator==(const Color& other) const noexcept {
			return Code == other.Code;
		}

		constexpr bool operator!=(const Color& other) const noexcept {
			return !(*this==other);
		}

		constexpr std::strong_ordering operator<=>(const Color& other) const noexcept {
			return Code <=> other.Code;
		}

		[[nodiscard]] constexpr uint8_t& operator[](const std::ptrdiff_t i) noexcept {
			ENGINE_ASSERT(i >= 0 && i < static_cast<std::ptrdiff_t>(Size()));

			return (&R)[i];
		}

		[[nodiscard]] constexpr const uint8_t& operator[](std::ptrdiff_t i) const noexcept {
			ENGINE_ASSERT(i >= 0 && i < static_cast<std::ptrdiff_t>(Size()));

			return (&R)[i];
		}

		[[nodiscard]] constexpr uint8_t& operator[](Channel channel) noexcept {
			const auto i = static_cast<std::size_t>(channel);
			ENGINE_ASSERT(i < Size());

			return (&R)[i];
		}

		[[nodiscard]] constexpr const uint8_t& operator[](Channel channel) const noexcept {
			const auto i = static_cast<std::size_t>(channel);
			ENGINE_ASSERT(i < Size());

			return (&R)[i];
		}

		[[nodiscard]] uint8_t GetChannel(Channel channel) const noexcept {
			return operator[](channel);
		}

		void SetChannel(Channel channel, uint8_t value) noexcept {
			switch (channel) {
			case Channel::Red:
				Code = (Code & ~RedFlag) | (value << RedBit);
				break;
			case Channel::Green:
				Code = (Code & ~GreenFlag) | (value << GreenBit);
				break;
			case Channel::Blue:
				Code = (Code & ~BlueFlag) | (value << BlueBit);
				break;
			case Channel::Alpha:
				Code = (Code & ~AlphaFlag) | (value << AlphaBit);
				break;
			}
		}

		[[nodiscard]] std::string ToString() const {
			return fmt::format("{:#010X}", ToRGBA());
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
		constexpr static uint8_t Translate(float color) noexcept {
			return static_cast<uint8_t>(std::clamp(color, 0.f, 1.f) * 255);
		}

		constexpr static uint8_t Translate(int32_t color) noexcept {
			return static_cast<uint8_t>(std::clamp(color, 0, 255));
		}

		constexpr static auto ExtractChannel(uint32_t color, uint32_t flag, uint32_t bits) noexcept {
			return (color & flag) >> bits;
		}

		constexpr static glm::vec4 TranslateToFloat(uint32_t color) noexcept {
			constexpr auto GetR = [](uint32_t color) {
				return static_cast<float>(ExtractChannel(color, RedFlag, RedBit)) / 255.f;
			};
			constexpr auto GetG = [](uint32_t color) {
				return static_cast<float>(ExtractChannel(color, GreenFlag, GreenBit)) / 255.f;
			};
			constexpr auto GetB = [](uint32_t color) {
				return static_cast<float>(ExtractChannel(color, BlueFlag, BlueBit)) / 255.f;
			};
			constexpr auto GetA = [](uint32_t color) {
				return static_cast<float>(ExtractChannel(color, AlphaFlag, AlphaBit)) / 255.f;
			};

			return {GetR(color), GetG(color), GetB(color), GetA(color)};
		}

	public:
		[[nodiscard]] constexpr auto ToFloat() const noexcept {
			return TranslateToFloat(Code);
		}

		[[nodiscard]] static constexpr Color Lerp(const Color& a, const Color& b, float t) noexcept {
			const glm::vec4 af = a.ToFloat();
			const glm::vec4 bf = b.ToFloat();

			const glm::vec4 result = glm::mix(af, bf, t);
			return Color(result);
		}

		[[nodiscard]] constexpr Color GrayScale() const noexcept {
			const float gray = 0.299f * static_cast<float>(R) / 255.0f + 0.587f * static_cast<float>(G) / 255.0f +
				0.114f * static_cast<float>(B) / 255.0f;
			return {gray, gray, gray, static_cast<float>(A) / 255.0f};
		}

		[[nodiscard]] glm::vec4 ToLinear() const noexcept {
			const auto c = ToFloat();
			return glm::vec4{glm::pow(c.r, 2.2f), glm::pow(c.g, 2.2f), glm::pow(c.b, 2.2f), c.a};
		}

		[[nodiscard]] static Color FromLinear(glm::vec4 color) noexcept {
			color = glm::vec4(glm::pow(color.r, 1.0f / 2.2f), glm::pow(color.g, 1.0f / 2.2f),
			                  glm::pow(color.b, 1.0f / 2.2f), color.a);
			return Color(color);
		}

		[[nodiscard]] constexpr uint32_t ToRGBA() const noexcept {
			return static_cast<uint32_t>(R) << RedBit |
				static_cast<uint32_t>(G) << GreenBit |
				static_cast<uint32_t>(B) << BlueBit |
				static_cast<uint32_t>(A) << AlphaBit;
		}

		[[nodiscard]] constexpr Color WithRed(uint8_t r) const noexcept { auto c = *this; c.R = r; return c; }
		[[nodiscard]] constexpr Color WithGreen(uint8_t g) const noexcept { auto c = *this; c.G = g; return c; }
		[[nodiscard]] constexpr Color WithBlue(uint8_t b) const noexcept { auto c = *this; c.B = b; return c; }
		[[nodiscard]] constexpr Color WithAlpha(uint8_t a) const noexcept { auto c = *this; c.A = a; return c; }

		[[nodiscard]] constexpr Color Premultiplied() const noexcept {
			const auto f = ToFloat();
			return {f.r * f.a, f.g * f.a, f.b * f.a, f.a};
		}

		[[nodiscard]] static constexpr Color Unpremultiplied(const glm::vec4& pma) noexcept {
			if (pma.a == 0.f) {
				return Color{0, 0, 0, 0};
			}
			return { pma.r / pma.a, pma.g / pma.a, pma.b / pma.a, pma.a };
		}

		[[nodiscard]] static Color BlendOver(const Color& sourceColor, const Color& destinationColor) noexcept {
			const auto src = sourceColor.ToFloat();
			const auto dsc = destinationColor.ToFloat();
			const float outA = src.a + dsc.a * (1.f - src.a);
			glm::vec4 out { (src.r * src.a + dsc.r * dsc.a * (1.f - src.a)) / (outA > 0.f ? outA : 1.f),
							(src.g * src.a + dsc.g * dsc.a * (1.f - src.a)) / (outA > 0.f ? outA : 1.f),
							(src.b * src.a + dsc.b * dsc.a * (1.f - src.a)) / (outA > 0.f ? outA : 1.f),
							outA };

			return Color(out);
		}
	};

	static_assert(sizeof(Color) == sizeof(uint32_t), "Color must be 4 bytes");
	static_assert(std::is_trivially_copyable_v<Color>);

}

namespace std {
	template <>
	struct tuple_size<Engine::Color> : std::integral_constant<std::size_t, 4> {
	};

	template <std::size_t N>
	struct tuple_element<N, Engine::Color> {
		using type = uint8_t;
	};

	//Hash support
	template<>
	struct hash<Engine::Color> {
		std::size_t operator()(const Engine::Color& color) const noexcept {
			return std::hash<uint32_t>{}(color.ToRGBA());
		}
	};
}

namespace Engine {
	template <std::size_t N>
	constexpr uint8_t& get(Color& color) noexcept {
		static_assert(N < 4, "Color index out of range");

		if constexpr (N == 0) return color.R;
		else if constexpr (N == 1) return color.G;
		else if constexpr (N == 2) return color.B;
		else return color.A;
	}

	template <std::size_t N>
	constexpr const uint8_t& get(const Color& color) noexcept {
		static_assert(N < 4, "Color index out of range");

		if constexpr (N == 0) return color.R;
		else if constexpr (N == 1) return color.G;
		else if constexpr (N == 2) return color.B;
		else return color.A;
	}
}

template <>
struct fmt::formatter<Engine::Color> : fmt::formatter<std::string_view> {
	enum class Style : uint8_t { Hex, Int, Float, Linear, Vec, Css, CssA, SingleChannel };

	Style Mode = Style::Hex;
	char Channel = '\0';
	bool Alt = false;
	bool LowerCase = false;
	int Precision = 3;

	// parse: [#][type][.precision]
	template <typename ParseContext>
	constexpr auto parse(ParseContext& ctx) {
		auto it = ctx.begin();
		const auto end = ctx.end();

		if (it != end && *it == '#') { Alt = true; ++it; }

		if (it != end && *it != '}' && *it != '.') {
			const char t = *it++;
			switch (t) {
			case 'x': Mode = Style::Hex; LowerCase = true; break; // hex (default)
			case 'X': Mode = Style::Hex; LowerCase = false; break; // hex (default)
			case 'i': Mode = Style::Int; break; // integer 0-255
			case 'f': Mode = Style::Float; break; // float 0.0-1.0
			case 'l': Mode = Style::Linear; break; // normalized linear float 0.0-1.0
			case 'v': Mode = Style::Vec; break; // vec4(r, g, b, a)
			case 's': Mode = Style::Css; break; // rgb(r, g, b)
			case 'S': Mode = Style::CssA; break; // rgba(r, g, b, a)
			case 'r': case 'g': case 'b': case 'a':
			case 'R': case 'G': case 'B': case 'A':
				Mode = Style::SingleChannel; Channel = t; break;
			default: break;
			}
		}

		if (it != end && *it == '.') {
			++it;
			if (it == end || *it == '}')
				throw fmt::format_error("Missing precision value after '.'");

			auto [pos, ec] = std::from_chars(it, end, Precision);
			it = pos;
		}

		return it;
	}

	template <typename FormatContext>
	constexpr auto format(const Engine::Color& color, FormatContext& ctx) const {
		fmt::memory_buffer buf;
		buf.reserve(64);
		switch (Mode) {
		case Style::Hex: {
			if (Alt) {
				if (LowerCase)
					fmt::format_to(std::back_inserter(buf), "{:#010x}", color.ToRGBA());
				else
					fmt::format_to(std::back_inserter(buf), "{:#010X}", color.ToRGBA());
			} else {
				if (LowerCase)
					fmt::format_to(std::back_inserter(buf), "{:08x}", color.ToRGBA());
				else
					fmt::format_to(std::back_inserter(buf), "{:08X}", color.ToRGBA());
			}

			break;
		}
		case Style::Int: {
			if (Alt) {
				fmt::format_to(std::back_inserter(buf), "R={} G={} B={} A={}", color.R, color.G, color.B, color.A);
			} else {
				fmt::format_to(std::back_inserter(buf), "{}, {}, {}, {}", color.R, color.G, color.B, color.A);
			}
			break;
		}
		case Style::Float: {
			const auto v = color.ToFloat();
			if (Alt) {
				fmt::format_to(std::back_inserter(buf), "R={:.{}f} G={:.{}f} B={:.{}f} A={:.{}f}", v.r, Precision, v.g, Precision, v.b, Precision, v.a, Precision);
			} else {
				fmt::format_to(std::back_inserter(buf), "{:.{}f}, {:.{}f}, {:.{}f}, {:.{}f}", v.r, Precision, v.g, Precision, v.b, Precision, v.a, Precision);
			}
			break;
		}
		case Style::Linear: {
			const auto v = color.ToLinear();
			if (Alt) {
				fmt::format_to(std::back_inserter(buf), "R={:.{}f} G={:.{}f} B={:.{}f} A={:.{}f}", v.r, Precision, v.g, Precision, v.b, Precision, v.a, Precision);
			} else {
				fmt::format_to(std::back_inserter(buf), "{:.{}f}, {:.{}f}, {:.{}f}, {:.{}f}", v.r, Precision, v.g, Precision, v.b, Precision, v.a, Precision);
			}
			break;
		}
		case Style::Vec: {
			const auto v = color.ToFloat();
			fmt::format_to(std::back_inserter(buf), "vec4({:.{}f}, {:.{}f}, {:.{}f}, {:.{}f})", v.r, Precision, v.g, Precision, v.b, Precision, v.a, Precision);
			break;
		}
		case Style::Css: {
			fmt::format_to(std::back_inserter(buf), "rgb({}, {}, {})", color.R, color.G, color.B);
			break;
		}
		case Style::CssA: {
			const auto v = color.ToFloat();
			fmt::format_to(std::back_inserter(buf), "rgba({}, {}, {}, {:.{}f})", color.R, color.G, color.B, std::clamp(v.a, 0.f, 1.f), Precision);
			break;
		}
		case Style::SingleChannel: {
			const auto v = color.ToFloat();

			switch (Channel) {
			case 'r': fmt::format_to(std::back_inserter(buf), "{}", color.R); break;
			case 'g': fmt::format_to(std::back_inserter(buf), "{}", color.G); break;
			case 'b': fmt::format_to(std::back_inserter(buf), "{}", color.B); break;
			case 'a': fmt::format_to(std::back_inserter(buf), "{}", color.A); break;
			case 'R': fmt::format_to(std::back_inserter(buf), "{:.{}f}", v.r, Precision); break;
			case 'G': fmt::format_to(std::back_inserter(buf), "{:.{}f}", v.g, Precision); break;
			case 'B': fmt::format_to(std::back_inserter(buf), "{:.{}f}", v.b, Precision); break;
			case 'A': fmt::format_to(std::back_inserter(buf), "{:.{}f}", v.a, Precision); break;
			default:
				fmt::format_to(std::back_inserter(buf), "{:#010X}", color.ToRGBA()); break;
			}
		}
		default:
			fmt::format_to(std::back_inserter(buf), "{:#010X}", color.ToRGBA());
			break;
		}

		return fmt::formatter<std::string_view>::format({ buf.data(), buf.size() }, ctx);
	}
};
