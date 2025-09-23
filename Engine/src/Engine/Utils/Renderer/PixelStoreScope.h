#pragma once
#include <cstdint>
#include <glad/glad.h>

namespace Engine::Utils {
	template <uint32_t Param>
	class PixelStoreScope {
	public:
		explicit PixelStoreScope(int newValue) noexcept;
		~PixelStoreScope() noexcept;

		PixelStoreScope(const PixelStoreScope&) = delete;
		PixelStoreScope& operator=(const PixelStoreScope&) = delete;
		PixelStoreScope(PixelStoreScope&&) = delete;
		PixelStoreScope& operator=(PixelStoreScope&&) = delete;

	private:
		int32_t m_Prev;
	};

	template <uint32_t Param>
	PixelStoreScope<Param>::PixelStoreScope(int newValue) noexcept {
		glGetIntegerv(Param, &m_Prev);
		glPixelStorei(Param, newValue);
	}

	template <uint32_t Param>
	PixelStoreScope<Param>::~PixelStoreScope() noexcept {
		glPixelStorei(Param, m_Prev);
	}

	using PackAlignmentScope = PixelStoreScope<GL_PACK_ALIGNMENT>;
	using UnpackAlignmentScope = PixelStoreScope<GL_UNPACK_ALIGNMENT>;
	using PackRowLengthScope = PixelStoreScope<GL_PACK_ROW_LENGTH>;
	using UnpackRowLengthScope = PixelStoreScope<GL_UNPACK_ROW_LENGTH>;
}
