#pragma once
#include <string>
#include <utility>
#include <string>

#include <sol/environment.hpp>

#include "Types.h"
#include "SceneCamera.h"
#include "Light.h"
#include "Transformable.h"
// #include "ScriptableEntity.h"
// #include "UUID.h"

namespace Game
{
	class ScriptableEntity;
	class EntityLuaHandle;
	class Entity;
	class Model;

	class Component
	{
		std::string m_Name;
		Entity *m_Entity;
	public:
		Component(std::string name, Entity *entity = nullptr) : m_Name(std::move(name)),
		                                                        m_Entity(entity) {}

		virtual ~Component() {}

		const std::string &Name() const { return m_Name; }
		Entity* GetEntity() const { return m_Entity; }
	};

	class IDComponent : public Component
	{
	public:
		IDComponent(Entity* entity = nullptr) : Component("IDComponent", entity) {}

		// UUID ID;
	};

	class TagComponent: public Component
	{
	public:
		TagComponent(Entity *entity = nullptr) : Component("TagComponent", entity) {}

		std::string Tag;
	};

	class TransformComponent: public Component, public Transformable
	{
	public:
		TransformComponent(Entity *entity = nullptr) : Component("TransformComponent", entity) {}
	};

	class CameraComponent: public Component
	{
	public:
		CameraComponent(Entity *entity = nullptr) : Component("CameraComponent", entity) {}

		SceneCamera Camera;
		bool Primary          = true;
		bool FixedAspectRatio = false;
	};

	class LightComponent: public Component
	{
	public:
		LightComponent(Entity *entity = nullptr) : Component("LightComponent", entity) {}

		Pointer<Light> Light = nullptr;;
		std::string TexturePath = {};
	};

	class LuaScriptComponent: public Component
	{
	public:
		LuaScriptComponent(Entity *entity = nullptr) : Component("LuaScriptComponent", entity) {}

		sol::environment Environment;
		std::vector<std::string> UserValues;
		
		std::string ScriptPath = {};

		EntityLuaHandle *Handle = nullptr;
		
		void OpenFile(const std::string &fileName);
	};

	class ModelComponent: public Component
	{
	public:
		ModelComponent(Entity *entity = nullptr) : Component("ModelComponent", entity) {}

		Pointer<Model> Model;
		bool Drawable = true;
		std::string ModelPath = {};

		void LoadModel(const std::string &fileName);
		void Draw(const glm::mat4 &transform = glm::mat4(1.f)) const;
	};

	class NativeScriptComponent: public Component
	{
	public:
		NativeScriptComponent(Entity *entity = nullptr) : Component("NativeScriptComponent", entity) {}

		ScriptableEntity *Instance = nullptr;

		ScriptableEntity*(*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent *);

		template <typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript     = [](NativeScriptComponent *nsc)
			{
				delete nsc->Instance;
				nsc->Instance = nullptr;
			};
		}
	};
}
