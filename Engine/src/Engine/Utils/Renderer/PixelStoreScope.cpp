#include "pch.h"

#include "PixelStoreScope.h"
#include <glad/glad.h>

namespace Engine::Utils {
	PackAlignmentScope::PackAlignmentScope(int newAlignment) noexcept {
		glGetIntegerv(GL_PACK_ALIGNMENT, &m_Prev);
		glPixelStorei(GL_PACK_ALIGNMENT, newAlignment);
		
	}

	PackAlignmentScope::~PackAlignmentScope() noexcept {
		glPixelStorei(GL_PACK_ALIGNMENT, m_Prev);
	}

	UnpackAlignmentScope::UnpackAlignmentScope(int newAlignment) noexcept {
		glGetIntegerv(GL_UNPACK_ALIGNMENT, &m_Prev);
		glPixelStorei(GL_UNPACK_ALIGNMENT, newAlignment);
	}


	UnpackAlignmentScope::~UnpackAlignmentScope() noexcept {
		glPixelStorei(GL_UNPACK_ALIGNMENT, m_Prev);
	}

}