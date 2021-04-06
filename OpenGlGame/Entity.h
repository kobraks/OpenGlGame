#pragma once

#include <string>
#include <unordered_map>
#include <typeindex>

#include "Types.h"

namespace Game
{
	class Component;
	class EntityLuaHandle;
	
	class Entity
	{
	public:
		using ComponentStorageType = std::unordered_map<std::type_index, Pointer<Component>>;

	
	private:
		size_t m_Id = 0;
		std::string m_Name;

		ComponentStorageType m_Components;
		
		Pointer<EntityLuaHandle> m_LuaHandle;
	public:
		Entity(const size_t &id, const std::string &name);

		size_t Id() const { return m_Id; };
		const std::string& Name() const { return m_Name; };

		void SetName(const std::string &name) { m_Name = name; }

		Pointer<EntityLuaHandle> GetLuaHandle() const { return m_LuaHandle; }
		void SetLuaHandle(Pointer<EntityLuaHandle> handle);
		
		void AddComponent(std::type_index index, Pointer<Component> component);

		Pointer<Entity> Clone() const;

		ComponentStorageType::iterator begin() { return m_Components.begin(); }
		ComponentStorageType::iterator end() { return m_Components.end(); }

		ComponentStorageType::const_iterator begin() const { return m_Components.begin(); }
		ComponentStorageType::const_iterator end() const { return m_Components.end(); }
	
	public:
		template <typename Type>
		void AddComponent(Pointer<Component> component);

		template <typename Type>
		void AddComponent();

		template <typename Type>
		Pointer<Type> GetComponent();

		template <typename Type>
		void RemoveComponent();

		template <typename Type>
		bool HasComponent();
	};

	template <typename Type>
	void Entity::AddComponent(Pointer<Component> component)
	{
		AddComponent(typeid(Type), component);
	}

	template <typename Type>
	void Entity::AddComponent()
	{
		AddComponent<Type>(MakePointer<Type>(this));
	}

	template <typename Type>
	Pointer<Type> Entity::GetComponent()
	{
		const auto component = m_Components.find(std::type_index(typeid(Type)));
		if(component != m_Components.end())
			return std::dynamic_pointer_cast<Type>(component->second);

		return nullptr;
	}

	template <typename Type>
	void Entity::RemoveComponent()
	{
		const auto component = m_Components.find(typeid(Type));
		if(component != m_Components.end())
			m_Components.erase(component);
	}

	template <typename Type>
	bool Entity::HasComponent()
	{
		return GetComponent<Type>();
	}
}
