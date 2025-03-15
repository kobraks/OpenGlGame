#pragma once

#include "Engine/Core/Base.h"

namespace Engine {
	class FileDialogs {
	public:
		static std::string OpenFile(std::string_view filter);
		static std::string SaveFile(const char* filter);
	};
}