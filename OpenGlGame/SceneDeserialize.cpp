#include "pch.h"
#include "SceneSerializer.h"

#include "Defines.h"
#include "Application.h"

#include "ModelLoader.h"
#include "TextureLoader.h"

#include "Scene.h"
#include "Entity.h"
#include "Component.h"

#include "LuaUtils.h"
#include "Utils.h"

#include "Exception.h"

#define ENUM(e, v) {#v, e##::##v}

namespace Game
{
	bool SceneSerializer::Deserialize(const std::string &filePath)
	{
		try
		{
			sol::state state;
			// state->open_libraries(sol::lib::string);

			state.new_enum<LightType>("LightType", {ENUM(LightType, Directional), ENUM(LightType, Spot), ENUM(LightType, Point)});
			state.new_enum<SceneCamera::ProjectionType>(
			                                            "CameraType",
			                                            {ENUM(SceneCamera::ProjectionType, Perspective), ENUM(SceneCamera::ProjectionType, Orthographic)}
			                                           );


			LOG_INFO("Opening \"{}\"", filePath);
			const auto result = state.do_file(filePath);

			if(!result.valid())
			{
				const sol::error err = result;

				LOG_ERROR("Unable to load \"{}\". because: {}", filePath, err.what());
				return false;
			}

			ReadScene(state);
		}
		catch(std::exception &ex)
		{
			LOG_ERROR("Unable to deserialize scene file \"{}\": exception thrown: \"{}\"", filePath, ex.what());
			return false;
		}

		return true;
	}

	void SceneSerializer::ReadScene(sol::state &state)
	{
		for(auto [k, v] : state.globals())
		{
			if(const std::string uk = ToUpperCopy(k.as<std::string>()); uk == "SCENE")
			{
				LOG_DEBUG("Found Scene key");

				if(v == sol::nil)
					throw Exception("Scene is an Nil value");
				if(v.is<sol::table>())
					ProcessScene(v.as<sol::table>());
				else
					throw Exception("Scene is not an table type");

				return;
			}
		}
	}

	void SceneSerializer::ProcessScene(sol::table scene)
	{
		LOG_DEBUG("Processing Scene table")

		if(!scene.valid())
		{
			throw Exception("Scene is not valid value");
		}

		for(auto [k, v] : scene)
		{
			const std::string key = k.as<std::string>();

			if(const auto upperKey = ToUpperCopy(key); upperKey == "TITLE")
			{
				LOG_DEBUG("Found scene Title key");
				if(v.is<std::string>())
				{
					const auto title = v.as<std::string>();
					m_Scene->SetTitle(title);
				}
				else
					LOG_WARN("Title value is not valid string");
			}
			else if(upperKey == "PROPERTIES")
			{
				LOG_DEBUG("Found properties key");
				//TODO Poperties
			}
			else if(upperKey == "ENTITIES")
			{
				LOG_DEBUG("Found entities key");

				if(v != sol::nil && v.is<sol::table>())
					ProcessEntityTable(v.as<sol::table>());
				else
					throw Exception("Entities type is not an valid table");
			}
			else
				LOG_WARN("Unrecognized variable key: ", key);
		}
	}

	void SceneSerializer::ProcessEntityTable(sol::table entities)
	{
		struct EntityStruct
		{
			int64_t Id = 0;
			sol::table EntityTable;
		};

		LOG_DEBUG("Processing entities");

		if(!entities.valid())
			throw Exception("Entity table is not valid");

		LOG_DEBUG("Entities count: {}", entities.size());
		std::vector<EntityStruct> ents;
		ents.reserve(entities.size());

		//Searching for an id
		for(const auto [key, value] : entities)
		{
			if(value.is<sol::table>())
			{
				EntityStruct ent;
				ent.EntityTable = value.as<sol::table>();
				ent.Id          = FindEntityId(ent.EntityTable);

				ents.emplace_back(ent);
			}
			else
			{
				LOG_WARN("Unknown value under {}", key.as<std::string>());
			}
		}

		std::ranges::sort(ents, [](const EntityStruct &e1, const EntityStruct &e2) { return e1.Id > e2.Id; });

		for(const auto &ent : ents)
			ProcessEntity(ent.Id < 0 ? m_Scene->CreateEmpty() : m_Scene->CreateEmpty(ent.Id), ent.EntityTable);
	}

	int64_t SceneSerializer::FindEntityId(sol::table entityTable)
	{
		int64_t id = -1;

		for(auto [key, value] : entityTable)
		{
			auto k = ToUpperCopy(key.as<std::string>());

			if(k == "ID")
			{
				LOG_DEBUG("Id found");

				if(value.is<uint32_t>())
				{
					id = value.as<uint32_t>();
					LOG_DEBUG("Setting id to: {}", id);
				}
				else
				{
					LOG_WARN("Unable to read id value");
				}

				break;
			}
		}

		return id;
	}

	void SceneSerializer::ProcessEntity(Entity entity, sol::table entityTable)
	{
		LOG_DEBUG("Processing entity table");

		if(!entityTable.valid())
		{
			throw std::exception("Entity table is invalid");
		}

		for(auto [key, value] : entityTable)
		{
			if(key.is<std::string>())
			{
				if(ToUpperCopy(TrimCopy(key.as<std::string>())) == "ID");
				else if(value.is<sol::table>())
					ProcessComponent(entity, TrimCopy(key.as<std::string>()), value.as<sol::table>());
				else
					LOG_WARN("{} Value is not valid table", key.as<std::string>());
			}
			else
				LOG_WARN("Key is not valid string");
		}
	}

	template <typename Type>
	int GetValue(sol::object obj, std::string validKey, const std::string &key, Type &value)
	{
		const std::string lowerKey = Trim(validKey);

		validKey = ToUpperCopy(validKey);

		if(key == validKey)
		{
			LOG_DEBUG("Found {} key", lowerKey);
			if(obj.valid())
			{
				if(obj.is<Type>())
				{
					value = obj.as<Type>();
					LOG_DEBUG("Readed {}", value);
					return 0;
				}
				LOG_DEBUG("valuse of {} has unexpected type", lowerKey);
				return 2;
			}
			LOG_DEBUG("Value is not valid");
			return 1;
		}

		return -1;
	}

	int GetValue3(sol::table obj, std::string validKey, const std::string &key, glm::vec3 &value)
	{
		const std::string lowerKey = Trim(validKey);

		validKey = ToUpperCopy(validKey);

		if(key == validKey)
		{
			LOG_DEBUG("Found {} key", lowerKey);
			if(obj.valid())
			{
				if(obj.is<sol::table>())
				{
					value = ReadVector3(obj);
					LOG_DEBUG("Readed [{}, {}, {}]", value.x, value.y, value.z);
					return 0;
				}
				LOG_DEBUG("valuse of {} has unexpected type", lowerKey);
				return 2;
			}
			LOG_DEBUG("Value is not valid");
			return 1;
		}

		return -1;
	}

	template <typename Component>
	void ProcessComponentPa(Component &comp, sol::table component)
	{
		static_assert(false);
	}

	template <>
	void ProcessComponentPa(TagComponent &comp, sol::table component)
	{
		for(auto [k, v] : component)
		{
			if(!k.is<std::string>())
				throw std::exception(fmt::format("Key value of {} is not an valid string", comp.Name()).c_str());

			const auto uKey = TrimCopy(ToUpperCopy(k.as<std::string>()));

			if(GetValue<std::string>(v, "Tag", uKey, comp.Tag) >= 0);
			else
				LOG_WARN("Ignoring uknown key {}", k.as<std::string>());
		}
	}

	template <>
	void ProcessComponentPa(TransformComponent &comp, sol::table component)
	{
		glm::vec3 vec;

		for(auto [k, v] : component)
		{
			if(!k.is<std::string>())
				throw std::exception(fmt::format("Key value of {} is not an valid string", comp.Name()).c_str());
			const auto uKey = TrimCopy(ToUpperCopy(k.as<std::string>()));

			if(GetValue3(v.as<sol::table>(), "Position", uKey, vec) >= 0)
				comp.SetPosition(vec);
			else if(GetValue3(v.as<sol::table>(), "Rotation", uKey, vec) >= 0)
				comp.SetRotation(vec);
			else if(GetValue3(v.as<sol::table>(), "Scale", uKey, vec) >= 0)
				comp.SetScale(vec);
			else
				LOG_WARN("Unknown key value: {}", k.as<std::string>());
		}
	}

	template <>
	void ProcessComponentPa(CameraComponent &comp, sol::table component)
	{
		auto type = SceneCamera::ProjectionType::Perspective;

		float nearClip = 0.f;
		float farClip  = 0.f;
		float fov      = 0.f;
		float size     = 0.f;
		float aspect   = 0.f;

		bool primary = false;
		bool fixed   = false;

		for(auto [k, v] : component)
		{
			if(!k.is<std::string>())
				throw std::exception(fmt::format("Key value of {} is not an valid string", comp.Name()).c_str());

			const auto key = TrimCopy(ToUpperCopy(k.as<std::string>()));

			if(GetValue<bool>(v, "Primary", key, primary) >= 0);
			else if(GetValue<bool>(v, "FixedAspectRatio", key, fixed) >= 0);
			else if(GetValue<SceneCamera::ProjectionType>(v, "Type", key, type) >= 0);
			else if(GetValue<float>(v, "NearClip", key, nearClip) >= 0);
			else if(GetValue<float>(v, "FarClip", key, farClip) >= 0);
			else if(GetValue<float>(v, "FOV", key, fov) >= 0);
			else if(GetValue<float>(v, "Size", key, size) >= 0);
			else if(GetValue<float>(v, "AspectRatio", key, aspect) >= 0);
			else
				LOG_WARN("Unknown key value: {}", k.as<std::string>());
		}

		comp.FixedAspectRatio = fixed;
		comp.Primary          = primary;

		if(type == SceneCamera::ProjectionType::Perspective)
		{
			comp.Camera.SetPerspective(fov, nearClip, farClip);
			comp.Camera.SetAspectRatio(aspect);
		}
		else
		{
			comp.Camera.SetOrthographic(size, nearClip, farClip);
			comp.Camera.SetAspectRatio(aspect);
		}
	}

	template <>
	void ProcessComponentPa(LightComponent &comp, sol::table component)
	{
		bool active = false;
		auto type   = LightType::Directional;

		glm::vec3 diffuse;
		glm::vec3 ambient;
		glm::vec3 specular;

		float constant  = 0.f;
		float linear    = 0.f;
		float quadratic = 0.f;

		float cutOff      = 0.f;
		float outerCutOff = 0.f;

		std::string cookie = {};

		for(auto [k, v] : component)
		{
			if(!k.is<std::string>())
				throw std::exception(fmt::format("Key value of {} is not an valid string", comp.Name()).c_str());

			const auto key = TrimCopy(ToUpperCopy(k.as<std::string>()));

			if(GetValue<bool>(v, "Active", key, active) >= 0);
			else if(GetValue<float>(v, "Constant", key, constant) >= 0);
			else if(GetValue<float>(v, "Linear", key, linear) >= 0);
			else if(GetValue<float>(v, "Quadratic", key, quadratic) >= 0);
			else if(GetValue<float>(v, "CutOff", key, cutOff) >= 0);
			else if(GetValue<float>(v, "OuterCutOff", key, outerCutOff) >= 0);
			else if(GetValue<std::string>(v, "LightCookie", key, cookie) >= 0);
			else if(GetValue<LightType>(v, "Type", key, type) >= 0);
			else if(v.is<sol::table>() && GetValue3(v, "DiffuseColor", key, diffuse) >= 0);
			else if(v.is<sol::table>() && GetValue3(v, "AmbientColor", key, ambient) >= 0);
			else if(v.is<sol::table>() && GetValue3(v, "SpecularColor", key, specular) >= 0);
			else
				LOG_WARN("Unknown key value: {}", k.as<std::string>());
		}

		Pointer<Light> light = nullptr;
		if(type == LightType::Directional)
			light = comp.Light = MakePointer<DirectionalLight>();
		if(type == LightType::Spot || type == LightType::Point)
		{
			Pointer<PointLight> l = nullptr;

			if(type == LightType::Point)
				l = MakePointer<PointLight>();
			else
			{
				Pointer<SpotLight> l2 = MakePointer<SpotLight>();

				if(!cookie.empty())
					l2->SetTexture(TextureLoader::Load(cookie, TEXTURES_PATH));

				l2->SetOuterCutOff(outerCutOff);
				l2->SetCutOff(cutOff);

				l = l2;
			}

			l->SetConstant(constant);
			l->SetLinear(linear);
			l->SetQuadratic(quadratic);
			light = l;
		}

		if(light)
		{
			light->SetActive(active);
			light->SetAmbientColor(ambient);
			light->SetAmbientColor(diffuse);
			light->SetAmbientColor(specular);

			comp.Light       = light;
			comp.TexturePath = cookie;
		}
	}

	template <>
	void ProcessComponentPa(LuaScriptComponent &comp, sol::table component)
	{
		for(auto [k, v] : component)
		{
			if(!k.is<std::string>())
				throw std::exception(fmt::format("Key value of {} is not an valid string", comp.Name()).c_str());

			const auto key = TrimCopy(ToUpperCopy(k.as<std::string>()));

			if(GetValue<std::string>(v, "Path", key, comp.ScriptPath) >= 0)
				comp.OpenFile(comp.ScriptPath);
			else if(key == "PROPERTIES"); //TODO Parameters for scripts
			else
				LOG_WARN("Unknown key value: {}", k.as<std::string>());
		}
	}

	template <>
	void ProcessComponentPa(ModelComponent &comp, sol::table component)
	{
		for(auto [k, v] : component)
		{
			if(!k.is<std::string>())
				throw std::exception(fmt::format("Key value of {} is not an valid string", comp.Name()).c_str());

			const auto key = TrimCopy(ToUpperCopy(k.as<std::string>()));

			if(GetValue<std::string>(v, "Path", key, comp.ModelPath) >= 0)
				comp.LoadModel(comp.ModelPath);
			else if(GetValue<bool>(v, "Drawable", key, comp.Drawable) >= 0);
			else
				LOG_WARN("Unknown key value: {}", k.as<std::string>());
		}
	}

	template <typename Component>
	void ProcessComponentP(Entity entity, std::string name, sol::table component)
	{
		if(entity.HasComponent<Component>())
		{
			LOG_WARN("Entity already has {}", name);
			return;
		}

		LOG_DEBUG("Adding {}", name);
		auto &comp = entity.AddComponent<Component>();

		ProcessComponentPa<Component>(comp, component);
	}

	void SceneSerializer::ProcessComponent(Entity entity, std::string name, sol::table component)
	{
		LOG_DEBUG("Processing component: {}", name);
		const std::string upperName = ToUpperCopy(name);

		if(upperName == "TAGCOMPONENT")
			ProcessComponentP<TagComponent>(entity, name, component);
		else if(upperName == "TRANSFORMCOMPONENT")
			ProcessComponentP<TransformComponent>(entity, name, component);
		else if(upperName == "CAMERACOMPONENT")
			ProcessComponentP<CameraComponent>(entity, name, component);
		else if(upperName == "LIGHTCOMPONENT")
			ProcessComponentP<LightComponent>(entity, name, component);
		else if(upperName == "LUASCRIPTCOMPONENT")
			ProcessComponentP<LuaScriptComponent>(entity, name, component);
		else if(upperName == "MODELCOMPONENT")
			ProcessComponentP<ModelComponent>(entity, name, component);
		else
			LOG_WARN("Ignoring unknown component named: {}", name);
	}
}
