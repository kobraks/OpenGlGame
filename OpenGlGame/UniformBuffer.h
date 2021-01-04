#pragma once

#include <array>
#include <typeindex>


#include "BufferObject.h"
#include "Color.h"

#include "OpenGlFunctions.h"
#include "Log.h"

namespace Game
{
	template <typename ...Types>
	struct TypeListLength
	{
		constexpr static size_t Length = sizeof...(Types);
	};

	template <typename ...Types>
	using TypeListLengthV = typename TypeListLength<Types...>::Length;

	template <typename T, typename ...Ts>
	class UniformBuffer: public BufferObject
	{
		struct TypeInfo
		{
			size_t Offset = 0;
			size_t Size   = 0;
			std::string Name;
		};

		char *m_Storage;

		std::array<TypeInfo, sizeof...(Ts) + 1> m_Types;
		size_t m_Offset = 0;
		size_t m_Size   = 0;
	public:
		UniformBuffer() : BufferObject(BufferType::Uniform)
		{
			size_t index = 0;
			RegisterType<T, Ts...>(index, m_Size);

			m_Storage = new char[m_Size];
			Allocate(m_Size);
		}

		template <typename Type>
		void Set(size_t index, const Type &value)
		{
			ASSERT(index < m_Size, "Out of range");
			if(index >= m_Size) throw std::out_of_range("Out of range");

			const auto &[offset, size, name] = m_Types[index];
			ASSERT(typeid(Type).name() == name, "Not same type as registered");
			if(typeid(Type).name() != name) throw std::runtime_error("Not same type as registered");

			std::memcpy(m_Storage + offset, &value, size);

			MarkAsChanged();
		}

		template <typename Type>
		Type Get(size_t index) const
		{
			ASSERT(index < m_Size, "Out of range");
			if(index >= m_Size) throw std::out_of_range("Out of range");

			const auto &[offset, size, name] = m_Types[index];
			ASSERT(typeid(Type).name() == name, "Not same type as registered");
			if(typeid(Type).name() != name) throw std::runtime_error("Not same type as registered");

			Type value;
			std::memcpy(&value, m_Storage, size);
			return value;
		}

	protected:
		void SendValues() const override
		{
			auto data = GetContent(BufferAccess::WriteOnly);
			data->Set(m_Storage, m_Size);
		}

	private:
		template <typename T>
		static constexpr size_t GetSize()
		{
			if constexpr(std::is_same_v<T, char> || std::is_same_v<T, short> || std::is_same_v<T, float> ||
				std::is_same_v<T, int> || std::is_same_v<T, uint32_t> || std::is_same_v<T, uint16_t> || std::is_same_v<
					T, bool>)
				return sizeof(float);
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
		void RegisterType(size_t &index, size_t &size)
		{
			TypeInfo type{m_Offset, GetSize<Type>(), typeid(Type).name()};

			LOG_INFO("{}: {}, Size = {}, Offset = {}", index, type.Name, type.Size, type.Offset);
			m_Types[index++] = type;
			m_Offset += type.Size;
			m_Size += type.Size;

			if constexpr(sizeof...(Types) > 0) RegisterType<Types...>(index, size);
		}
	};
}
