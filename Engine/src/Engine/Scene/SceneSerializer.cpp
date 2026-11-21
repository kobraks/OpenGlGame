#include "pch.h"
#include "Engine/Scene/SceneSerializer.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Components.h"

#include "Engine/Utils/Converters.h"

#include <yaml-cpp/yaml.h>
#include <sstream>
#include <fstream>
#include <stdexcept>

namespace Engine {
	namespace {
		void WriteVector(YAML::Emitter& output, const glm::vec3& value) {
			output << YAML::Flow << YAML::BeginSeq << value.x << value.y << value.z << YAML::EndSeq;
		}

		inline void WriteVector(YAML::Emitter& output, const glm::vec2& value) {
			WriteVector(output, glm::vec3(value.x, value.y, 0.0f));
		}

		glm::vec3 ReadVector(const YAML::Node& node) {
			if (!node.IsSequence() || node.size() != 3) throw std::runtime_error("Invalid vector format");
			return { node[0].as<float>(), node[1].as<float>(), node[2].as<float>() };
		}

		// UUID ReadUUID(const YAML::Node& node) {
		// 	UUID result = UUID::Invalid();
		// 	std::istringstream input(node.as<std::string>());
		// 	input >> result;
		// 	if (input.fail()) throw std::runtime_error("Invalid UUID format");
		// 	return result;
		// }

		void WriteOverride(YAML::Emitter& output, const ScriptUserValue& value) {
			output << YAML::BeginMap;

			std::visit([&](const auto& data) {
				using ValueType = std::decay_t<decltype(data)>;
				if constexpr (std::is_same_v<ValueType, std::monostate>) {
					output << YAML::Key << "Type" << YAML::Value << "None";
				} else if constexpr (std::is_same_v<ValueType, bool>) {
					output << YAML::Key << "Type" << YAML::Value << "Bool";
					output << YAML::Key << "Value" << YAML::Value << data;
				} else if constexpr (std::is_same_v<ValueType, int64_t>) {
					output << YAML::Key << "Type" << YAML::Value << "Int";
					output << YAML::Key << "Value" << YAML::Value << data;
				} else if constexpr (std::is_same_v<ValueType, double>) {
					output << YAML::Key << "Type" << YAML::Value << "Float";
					output << YAML::Key << "Value" << YAML::Value << data;
				} else if constexpr (std::is_same_v<ValueType, glm::vec2>) {
					output << YAML::Key << "Type" << YAML::Value << "Vec2";
					output << YAML::Key << "Value" << YAML::Value;
					WriteVector(output, data);
				} else if constexpr (std::is_same_v<ValueType, glm::vec3>) {
					output << YAML::Key << "Type" << YAML::Value << "Vec3";
					output << YAML::Key << "Value" << YAML::Value;
					WriteVector(output, data);
				} else if constexpr (std::is_same_v<ValueType, std::string>) {
					output << YAML::Key << "Type" << YAML::Value << "String";
					output << YAML::Key << "Value" << YAML::Value << data;
				} else if constexpr (std::is_same_v<ValueType, UUID>) {
					output << YAML::Key << "Type" << YAML::Value << "UUID";
					output << YAML::Key << "Value" << YAML::Value << fmt::format("{}", data);;
				}

				}, value);

			output << YAML::EndMap;

		}

		ScriptUserValue ReadOverride(const YAML::Node& node) {
			const std::string type = node["Type"].as<std::string>();
			const auto data = node["Value"];

			if (type == "None") {
				return std::monostate{};
			}
			if (type == "Bool") {
				return data.as<bool>();
			}
			if (type == "Int") {
				return data.as<int64_t>();
			}
			if (type == "Float") {
				return data.as<double>();
			}
			if (type == "Vec2") {
				glm::vec3 vec3 = ReadVector(data);
				return glm::vec2(vec3.x, vec3.y);
			}
			if (type == "Vec3") {
				return ReadVector(data);
			}
			if (type == "String") {
				return data.as<std::string>();
			}
			if (type == "UUID") {
				return data.as<UUID>();
			}
			throw std::runtime_error("Unsupported serialized field type: " + type);
		}
	}

	SceneSerializer::SceneSerializer(const Ref<Scene> &scene) : m_Scene(scene) {
		
	}

	void SceneSerializer::Serialize(const std::filesystem::path &path) {
		YAML::Emitter output;
		output << YAML::BeginMap;
		output << YAML::Key << "Scene" << YAML::Value << std::string(m_Scene->Title());
		output << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		auto view = m_Scene->m_Registry.view<IDComponent>();
		for (auto handle : view) {
			Entity target(handle, m_Scene.get());

			output << YAML::BeginMap;
			output << YAML::Key << "Entity" << YAML::Value << fmt::format("{}", target.GetUUID());
			output << YAML::Key << "Tag" << YAML::Value << target.GetName();
			if (target.HasComponent<FlagsComponent>()) {
				output << YAML::Key << "Flags" << YAML::Value << target.GetComponent<FlagsComponent>().Flags;
			}
			if (target.HasComponent<TransformComponent>()) {
				const auto& transform = target.GetComponent<TransformComponent>();
				output << YAML::Key << "Transform" << YAML::Value << YAML::BeginMap;
				output << YAML::Key << "Translation" << YAML::Value; WriteVector(output, transform.Translation);
				output << YAML::Key << "Rotation" << YAML::Value; WriteVector(output, transform.Rotation);
				output << YAML::Key << "Scale" << YAML::Value; WriteVector(output, transform.Scale);
				output << YAML::EndMap;
			}
			if (target.HasComponent<CameraComponent>()) {
				const auto& component = target.GetComponent<CameraComponent>();
				const auto& camera = component.Camera;

				output << YAML::Key << "Camera" << YAML::Value << YAML::BeginMap;
				output << YAML::Key << "Primary" << YAML::Value << component.Primary;
				output << YAML::Key << "FixedAspectRatio" << YAML::Value << component.FixedAspectRatio;
				output << YAML::Key << "ProjectionType" << YAML::Value << static_cast<int>(camera.GetProjectionType());
				output << YAML::Key << "PerspectiveFovRadians" << YAML::Value << camera.GetPerspectiveVerticalFOV();
				output << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
				output << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
				output << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
				output << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
				output << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
 				output << YAML::EndMap;
			}
			if (target.HasComponent<ScriptComponent>()) {
				const auto& component = target.GetComponent<ScriptComponent>();

				output << YAML::Key << "Script" << YAML::Value << YAML::BeginMap;
				output << YAML::Key << "ScriptID" << YAML::Value << fmt::format("{}", component.ScriptID);
				output << YAML::Key << "SourcePath" << YAML::Value << component.SourcePath.string();
				output << YAML::Key << "Enabled" << YAML::Value << component.Enabled;
				output << YAML::Key << "FieldValues" << YAML::Value << YAML::BeginMap;
				for (const auto& [name, value] : component.FieldValues) {
					output << YAML::Key << name << YAML::Value;
					WriteOverride(output, value);
				}
				output << YAML::EndMap << YAML::EndMap;
			}
		}
		
		output << YAML::EndSeq << YAML::EndMap;
		
		std::ofstream file(path);
		if (!file) { 			
			LOG_ENGINE_ERROR("Cannot save scene: {}", path.string());
			return;
		}
		file << output.c_str();
	}

	bool SceneSerializer::Deserialize(const std::filesystem::path &path) {
		try {
			if (!m_Scene->m_EntityMap.empty()) {
				LOG_ENGINE_ERROR("Deserialize expects an empty scene");
				return false;
			}

			YAML::Node root = YAML::LoadFile(path.string());
			if (!root["Scene"] || !root["Entities"]) return false;

			m_Scene->SetTitle(root["Scene"].as<std::string>());
			for (const auto& node : root["Entities"]) {
				const UUID entityID = node["Entity"].as<UUID>();
				Entity target = m_Scene->CreateEntity(entityID, node["Tag"].as<std::string>());
				if (node["Flags"]) {
					target.AddComponent<FlagsComponent>(node["Flags"].as<uint32_t>());
				}
				if (auto data = node["Transform"]) {
					auto& transform = target.AddComponent<TransformComponent>();

					transform.Translation = ReadVector(data["Translation"]);
					transform.Rotation = ReadVector(data["Rotation"]);
					transform.Scale = ReadVector(data["Scale"]);
				}
				if (auto data = node["Camera"]) {
					auto& component = target.AddComponent<CameraComponent>();

					component.Primary = data["Primary"].as<bool>();
					component.FixedAspectRatio = data["FixedAspectRatio"].as<bool>();
					if (data["ProjectionType"]) {
						component.Camera.SetPerspective(data["PerspectiveFovRadians"].as<float>(), data["PerspectiveNear"].as<float>(), data["PerspectiveFar"].as<float>());
					} else {
						component.Camera.SetOrthographic(data["OrthographicSize"].as<float>(), data["OrthographicNear"].as<float>(), data["OrthographicFar"].as<float>());
					}
				}
				if (auto data = node["Script"]) {
					auto component = target.AddComponent<ScriptComponent>();

					component.ScriptID = data["ScriptID"].as<UUID>();
					component.SourcePath = data["SourcePath"].as<std::string>();
					component.Enabled = data["Enabled"].as<bool>();
					if (auto fields = data["FieldValues"]) {
						for (const auto& field : fields) {
							component.FieldValues.emplace(field.first.as<std::string>(), ReadOverride(field.second));
						}
					}
				}
			}

			return true;
		} catch (const std::exception& error) {
			LOG_ENGINE_ERROR("Cannot load scene: '{}': {}", path.string(), error.what());
			return false;
		}
	}



}