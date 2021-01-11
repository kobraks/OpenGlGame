#pragma once

#include <vector>
#include <array>
#include <typeindex>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "BufferObject.h"
#include "Color.h"
#include "Vector2.h"

#include "Utils.h"

#include "OpenGlFunctions.h"
#include "Log.h"

namespace Game
{
	template <typename T, typename ...Ts>
	class UniformBuffer: public BufferObject
	{
		struct TypeInfo
		{
			size_t Offset = 0;
			size_t Size   = 0;
		};

		char *m_Storage;

		std::array<std::pair<std::string, std::vector<TypeInfo>>, TypeListLengthV<Ts...> + 1> m_Types;
		size_t m_Offset = 0;
		size_t m_Size   = 0;
	public:
		UniformBuffer() : BufferObject(BufferType::Uniform)
		{
			size_t index = 0;
			RegisterTypes<T, Ts...>(index);
			m_Size += m_Offset;

			m_Storage = new char[m_Size];
			Allocate(m_Size);
		}

		template <typename Type>
		void Set(size_t index, const Type &value)
		{
			ASSERT(index < m_Size, "Out of range");
			if(index >= m_Size) throw std::out_of_range("Out of range");

			const auto &[name, typeInfo] = m_Types[index];
			ASSERT(typeid(Type).name() == name, "Not same type as registered");
			if(typeid(Type).name() != name) throw std::runtime_error("Not same type as registered");

			if constexpr(std::is_same_v<Type, glm::mat4> || std::is_same_v<Type, glm::mat3>)
			{
				for (size_t i = 0; i < Type::length(); ++i)
					Set<glm::vec4>(value[i], typeInfo[i].Offset);
			}
			else if constexpr(std::is_same_v<Type, glm::mat4x2> || std::is_same_v<Type, glm::mat3x2>)
			{
				for (size_t i = 0; i < Type::length(); ++i)
					Set<glm::vec2>(value[i], typeInfo[i].Offset);
			}
			else if constexpr(std::is_same_v<Type, glm::mat4x3> || std::is_same_v<Type, glm::mat3x4>)
			{
				for (size_t i = 0; i < Type::length(); ++i)
					Set<glm::vec4>(value[i], typeInfo[i].Offset);
			}
			else if constexpr(std::is_same_v<Type, glm::mat2>)
			{
				for (size_t i = 0; i < Type::length(); ++i)
					Set<glm::vec2>(value[i], typeInfo[i].Offset);
			}
			else if constexpr(std::is_same_v<Type, glm::mat2x3>)
			{
				for (size_t i = 0; i < Type::length(); ++i)
					Set<glm::vec4>(value[i], typeInfo[i].Offset);
			}
			else if constexpr(std::is_same_v<Type, glm::mat2x4>)
			{
				for (size_t i = 0; i < Type::length(); ++i)
					Set<glm::vec4>(value[i], typeInfo[i].Offset);
			}
			else
				Set<Type>(value, typeInfo[0].Offset);
			
			MarkAsChanged();
		}

		template <typename Type>
		Type Get(size_t index) const
		{
			ASSERT(index < m_Size, "Out of range");
			if(index >= m_Size) throw std::out_of_range("Out of range");

			const auto &[name, typeInfo] = m_Types[index];
			ASSERT(typeid(Type).name() == name, "Not same type as registered");
			if(typeid(Type).name() != name) throw std::runtime_error("Not same type as registered");

			if constexpr(std::is_same_v<Type, glm::mat4> || std::is_same_v<Type, glm::mat3>)
			{
				Type mat;
				for (size_t i = 0; i < Type::length(); ++i)
					mat[i] = GetValue<glm::vec4>(typeInfo[i].Offset);
				return mat;
			}
			else if constexpr(std::is_same_v<Type, glm::mat4x2> || std::is_same_v<Type, glm::mat3x2>)
			{
				Type mat;
				for (size_t i = 0; i < Type::length(); ++i)
					mat[i] = GetValue<glm::vec2>(typeInfo[i].Offset);
				return mat;
			}
			else if constexpr(std::is_same_v<Type, glm::mat4x3> || std::is_same_v<Type, glm::mat3x4>)
			{
				Type mat;
				for (size_t i = 0; i < Type::length(); ++i)
					mat[i] = GetValue<glm::vec4>(typeInfo[i].Offset);
				return mat;
			}
			else if constexpr(std::is_same_v<Type, glm::mat2>)
			{
				Type mat;
				for (size_t i = 0; i < Type::length(); ++i)
					mat[i] = GetValue<glm::vec2>(typeInfo[i].Offset);
				return mat;
			}
			else if constexpr(std::is_same_v<Type, glm::mat2x3>)
			{
				Type mat;
				for (size_t i = 0; i < Type::length(); ++i)
					mat[i] = GetValue<glm::vec4>(typeInfo[i].Offset);
				return mat;
			}
			else if constexpr(std::is_same_v<Type, glm::mat2x4>)
			{
				Type mat;
				for (size_t i = 0; i < Type::length(); ++i)
					mat[i] = GetValue<glm::vec4>(typeInfo[i].Offset);
				return mat;
			}
			else
				return GetValue<Type>(typeInfo[0].Offset);
		}

	protected:
		void SendValues() const override
		{
			auto data = GetContent(BufferAccess::WriteOnly);
			data->Set(m_Storage, m_Size);
		}

	private:
		template <typename T>
		T GetValue(const size_t offset) const
		{
			T value;
			std::memcpy(&value, m_Storage + offset, sizeof(T));

			return value;
		}

		template <typename T>
		void Set(const T& value, const size_t offset)
		{
			std::memcpy(m_Storage + offset, &value, sizeof(T));
		}
		
		template <typename T>
		static constexpr size_t GetSize()
		{
			if constexpr(std::is_same_v<T, char> || std::is_same_v<T, short> || std::is_same_v<T, float> ||
				std::is_same_v<T, int> || std::is_same_v<T, uint32_t> || std::is_same_v<T, uint16_t> || std::is_same_v<
					T, bool>) return sizeof(float);
			if constexpr(std::is_same_v<T, glm::vec2> || std::is_same_v<T, Vector2f>) return sizeof(float) * 2;
			if constexpr(std::is_same_v<T, glm::vec3>) return sizeof(float) * 4;
			if constexpr(std::is_same_v<T, glm::vec4> || std::is_same_v<T, Color>) return sizeof(float) * 4;

			if constexpr(std::is_same_v<T, glm::mat2>) return sizeof(float) * 4;
			if constexpr(std::is_same_v<T, glm::mat2x3>) return sizeof(float) * 8;
			if constexpr(std::is_same_v<T, glm::mat2x4>) return sizeof(float) * 8;

			if constexpr(std::is_same_v<T, glm::mat3>) return sizeof(float) * 16;
			if constexpr(std::is_same_v<T, glm::mat3x2>) return sizeof(float) * 8;
			if constexpr(std::is_same_v<T, glm::mat3x4>) return sizeof(float) * 16;

			if constexpr(std::is_same_v<T, glm::mat4>) return sizeof(float) * 16;
			if constexpr(std::is_same_v<T, glm::mat4x2>) return sizeof(float) * 8;
			if constexpr(std::is_same_v<T, glm::mat4x3>) return sizeof(float) * 16;

			ASSERT(false, "Unknown Type");
			throw std::runtime_error("Unknown type");
			return 0;
		}

		template <typename Type, typename ... Types>
		constexpr void RegisterType(std::vector<TypeInfo>& types)
		{
			const TypeInfo type = {m_Offset, GetSize<Type>()};

			types.emplace_back(type);
			m_Offset += type.Size;
			m_Size += type.Size;

			LOG_INFO("{}: {}, Size = {}, Offset = {}", m_Types.size() - 1, typeid(Type).name(), type.Size, type.Offset);

			if constexpr(TypeListLengthV<Types...> > 0) RegisterType<Types...>(types);
		}

		template <typename Type, typename ... Types>
		void RegisterTypes(size_t &index)
		{
			auto& type = m_Types[index++];
			type.first = typeid(Type).name();
			
			if constexpr(std::is_same_v<Type, glm::mat4> || std::is_same_v<Type, glm::mat3>)
				RegisterType<glm::vec4, glm::vec4, glm::vec4, glm::vec4>(type.second);
			else if constexpr(std::is_same_v<Type, glm::mat4x2> || std::is_same_v<Type, glm::mat3x2>)
				RegisterType<glm::vec2, glm::vec2, glm::vec2, glm::vec2>(type.second);
			else if constexpr(std::is_same_v<Type, glm::mat4x3> || std::is_same_v<Type, glm::mat3x4>)
				RegisterType<glm::vec4, glm::vec4, glm::vec4, glm::vec4>(type.second);
			else if constexpr(std::is_same_v<Type, glm::mat2>)
				RegisterType<glm::vec2, glm::vec2>(type.second);
			else if constexpr(std::is_same_v<Type, glm::mat2x3>)
				RegisterType<glm::vec4, glm::vec4>(type.second);
			else if constexpr(std::is_same_v<Type, glm::mat2x4>)
				RegisterType<glm::vec4, glm::vec4>(type.second);
			else RegisterType<Type>(type.second);
			if constexpr(TypeListLengthV<Types...> > 0) RegisterTypes<Types...>(index);
		}
	};
}
