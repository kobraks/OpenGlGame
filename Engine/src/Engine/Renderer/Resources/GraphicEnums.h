#pragma once
#include "Engine/Core/Base.h"

namespace Engine {

	enum class CompareFunction : uint8_t{
		Never = 0,
		Less,
		LEqual,
		Greater,
		GEqual,
		Equal,
		NotEqual,
		Always
	};

	enum class UniformKind : uint8_t { Unknown = 0, Scalar, Vector, Matrix, Sampler, Image, AtomicCounter };
	enum class ScalarKind : uint8_t { None = 0, Bool, Int, UInt, Float, Double };
	enum class TextureDim : uint8_t { None = 0, D1, D2, D3, Cube, Rect, Buffer, D1Array, D2Array, CubeArray, D2MS, D2MSArray };

	struct UniformTypeDesc {
		UniformKind Kind = UniformKind::Unknown;
		ScalarKind Scalar = ScalarKind::None;

		uint8_t Rows = 0; // vectors: length; matrices: Rows; scalars: 1
		uint8_t Cols = 0; // matrices: Cols; vectors/scalars: 1

		TextureDim Dim = TextureDim::None; //Only for samplers and images
		bool Shadow = false; //Only for samplers
	};
}