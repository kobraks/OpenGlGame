#pragma once
#include "Engine/Renderer/GraphicEnums.h"

namespace Engine::Utils {
	constexpr bool IsScalar(const UniformTypeDesc& desc) { return desc.Kind == UniformKind::Scalar; }
	constexpr bool IsVector(const UniformTypeDesc& desc) { return desc.Kind == UniformKind::Vector; }
	constexpr bool IsMatrix(const UniformTypeDesc& desc) { return desc.Kind == UniformKind::Matrix; }
	constexpr bool IsSampler(const UniformTypeDesc& desc) { return desc.Kind == UniformKind::Sampler; }
	constexpr bool IsImage(const UniformTypeDesc& desc) { return desc.Kind == UniformKind::Image; }

	constexpr uint8_t VectorLength(const UniformTypeDesc& desc) { return IsVector(desc) ? desc.Rows : 0; }
	constexpr uint8_t MatrixRows(const UniformTypeDesc& desc) { return IsMatrix(desc) ? desc.Rows : 0; }
	constexpr uint8_t MatrixCols(const UniformTypeDesc& desc) { return IsMatrix(desc) ? desc.Cols : 0; }

	constexpr uint32_t ComponentCount(const UniformTypeDesc& desc) {
		if (IsScalar(desc)) return 1;
		if (IsVector(desc)) return desc.Rows;
		if (IsMatrix(desc)) return desc.Rows * desc.Cols;
		return 0; // For samplers and images, we don't count components
	}
}