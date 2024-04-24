#include "pch.h"
#include "Engine/Scene/SceneSerializer.h"

#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Scene.h"

#include "Engine/Utils/LuaSerializer.h"
#include "Engine/Utils/LuaUtils.h"

#include "Engine/Scene/Components.h"


const char* LuaReader(lua_State *L, void *data, size_t *size)
{
	(*size) = 0;

	return nullptr;
}

namespace Game
{
	static void SerializeEntity(LuaSerializer &out, Entity entity)
	{
		out.BeginTable();
		out.Value("Id", entity.GetUUID());

		if(entity.HasComponent<TagComponent>())
		{
			const auto &comp = entity.GetComponent<TagComponent>();

			out.BeginTable("TagComponent");

			out.Value("Tag", comp.Tag);

			out.EndTable();
		}

		if(entity.HasComponent<TransformComponent>())
		{
			const auto &comp = entity.GetComponent<TransformComponent>();

			out.BeginTable("TransformComponent");

			out.Value("Translation", comp.Translation);
			out.Value("Rotation", comp.Rotation);
			out.Value("Scale", comp.Scale);

			out.EndTable();
		}

		if(entity.HasComponent<CameraComponent>())
		{
			const auto &comp   = entity.GetComponent<CameraComponent>();
			const auto &camera = comp.Camera;

			out.BeginTable("CameraComponent");

			if(camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
			{
				out.CustomValue("Type", "Perspective");
				out.Value("FarClip", camera.GetPerspectiveFarClip());
				out.Value("NearClip", camera.GetPerspectiveNearClip());
				out.Value("FOV", glm::degrees(camera.GetPerspectiveVerticalFOV()));
				out.Value("AspectRatio", camera.GetAspectRatio());
			}
			else
			{
				out.CustomValue("Type", "Orthographic");
				out.Value("FarClip", camera.GetOrthographicFarClip());
				out.Value("NearClip", camera.GetOrthographicNearClip());
				out.Value("Size", camera.GetOrthographicSize());
				out.Value("AspectRatio", camera.GetAspectRatio());
			}

			out.EndTable();
		}

		out.EndTable();
	}

	SceneSerializer::SceneSerializer(const Ref<Scene> &scene) : m_Scene(scene) {}

	void SceneSerializer::Serialize(const std::filesystem::path &path)
	{
		LuaSerializer serializer(path.string(), true);

		if(serializer.Good())
		{
			serializer.BeginTable("Scene");
			serializer.Value("Title", m_Scene->Title());

			serializer.BeginTable("Entities");

			/*
			m_Scene->m_Registry.
			m_Scene->m_Registry.each(
			                         [&](auto entityId)
			                         {
				                         Entity entity(entityId, m_Scene.get());
				                         if(!entity)
					                         return;

				                         SerializeEntity(serializer, entity);
			                         }
			                        );*/

			serializer.EndTable().EndTable();
		}
		else
		{
			LOG_ERROR(fmt::format("Unable to open \'{}\' for write mode, scene could not be saved", path.string()));
		}
	}

	void SceneSerializer::Deserialize(const std::filesystem::path &path)
	{
		if(!exists(path))
			throw std::runtime_error(fmt::format("File does not exists: '{}'", path.string()));
	}
}
