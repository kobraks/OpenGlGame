#include "pch.h"
#include "SceneSerializer.h"

#include "Defines.h"
#include "Application.h"
#include "Scene.h"
#include "Entity.h"
#include "Component.h"

#include "ModelLoader.h"
#include "TextureLoader.h"

#include "LuaUtils.h"
#include "Utils.h"
#include "LuaSerializer.h"

namespace Game
{
	static void SerializeEntity(LuaSerializer &out, Entity entity)
	{
		out.BeginTable();
		out.Value("Id", entity.GetUUID());


		if(entity.HasComponent<TagComponent>())
		{
			const auto &comp = entity.GetComponent<TagComponent>();
			out.BeginTable(comp.Name());

			out.Value("Tag", comp.Tag);

			out.EndTable();
		}

		if(entity.HasComponent<TransformComponent>())
		{
			const auto &comp = entity.GetComponent<TransformComponent>();
			out.BeginTable(comp.Name());

			out.Value("Position", comp.GetPosition());
			out.Value("Rotation", comp.GetRotation());
			out.Value("Scale", comp.GetScale());

			out.EndTable();
		}

		if(entity.HasComponent<CameraComponent>())
		{
			const auto &comp   = entity.GetComponent<CameraComponent>();
			const auto &camera = comp.Camera;

			out.BeginTable(comp.Name());

			if(camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
			{
				out.Value("Type", "Perspective");
				out.Value("FarClip", camera.GetPerspectiveFarClip());
				out.Value("NearClip", camera.GetPerspectiveNearClip());
				out.Value("FOV", glm::degrees(camera.GetPerspectiveVerticalFOV()));
				out.Value("AspectRatio", camera.GetAspectRatio());
			}
			else
			{
				out.Value("Type", "Orthographic");
				out.Value("FarClip", camera.GetOrthographicFarClip());
				out.Value("NearClip", camera.GetOrthographicNearClip());
				out.Value("Size", glm::degrees(camera.GetOrthographicSize()));
				out.Value("AspectRatio", camera.GetAspectRatio());
			}

			out.EndTable();
		}

		if(entity.HasComponent<LightComponent>())
		{
			const auto &comp = entity.GetComponent<LightComponent>();
			const auto light = comp.Light;

			if(light)
			{
				if(const auto lightInfo = light->GetInfo(); lightInfo.Type != LightType::Unknown)
				{
					out.BeginTable(comp.Name());
					out.Value("Active", lightInfo.Active);

					if (lightInfo.Type == LightType::Directional)
						out.Value("Type", "LightType.Directional");
					if (lightInfo.Type == LightType::Point)
						out.Value("Type", "LightType.Point");
					if (lightInfo.Type == LightType::Spot)
						out.Value("Type", "LightType.Spot");

					out.Color("DiffuseColor", lightInfo.DiffuseColor);
					out.Color("AmbientColor", lightInfo.AmbientColor);
					out.Color("SpecularColor", lightInfo.SpecularColor);

					if (lightInfo.Type == LightType::Point || lightInfo.Type == LightType::Spot)
					{
						out.Value("Constant", lightInfo.Constant);
						out.Value("Linear", lightInfo.Linear);
						out.Value("Quadratic", lightInfo.Quadratic);

						if (lightInfo.Type == LightType::Spot)
						{
							out.Value("CutOff", lightInfo.CutOff);
							out.Value("OuterCutOff", lightInfo.OuterCutOff);
							out.String("LightCookie", comp.TexturePath);
						}
					}
					
					out.EndTable();
				}
			}
		}

		if(entity.HasComponent<ModelComponent>())
		{
			const auto &comp = entity.GetComponent<ModelComponent>();
			out.BeginTable(comp.Name());

			out.Value("Path", fmt::format("\"{}\"", comp.ModelPath));
			out.Value("Drawable", comp.Drawable);

			out.EndTable();
		}
		if(entity.HasComponent<LuaScriptComponent>())
		{
			const auto &comp = entity.GetComponent<LuaScriptComponent>();
			out.BeginTable(comp.Name());

			//TODO Properties
			out.Value("Path", fmt::format("\"{}\"", comp.ScriptPath));

			out.EndTable();
		}

		out.EndTable();
	}



	SceneSerializer::SceneSerializer(const Pointer<Scene> scene) : m_Scene(scene)
	{
		m_AppState = &Application::Get().GetLua();
	}

	void SceneSerializer::Serialize(const std::string &filePath)
	{
		LuaSerializer serializer(filePath);

		if(serializer.Good())
		{
			serializer.BeginTable("Scene");
			serializer.Value("Title", fmt::format("\"{}\"", m_Scene->Title()));

			serializer.BeginTable("Entities");

			m_Scene->m_Registry.each(
			                         [&](auto entityId)
			                         {
				                         Entity entity{entityId, m_Scene.get()};
				                         if(!entity)
					                         return;

				                         SerializeEntity(serializer, entity);
			                         }
			                        );


			serializer.EndTable();
			serializer.EndTable();
		}
		else
		{
			LOG_ERROR("Unable to open \"{}\" scene not saved", filePath);
		}
	}
}
