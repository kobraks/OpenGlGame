#pragma once
#include <string>
#include <utility>

#include "Types.h"
#include "SceneCamera.h"
#include "Light.h"
#include "Transformable.h"
#include "ScriptableEntity.h"

namespace Game
{
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

		const std::string& Name() const { return m_Name; }

		Entity* GetEntity() const { return m_Entity; }
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

		~LightComponent() override
		{
			delete Light;
		}

		Light *Light = nullptr;;
	};

	class LuaScriptComponent: public Component
	{
	public:
		LuaScriptComponent(Entity *entity = nullptr) : Component("LuaScriptComponent", entity) {}

		void OpenFile(const std::string &fileName);
	};

	class ModelComponent: public Component
	{
	public:
		ModelComponent(Entity *entity = nullptr) : Component("ModelComponent", entity) {}

		Pointer<Model> Model;

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
