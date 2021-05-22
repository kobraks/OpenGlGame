#pragma once
#include "Vector2.h"

namespace Game
{
	class LuaSerializer
	{
	public:
		/*struct Color
		{
			std::variant<glm::vec3, glm::vec4> C;

			Color(const glm::vec3 &color)
			{
				C = color;
			}

			Color(const glm::vec4 &color)
			{
				C = color;
			}
		};*/

	private:
		std::ofstream m_File;
		std::string m_FileName;
		uint64_t m_Depth    = 0;
		std::string m_Begin = {};

	public:
		LuaSerializer(const std::string &fileName, bool trunc = false) { }

		LuaSerializer() = default;

		~LuaSerializer()
		{
			Close();
		}

		void Open(const std::string &fileName, bool trunc = false);

		std::string FileName() const;

		bool IsOpen() const;
		bool Good() const;
		bool Fail() const;
		bool Bad() const;

		void Close();
		void Flush();

		LuaSerializer& BeginTable(std::string_view name = "");
		LuaSerializer& EndTable();

	private:
		std::string& Trim(std::string &string) const;

	public:
		LuaSerializer& String(std::string_view name, std::string_view string);
		LuaSerializer& Color(std::string_view name, const glm::vec3 &color);
		LuaSerializer& Color(std::string_view name, const glm::vec4 &color);

		LuaSerializer& Size(std::string_view name, const glm::vec2 &size);

		template<typename Type>
		LuaSerializer& Size(std::string_view name, const Vector2<Type>& size)
		{
			if (!IsOpen())
				throw std::runtime_error("Trying to write to unopen file");

			BeginTable(name).Value("Width", size.Width).Value("Height", size.Height).EndTable();
			return *this;
		}

		template <class T>
		LuaSerializer& Value(std::string_view name, const T &value)
		{
			if(!IsOpen())
				throw std::runtime_error("Trying to write to unopen file");

			m_File << m_Begin << name << " = " << value << ";\n";
			return *this;
		}

		template <>
		LuaSerializer& Value(std::string_view name, const bool &value)
		{
			return Value<std::string>(name, value ? "true" : "false");
		}

		template <>
		LuaSerializer& Value(std::string_view name, const glm::vec3 &value)
		{
			return BeginTable(name).Value("x", value.x).Value("y", value.y).Value("z", value.z).EndTable();
		}

		template <>
		LuaSerializer& Value(std::string_view name, const glm::vec4 &value)
		{
			return BeginTable(name).Value("x", value.x).Value("y", value.y).Value("z", value.z).Value("w", value.w).EndTable();
		}

		/*template <>
		LuaSerializer& Value(std::string_view name, const Color &value)
		{
			const auto color = value.C;

			if(std::holds_alternative<glm::vec3>(color))
			{
				const auto v = std::get<glm::vec3>(color);
				BeginTable(name).Value<int32_t>("Red", v.r * 255).Value<int32_t>("Green", v.g * 255).Value<int32_t>("Blue", v.b * 255).EndTable();
			}
			else
			{
				const auto v = std::get<glm::vec4>(color);
				BeginTable(name).Value<int32_t>("Red", v.r * 255).Value<int32_t>("Green", v.g * 255).Value<int32_t>("Blue", v.b * 255).Value<
					int32_t>("Alpha", v.a * 255).EndTable();
			}

			return *this;
		}*/
	};
}
