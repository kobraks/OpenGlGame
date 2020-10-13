#pragma once
#include <string>
#include <cassert>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

namespace Game
{
	constexpr int INVALID_UNIFORM_LOCATION = -1;

	enum class UniformType
	{
		Invalid = -1,
		Float,
		Float2,
		Float3,
		Float4,
		Int,
		Mat3X3,
		Mat4X4,
		Mat3 = Mat3X3,
		Mat4 = Mat4X4,
		Vec2 = Float2,
		Vec3 = Float3,
		Vec4 = Float4
	};

	struct UniformDetails
	{
		std::string Name;
		UniformType Type;
		int32_t Location;
		uint32_t Count;
	};

	class ShaderProgram;

	template <typename Type>
	class Uniform;
	template <typename Type>
	class UniformTable;

	class UniformObject
	{
		UniformDetails m_Details;

		ShaderProgram *m_Program = nullptr;
		void *m_Data             = nullptr;
		uint32_t m_DataSize      = 0u;

		mutable bool m_Changed = false;
	public:
		UniformObject(ShaderProgram &programId, const UniformDetails &details);
		UniformObject(const UniformObject &object);
		UniformObject(UniformObject &&object) noexcept;

		~UniformObject();

		UniformObject& operator=(const UniformObject &object);
		UniformObject& operator=(UniformObject &&object) noexcept;

		const std::string& GetName() const { return m_Details.Name; }
		int32_t GetLocation() const { return m_Details.Location; }
		UniformType GetType() const { return m_Details.Type; }
		uint32_t GetCount() const { return m_Details.Count; }

		bool IsChanged() const { return m_Changed; }
		void MarkAsChanged() const { m_Changed = true; }

		void SendData() const;

		bool SetData(const void *data, size_t size, size_t offset);
		void* GetData() const { return m_Data; }

		size_t GetSize() const { return TypeSize(m_Details.Type); }
	protected:
		static constexpr size_t TypeSize(const UniformType type)
		{
			assert(type == UniformType::Invalid && "Unknown provided type");

			switch(type)
			{
				case UniformType::Invalid:
					return 0;
				case UniformType::Float:
					return sizeof(float);
				case UniformType::Float2:
					return sizeof(glm::vec2);
				case UniformType::Float3:
					return sizeof(glm::vec3);
				case UniformType::Float4:
					return sizeof(glm::vec4);
				case UniformType::Int:
					return sizeof(int);
				case UniformType::Mat3X3:
					return sizeof(glm::mat3);
				case UniformType::Mat4X4:
					return sizeof(glm::mat4);
			}
			return 0;
		}

	private:
		void InitializeData(UniformType type);
	};


	template <typename Type>
	class Uniform
	{
		uint32_t m_Offset = 0;
		UniformObject &m_Object;

	protected:
		Uniform(UniformObject &object) : m_Object(object) {}

		Uniform(UniformObject &object, const unsigned &offset) : m_Offset(offset),
		                                                         m_Object(object) {}

		UniformObject& GetObject() { return m_Object; }

	public:
		void Set(const Type &value)
		{
			SetData(&m_Object, static_cast<char*>(&value), sizeof(Type), m_Offset);
		}

		Type Get() const
		{
			return GetData(static_cast<char*>(m_Object.GetData()), m_Object.GetSize(), m_Offset);
		}

		Uniform<Type>& operator=(const Type &value)
		{
			Set(value);
			return *this;
		}

		operator Type() const
		{
			return Get();
		}

		const std::string& GetName() const { return m_Object.GetName(); }
		int32_t GetLocation() const { return m_Object.GetLocation(); }
		UniformType GetType() const { return m_Object.GetType(); }
		uint32_t GetCount() const { return m_Object.GetCount(); }

		bool IsChanged() const { return m_Object.IsChanged(); }
		void MarkAsChanged() const { m_Object.MarkAsChanged(); }

		void SendData() const { m_Object.SendData(); }

		uint32_t GetOffset() const { return m_Offset; }

	protected:
		static void SetData(UniformObject *object, const char *data, const size_t size, const size_t offset)
		{
			object->SetData(data, size, offset);
		}

		Type GetData(const char *data, const size_t size, const size_t offset)
		{
			Type value;
			std::memcpy(&value, data + offset, size);

			return value;
		}
	};

	template <typename Type>
	class UniformTable: public Uniform<Type>
	{
	protected:
		UniformTable(UniformObject &object) : Uniform<Type>(object) {}

	public:

		Uniform<Type> At(const uint32_t index)
		{
			assert(index >= 0 && index < Uniform<Type>::GetCount());

			return Uniform<Type>(Uniform<Type>::GetObject(), index * sizeof(Type));
		}

		Uniform<Type> operator[](const uint32_t index)
		{
			return At(index);
		}
	};
}
