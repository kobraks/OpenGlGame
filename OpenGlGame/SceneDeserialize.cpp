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
	namespace Priv
	{
		/// <summary>
		/// 
		/// </summary>
		/// <typeparam name="Type"></typeparam>
		/// <param name="obj">Object</param>
		/// <param name="validKey">Valid key</param>
		/// <param name="key">Recived key</param>
		/// <param name="value">Value to be set</param>
		/// <returns>-1 in case of key != validKey
		/// 0 in case of success
		/// 1 in case of invalid value
		/// 2 in case of wrong type</returns>
		template <typename Type>
		auto GetValue(sol::object obj, std::string validKey, const std::string &key, Type &value)->int
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

		/// <summary>
		/// 
		/// </summary>
		/// <typeparam name="Type"></typeparam>
		/// <param name="obj">Object</param>
		/// <param name="validKey">Valid key</param>
		/// <param name="key">Recived key</param>
		/// <param name="value">Value to be set</param>
		/// <returns>-1 in case of key != validKey
		/// 0 in case of success
		/// 1 in case of invalid value
		/// 2 in case of wrong type</returns>
		auto GetValue3(sol::table obj, std::string validKey, const std::string &key, glm::vec3 &value)->int
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
		auto ProcessComponentPa(uint64_t index, Component &comp, sol::table componentTable)->void
		{
			static_assert(false);
		}

		template <>
		auto ProcessComponentPa(uint64_t index, TagComponent &comp, sol::table componentTable)->void
		{
			for(auto [key, value] : componentTable)
			{
				const auto upperKey = ToUpperCopy(TrimCopy(key.as<std::string>()));
				if(GetValue<std::string>(value, "Tag", upperKey, comp.Tag) >= 0);
				else
					LOG_WARN("Unknwon key value {}", key.as<std::string>());
			}
		}

		template <>
		auto ProcessComponentPa(uint64_t index, TransformComponent &comp, sol::table componentTable)->void
		{
			glm::vec3 vec;

			for(auto [key, value] : componentTable)
			{
				const auto upperKey = ToUpperCopy(TrimCopy(key.as<std::string>()));

				if(GetValue3(value.as<sol::table>(), "Position", upperKey, vec) >= 0)
					comp.SetPosition(vec);
				else if(GetValue3(value.as<sol::table>(), "Rotation", upperKey, vec) >= 0)
					comp.SetRotation(vec);
				else if(GetValue3(value.as<sol::table>(), "Scale", upperKey, vec) >= 0)
					comp.SetScale(vec);
				else
					LOG_WARN("Unknown key value: {}", key.as<std::string>());
			}
		}

		template <>
		auto ProcessComponentPa(uint64_t index, CameraComponent &comp, sol::table componentTable)->void
		{
			auto type = SceneCamera::ProjectionType::Perspective;

			float nearClip = 0.f;
			float farClip  = 0.f;
			float fov      = 0.f;
			float size     = 0.f;
			float aspect   = 0.f;

			bool primary = false;
			bool fixed   = false;

			for(auto [key, value] : componentTable)
			{
				const auto upperKey = ToUpperCopy(TrimCopy(key.as<std::string>()));

				if(GetValue<bool>(value, "Primary", upperKey, primary) >= 0);
				else if(GetValue<bool>(value, "FixedAspectRatio", upperKey, fixed) >= 0);
				else if(GetValue<SceneCamera::ProjectionType>(value, "Type", upperKey, type) >= 0);
				else if(GetValue<float>(value, "NearClip", upperKey, nearClip) >= 0);
				else if(GetValue<float>(value, "FarClip", upperKey, farClip) >= 0);
				else if(GetValue<float>(value, "FOV", upperKey, fov) >= 0);
				else if(GetValue<float>(value, "Size", upperKey, size) >= 0);
				else if(GetValue<float>(value, "AspectRatio", upperKey, aspect) >= 0);
				else
					LOG_WARN("Unknown key value: {}", key.as<std::string>());
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
		auto ProcessComponentPa(uint64_t index, LightComponent &comp, sol::table componentTable)->void
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

			for(auto [key, value] : componentTable)
			{
				const auto upperKey = ToUpperCopy(TrimCopy(key.as<std::string>()));

				if(GetValue<bool>(value, "Active", upperKey, active) >= 0);
				else if(GetValue<float>(value, "Constant", upperKey, constant) >= 0);
				else if(GetValue<float>(value, "Linear", upperKey, linear) >= 0);
				else if(GetValue<float>(value, "Quadratic", upperKey, quadratic) >= 0);
				else if(GetValue<float>(value, "CutOff", upperKey, cutOff) >= 0);
				else if(GetValue<float>(value, "OuterCutOff", upperKey, outerCutOff) >= 0);
				else if(GetValue<std::string>(value, "LightCookie", upperKey, cookie) >= 0);
				else if(GetValue<LightType>(value, "Type", upperKey, type) >= 0);
				else if(value.is<sol::table>() && GetValue3(value, "DiffuseColor", upperKey, diffuse) >= 0);
				else if(value.is<sol::table>() && GetValue3(value, "AmbientColor", upperKey, ambient) >= 0);
				else if(value.is<sol::table>() && GetValue3(value, "SpecularColor", upperKey, specular) >= 0);
				else
					LOG_WARN("Unknown key value: {}", key.as<std::string>());
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
		auto ProcessComponentPa(uint64_t index, LuaScriptComponent &comp, sol::table componentTable)->void
		{
			for(auto [key, value] : componentTable)
			{
				const auto upperKey = ToUpperCopy(TrimCopy(key.as<std::string>()));

				if(GetValue<std::string>(value, "Path", upperKey, comp.ScriptPath) >= 0)
					comp.OpenFile(comp.ScriptPath);
				else if(upperKey == "PROPERTIES"); //TODO Parameters for scripts
				else
					LOG_WARN("Unknown key value: {}", key.as<std::string>());
			}
		}

		template <>
		auto ProcessComponentPa(uint64_t index, ModelComponent &comp, sol::table componentTable)->void
		{
			for(auto [key, value] : componentTable)
			{
				const auto upperKey = ToUpperCopy(TrimCopy(key.as<std::string>()));

				if(GetValue<std::string>(value, "Path", upperKey, comp.ModelPath) >= 0)
					comp.LoadModel(comp.ModelPath);
				else if(GetValue<bool>(value, "Drawable", upperKey, comp.Drawable) >= 0);
				else
					LOG_WARN("Unknown key value: {}", key.as<std::string>());
			}
		}

		template <typename Component>
		auto ProcessComponentP(uint64_t index, Entity entity, std::string name, sol::table componentTable)->void
		{
			if(entity.HasComponent<Component>())
			{
				LOG_WARN("Entity {} already has {}", index, name);
				LOG_DEBUG("Replacing {} to entity: {}", index, name);
			}

			auto &component = entity.AddComponent<Component>();
			ProcessComponentPa<Component>(index, component, componentTable);
		}
	}

	auto SceneSerializer::Deserialize(const std::string &filePath)->bool
	{
		try
		{
			sol::state state;
			// state->open_libraries(sol::lib::string);

			state.new_enum<LightType>(
			                          "LightType",
			                          {ENUM(LightType, Directional), ENUM(LightType, Spot), ENUM(LightType, Point)}
			                         );
			state.new_enum<SceneCamera::ProjectionType>(
			                                            "CameraType",
			                                            {
				                                            ENUM(SceneCamera::ProjectionType, Perspective),
				                                            ENUM(SceneCamera::ProjectionType, Orthographic)
			                                            }
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

	auto SceneSerializer::ReadScene(sol::state &state)->void
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

	auto SceneSerializer::ProcessScene(sol::table scene)->void
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

	auto SceneSerializer::ProcessEntityTable(sol::table entities)->void
	{
		struct EntityStruct
		{
			uint64_t Key = 0;
			uint64_t Id  = 0;
			bool HasId   = false;
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
			if(!key.is<uint64_t>())
				throw Exception("Table name is not an number");

			if(value != sol::nil && value.is<sol::table>())
			{
				EntityStruct ent;
				ent.EntityTable = value.as<sol::table>();
				ent.Key         = key.as<uint64_t>();

				FindEntityId(ent.EntityTable, ent.Id, ent.HasId);

				ents.emplace_back(ent);

				if(ent.Id >= 0)
				{
					LOG_DEBUG("Used static id for entity: {}, id = {}", ent.Key, ent.Id);
				}
				else
					LOG_DEBUG("Used auto id for entity: {}", ent.Key);
			}
			else
			{
				LOG_WARN("Unknown value under {}", key.as<std::string>());
			}
		}

		std::ranges::sort(ents, [](const EntityStruct &e1, const EntityStruct &e2) { return e1.Id > e2.Id; });

		for(const auto &[key, id, HasId, entityTable] : ents)
			ProcessEntity(key, !HasId ? m_Scene->CreateEntity() : m_Scene->CreateEntity(UUID(id)), entityTable);
	}

	auto SceneSerializer::FindEntityId(sol::table entityTable, uint64_t &id, bool &found)->void
	{
		id    = -1;
		found = false;

		for(auto [key, value] : entityTable)
		{
			auto k = ToUpperCopy(key.as<std::string>());

			if(k == "ID")
			{
				if(value != sol::nil && value.is<uint64_t>())
				{
					id    = value.as<uint64_t>();
					found = true;
				}
				else
					LOG_WARN("Unable to read id value");

				break;
			}
		}
	}

	auto SceneSerializer::ProcessEntity(uint64_t index, Entity entity, sol::table entityTable)->void
	{
		LOG_DEBUG("Processing entity {}, table", index);

		if(!entityTable.valid())
		{
			throw Exception("Entity table is invalid");
		}

		for(auto [key, value] : entityTable)
		{
			const auto trimedKey = TrimCopy(key.as<std::string>());
			const auto upperKey  = ToUpperCopy(trimedKey);

			if(upperKey == "ID");
			else if(value.is<sol::table>())
				ProcessComponent(index, entity, trimedKey, value.as<sol::table>());
			else
				LOG_WARN("{} Value is not valid table of entity: {}", key.as<std::string>(), index);
		}
	}

	auto SceneSerializer::ProcessComponent(uint64_t index, Entity entity, std::string name, sol::table component)->void
	{
		LOG_DEBUG("Processing component: {}", name);
		const std::string upperName = ToUpperCopy(name);

		if(upperName == "TAGCOMPONENT")
			Priv::ProcessComponentP<TagComponent>(index, entity, name, component);
		else if(upperName == "TRANSFORMCOMPONENT")
			Priv::ProcessComponentP<TransformComponent>(index, entity, name, component);
		else if(upperName == "CAMERACOMPONENT")
			Priv::ProcessComponentP<CameraComponent>(index, entity, name, component);
		else if(upperName == "LIGHTCOMPONENT")
			Priv::ProcessComponentP<LightComponent>(index, entity, name, component);
		else if(upperName == "LUASCRIPTCOMPONENT")
			Priv::ProcessComponentP<LuaScriptComponent>(index, entity, name, component);
		else if(upperName == "MODELCOMPONENT")
			Priv::ProcessComponentP<ModelComponent>(index, entity, name, component);
		else
			LOG_WARN("Ignoring unknown component named: {}", name);
	}
}
