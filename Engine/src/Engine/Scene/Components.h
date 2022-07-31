#pragma once

#include "Engine/Core/UUID.h"
#include "Engine/Scene/SceneCamera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Game
{
	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;

		IDComponent(UUID& id) : ID(id){}
	};

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string &tag) : Tag(tag) {}
	};

	struct TransformComponent
	{
		glm::vec3 Translation { 0.f, 0.f, 0.f };
		glm::vec3 Rotation { 0.f, 0.f, 0.f};
		glm::vec3 Scale { 1.f, 1.f, 1.f};

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;

		TransformComponent(const glm::vec3& translation) : Translation(translation) {}
		TransformComponent(const glm::vec3& translation, const glm::vec3 rotation) : Translation(translation), Rotation(rotation) {}
		TransformComponent(const glm::vec3& translation, const glm::vec3 rotation, const glm::vec3 scale) : Translation(translation), Rotation(rotation), Scale(scale) {}

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

			return glm::translate(glm::mat4(1.f), Translation) * rotation * glm::scale(glm::mat4(1.f), Scale);
		}
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	template <typename... Component>
	struct ComponentGroup{};

	using AllComponents = ComponentGroup<TransformComponent, CameraComponent>;
}