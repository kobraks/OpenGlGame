#pragma once
#include "Engine/Core/UUID.h"

#include <yaml-cpp/yaml.h>

namespace YAML {
	template<>
	struct convert<Engine::UUID> {
		static Node encode(const Engine::UUID& identifier) {
			return Node(identifier.ToString());
		}

		static bool decode(const Node& node, Engine::UUID& identifier) {
			if (!node.IsScalar())
				return false;

			const auto parsed = Engine::UUID::TryParse(node.Scalar());
			if (!parsed)
				return false;

			identifier = *parsed;
			return true;
		}
	};
}