#pragma once
#include <string_view>
#include "Context.h"
#include "Exception.h"

#define CHECK_IF_VALID_CONTEXT if (!Context::GetContext() || !Context::GetContext()->IsContextCurrent()) {ASSERT(false, "Not in valid context");}


namespace Game::Priv
{
	void GLCheckError(const std::string_view &expr, const std::string_view &fileName, size_t line);
}

#ifdef _DEBUG
#define GL_CHECK(expr) CHECK_IF_VALID_CONTEXT; \
	do {\
	expr; \
	Game::Priv::GLCheckError(#expr, __FILE__, __LINE__);\
	}while(0)
#else
#define GL_CHECK(expr) expr;
#endif
