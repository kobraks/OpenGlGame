#pragma once

#include "Rect.h"

namespace Game
{
	class Glyph
	{
	public:
		float Advance = 0;
		FloatRect Bounds;
		IntRect TextureRect;
	};
}