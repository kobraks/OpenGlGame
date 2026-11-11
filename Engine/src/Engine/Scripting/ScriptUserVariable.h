#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/UUID.h"

#include <variant>
#include <string>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Engine {
	enum class ScriptUserValueType {
		None = 0,
		Bool, Int, Float, Vec2, Vec3, String, Enum, Entity, UUID
	};

	using ScriptUserValue = std::variant<std::monostate, bool, int64_t, double, glm::vec2, glm::vec3, std::string, UUID>;

	struct ScriptUserNumericRange {
		double Min = 0.0;
		double Max = 0.0;
		double Step = 0.1;
		bool HasRange = false;
	};

	struct ScriptUserEnumInfo {
		std::string EnumName;
		std::vector<std::string> Values;
	};

	struct ScriptUserVariable {
		std::string Name;
		ScriptUserValueType Type = ScriptUserValueType::None;
		ScriptUserValue DefaultValue;
		ScriptUserNumericRange NumericRange;
		std::optional<ScriptUserEnumInfo> EnumInfo;
		std::string Category;
		std::string Tooltip;
		bool ReadOnly = false;
		std::string MetaKeyOverride;
	};
}