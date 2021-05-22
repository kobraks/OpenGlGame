#include "pch.h"
#include "LuaSerializer.h"
#include "Utils.h"

namespace Game
{
	void LuaSerializer::Open(const std::string &fileName, bool trunc)
	{
		if(IsOpen())
			Close();

		m_FileName = fileName;

		if(trunc)
			m_File.open(fileName, std::fstream::out | std::fstream::trunc);
		else
			m_File.open(fileName, std::ios::out);
	}

	std::string LuaSerializer::FileName() const
	{
		return m_FileName;
	}

	bool LuaSerializer::IsOpen() const
	{
		return m_File.is_open();
	}

	bool LuaSerializer::Good() const
	{
		return m_File.is_open() && m_File.good();
	}

	bool LuaSerializer::Fail() const
	{
		return m_File.fail();
	}

	bool LuaSerializer::Bad() const
	{
		return m_File.bad();
	}

	LuaSerializer& LuaSerializer::BeginTable(std::string_view name)
	{
		if(!IsOpen())
			throw std::runtime_error("Trying to write to unopen file");

		if(!name.empty())
			m_File << m_Begin << name << " =\n";
		m_File << m_Begin << "{";
		m_Depth++;
		m_Begin = std::string(m_Depth, '\t');


		return *this;
	}

	LuaSerializer& LuaSerializer::EndTable()
	{
		if(!IsOpen())
			throw std::runtime_error("Trying to write to unopen file");

		m_Depth--;
		m_Begin = std::string(m_Depth, '\t');

		m_File << "\n" << m_Begin << "};";
		return *this;
	}

	std::string& LuaSerializer::Trim(std::string &string) const
	{
		return Game::Trim(string);
	}

	LuaSerializer& LuaSerializer::String(std::string_view name, std::string_view string)
	{
		if(!IsOpen())
			throw std::runtime_error("Trying to write to unopen file");

		m_File << m_Begin << name << " = \"" << string << "\";\n";

		return *this;
	}

	LuaSerializer& LuaSerializer::Color(std::string_view name, const glm::vec3 &color)
	{
		if(!IsOpen())
			throw std::runtime_error("Trying to write to unopen file");

		return BeginTable(name).Value("Red", color.r).Value("Green", color.g).Value("Blue", color.b).EndTable();
	}

	LuaSerializer& LuaSerializer::Color(std::string_view name, const glm::vec4 &color)
	{
		if(!IsOpen())
			throw std::runtime_error("Trying to write to unopen file");

		return BeginTable(name).Value("Red", color.r).Value("Green", color.g).Value("Blue", color.b).Value("Alpha", color.a).EndTable();
	}

	LuaSerializer& LuaSerializer::Size(std::string_view name, const glm::vec2 &size)
	{
		if(!IsOpen())
			throw std::runtime_error("Trying to write to unopen file");

		BeginTable(name).Value("Width", size.y).Value("Height", size.x).EndTable();
		return *this;
	}

	void LuaSerializer::Close()
	{
		m_File.close();
		m_Depth    = 0;
		m_Begin    = {};
		m_FileName = {};
	}

	void LuaSerializer::Flush()
	{
		m_File.flush();
	}
}
