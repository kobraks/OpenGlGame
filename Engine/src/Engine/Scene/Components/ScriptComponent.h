#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/UUID.h"
#include "Engine/Scripting/ScriptUserVariable.h"

#include <filesystem>
#include <string>
#include <unordered_map>

namespace Engine {
	struct ScriptComponent {
		UUID ScriptID = UUID::Invalid();
		std::filesystem::path SourcePath;

		bool Enabled = true;
		std::unordered_map<std::string, ScriptUserValue> FieldValues;
	};
}