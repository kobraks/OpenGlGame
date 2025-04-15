#pragma once
#include "Engine/Core/UUID.h"

#include <yaml-cpp/yaml.h>

namespace YAML {
	template<>
	struct convert<Engine::UUID> {
		static Node encode(const Engine::UUID& uuid) {
			Node node;
			node = static_cast<std::string>(Engine::UUID::Type(uuid).str());
			return node;
		}

		static bool decode(const Node& node, Engine::UUID& uuid) {
			if (!node.IsScalar())
				return false;

			try {
				uuid = Engine::UUID(Engine::UUID::Type(node.as<std::string>()));
				return true;
			} catch (...) {
				return false;
			}
		}
	};
}