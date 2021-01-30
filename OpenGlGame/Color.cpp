#include "pch.h"
#include "Color.h"

namespace Game
{
	const Color Color::Black{0, 0, 0};
	const Color Color::White{255, 255, 255};
	const Color Color::Red{255, 0, 0};
	const Color Color::Green{0, 255, 0};
	const Color Color::Blue{0, 0, 255};
	const Color Color::Yellow{255, 255, 0};
	const Color Color::Magenta{255, 0, 255};
	const Color Color::Cyan{0, 255, 255};

	uint32_t Color::ToInteger() const
	{
		return (R << 24) | (G << 16) | (B << 8) | A;
	}
}
