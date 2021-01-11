#pragma once
#include <string_view>

namespace Game::Priv
{
	void GLCheckError(const std::string_view &expr, const std::string_view &fileName, size_t line);
}

#ifdef _DEBUG
#define GL_CHECK(expr) do {\
	expr; \
	Game::Priv::GLCheckError(#expr, __FILE__, __LINE__);\
	}while(0)
#else
#define GL_CHECK(expr) expr;
#endif
