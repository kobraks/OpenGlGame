#pragma once

#include "Engine/Core/Vector2.h"
#include "Engine/Core/Color.h"
#include "Engine/Core/UUID.h"

#include <fstream>

namespace Game
{
	class LuaSerializer
	{
	public:
	private:
		std::ofstream m_File;
		std::string m_FileName = {};

		uint64_t m_Depth = 0;
		std::string m_Begin = {};

	public:
		LuaSerializer() = default;
		LuaSerializer(const std::string &fileName, bool trunc = false);

		~LuaSerializer();

		bool Open(const std::string &fileName, bool trunc = false);

		std::string FileName() const;

		bool IsOpen() const;
		bool Good() const;
		bool Fail() const;
		bool Bad() const;

		void Close();
		void Flush();

		LuaSerializer& BeginTable(std::string_view name = "");
		LuaSerializer& EndTable();

		LuaSerializer& String(std::string_view name, std::string_view string);

		LuaSerializer& Color(std::string_view name, const glm::vec3 &color);
		LuaSerializer& Color(std::string_view name, const glm::vec4 &color);
		LuaSerializer& Color(std::string_view name, const Game::Color &color);

		LuaSerializer& Vector(std::string_view name, const glm::vec3 &vec);
		LuaSerializer& Vector(std::string_view name, const glm::vec4 &vec);

		LuaSerializer& Size(std::string_view name, const glm::vec2 &size);

	private:
		LuaSerializer& PrivValueName(std::string_view name);
		LuaSerializer& PrivStrValue(std::string_view name, std::string_view value);

	public:

		template<typename T>
		LuaSerializer& CustomValue(std::string_view name, const T& value)
		{
			return PrivValueName(name).PrivValue(value);
		}

		template<typename T, typename... Args>
		LuaSerializer& CustomValue(std::string_view name, const T& value, Args&&... args)
		{
			return PrivValueName(name).PrivValue(value, std::forward<Args>(args)...);
		}

		template<typename T>
		LuaSerializer& Size(std::string_view name, const Vector2<T>& size)
		{
			return BeginTable(name).Value("With", size.Width).Value("Height", size.Height).EndTable();
		}

		template <typename T>
		LuaSerializer& Value(std::string_view name, const T &value)
		{
			return PrivValueName(name).PrivValue(value);
		}

		template <>
		LuaSerializer& Value(std::string_view name, const bool& value)
		{
			return PrivValueName(name).PrivValue(value ? "true" : "false");
		}

		template<>
		LuaSerializer& Value(std::string_view name, const glm::vec3 &value)
		{
			return Vector(name, value);
		}

		template<>
		LuaSerializer& Value(std::string_view name, const glm::vec4 &value)
		{
			return Vector(name, value);
		}

		template<>
		LuaSerializer& Value(std::string_view name, const Game::Color& color)
		{
			return Color(name, color);
		}

		template<>
		LuaSerializer& Value(std::string_view name, const std::string_view &str)
		{
			return String(name, str);
		}

		template<>
		LuaSerializer& Value(std::string_view name, const std::string &str)
		{
			return String(name, str);
		}

	private:
		template <typename T, typename... Args>
		LuaSerializer& PrivValue(const T& value, Args&&... args)
		{
			m_File << value;
			return PrivValue(value, std::forward<Args>(args)...);
		}

		template<typename T>
		LuaSerializer& PrivValue(const T& value)
		{
			m_File << value << ";\n";
			return *this;
		}
	};
}
