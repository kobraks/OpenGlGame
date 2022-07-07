#include "pch.h"
#include "SceneSerializer.h"

#include "Component.h"
#include "Entity.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

#define BEGIN_MAP(key) out << YAML::Key << key << YAML::BeginMap
#define END_MAP() out << YAML::EndMap

#define VALUE(key, value) out << YAML::Key << key << YAML::Value << value;

namespace YAML
{
	template <>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2 &rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node &node, glm::vec2 &rhs)
		{
			if(!node.IsSequence() || node.size() != 0)
				return false;

			rhs.x = node[0].as<float>();
			rhs.x = node[1].as<float>();
		}
	};


	template <>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3 &rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node &node, glm::vec3 &rhs)
		{
			if(!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template <>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4 &rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node &node, glm::vec4 &rhs)
		{
			if(!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}

namespace Game
{
	YAML::Emitter& operator<<(YAML::Emitter &out, const glm::vec2 &v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;

		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter &out, const glm::vec3 &v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter &out, const glm::vec4 &v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter &out, const boost::multiprecision::uint128_t &v)
	{
		return out << v;
	}

	static void SerializeEntity(YAML::Emitter &out, Entity entity)
	{
		ASSERT(entity.HasComponent<IDComponent>());

		out << YAML::BeginMap;
		out << YAML::Key << "ID" << YAML::Value << entity.GetUUID();

		if(entity.HasComponent<TagComponent>())
		{
			BEGIN_MAP("TagComponent"); //TagComponent

			const auto &tag = entity.GetComponent<TagComponent>().Tag;
			VALUE("Tag", tag);

			END_MAP(); //TagComponent
		}

		if(entity.HasComponent<TransformComponent>())
		{
			BEGIN_MAP("TransformComponent"); //TransfromComponent

			const auto &tc = entity.GetComponent<TransformComponent>();

			VALUE("Translation", tc.GetPosition());
			VALUE("Scale", tc.GetScale());
			VALUE("Rotation", tc.GetRotation());

			END_MAP(); //TransformComponent
		}

		if(entity.HasComponent<CameraComponent>())
		{
			BEGIN_MAP("CameraComponent"); //CameraComponent

			const auto &cameraComponent = entity.GetComponent<CameraComponent>();
			const auto &camera          = cameraComponent.Camera;

			BEGIN_MAP("Camera"); //Camera

			VALUE("ProjectionType", static_cast<int>(camera.GetProjectionType()));

			VALUE("PerspectiveFOV", camera.GetPerspectiveVerticalFOV());
			VALUE("PerspectiveNear", camera.GetPerspectiveNearClip());
			VALUE("PerspectiveFar", camera.GetPerspectiveFarClip());

			VALUE("OrthographicSize", camera.GetOrthographicSize());
			VALUE("OrthographicNear", camera.GetOrthographicNearClip());
			VALUE("OrthographicFar", camera.GetOrthographicFarClip());

			END_MAP(); //Camera

			VALUE("Primary", cameraComponent.Primary);
			VALUE("FixedAspectRatio", cameraComponent.FixedAspectRatio);

			END_MAP(); //CameraComponent
		}

		if(entity.HasComponent<LightComponent>())
		{
			BEGIN_MAP("LightComponent"); //LightComponent

			const auto &lightComponent = entity.GetComponent<LightComponent>();
			const auto light           = lightComponent.Light->GetInfo();

			BEGIN_MAP("Light"); //Light

			VALUE("Type", static_cast<int>(light.Type));
			VALUE("Position", light.Position);
			VALUE("Direction", light.Direction);

			VALUE("AmbientColor", light.AmbientColor);
			VALUE("DiffuseColor", light.DiffuseColor);
			VALUE("SpecularColor", light.SpecularColor);

			VALUE("Active", light.Active);

			VALUE("CutOff", light.CutOff);
			VALUE("OuterCutOff", light.OuterCutOff);

			VALUE("Constant", light.Constant);
			VALUE("Linear", light.Linear);
			VALUE("Quadratic", light.Quadratic);

			END_MAP(); //Light

			VALUE("TexturePath", lightComponent.TexturePath);

			END_MAP(); //LightComponent
		}

		if (entity.HasComponent<LuaScriptComponent>())
		{
			BEGIN_MAP("LuaScriptComponent"); //LuaScriptComponent

			const auto &luaScript = entity.GetComponent<LuaScriptComponent>();
			const auto &userValues = luaScript.UserValues; 
			const auto &env = luaScript.Environment;

			//TODO Properties Serialize
			BEGIN_MAP("Properties"); //Properties

			for (auto &name : userValues)
			{
				
			}

			END_MAP(); //Properties

			VALUE("ScriptPath", luaScript.ScriptPath);



			END_MAP(); //LuaScriptComponent
		}

		out << YAML::EndMap;
	}

	SceneSerializer::SceneSerializer(const Ref<Scene> &scene) : m_Scene(scene) {}

	void SceneSerializer::Serialize(const std::string &filePath)
	{
		YAML::Emitter out;

		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << m_Scene->Title();
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registry.each(
		                         [&](auto entityID)
		                         {
			                         Entity entity = {entityID, m_Scene.get()};
			                         if(!entity)
				                         return;

			                         SerializeEntity(out, entity);
		                         }
		                        );
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filePath);
		fout << out.c_str();
	}

	bool SceneSerializer::Deserialize(const std::string &filePath)
	{
		return false;
	}
}
