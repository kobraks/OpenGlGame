#pragma once
#include <string_view>
#include "Context.h"

#define CHECK_IF_VALID_CONTEXT if (!Context::GetContext() || !Context::GetContext()->IsContextCurrent()) return

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
