#include "pch.h"
#include "Engine/Utils/LuaSerializer.h"
#include "Engine/Utils/Utils.h"

#define SANITY_CHECK()\
	if (!IsOpen())\
		throw std::runtime_error("Trying to access not opened file");\
	if (Bad() || Fail())\
		throw std::runtime_error(fmt::format("Unable to write to file, '{}'", m_FileName));

namespace Game
{
	LuaSerializer::LuaSerializer(const std::string &fileName, bool trunc)
	{
		Open(fileName, trunc);
	}

	LuaSerializer::~LuaSerializer()
	{
		Close();
	}

	bool LuaSerializer::Open(const std::string &fileName, bool trunc)
	{
		if (IsOpen())
			Close();

		m_FileName = fileName;

		if (trunc)
			m_File.open(fileName, std::fstream::out | std::fstream::trunc);
		else
			m_File.open(fileName, std::fstream::out);

		if (Fail() || Bad())
			return false;

		return IsOpen() ? true : false;
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
		return m_File.good();
	}

	bool LuaSerializer::Fail() const
	{
		return m_File.fail();
	}

	bool LuaSerializer::Bad() const
	{
		return m_File.bad();
	}

	void LuaSerializer::Close()
	{
		while(m_Depth != 0)
		{
			EndTable();
		}

		Flush();
		m_File.close();

		m_Depth = 0;
		m_Begin = {};
		m_FileName = {};
	}

	void LuaSerializer::Flush()
	{
		m_File.flush();
	}

	LuaSerializer& LuaSerializer::BeginTable(std::string_view name)
	{
		SANITY_CHECK()

		if (!name.empty())
			PrivValueName(name);

		m_File << m_Begin << "{ \n";
		m_Depth++;
		m_Begin = std::string(m_Depth, '\t');

		return *this;
	}

	LuaSerializer& LuaSerializer::EndTable()
	{
		SANITY_CHECK()

		ASSERT(m_Depth != 0);

		if (m_Depth == 0)
			throw std::runtime_error("No table has been opened");

		m_Depth--;
		m_Begin = std::string(m_Depth, '\t');

		m_File << "\n" << m_Begin << "};";

		return *this;
	}

	LuaSerializer& LuaSerializer::String(std::string_view name, std::string_view string)
	{
		return PrivValueName(name).PrivValue('\"', string, '\"');
	}

	LuaSerializer& LuaSerializer::Color(std::string_view name, const glm::vec3 &color)
	{
		BeginTable(name);
		Value("Red", color.r);
		Value("Green", color.g);
		Value("Blue", color.b);
		return EndTable();
	}

	LuaSerializer& LuaSerializer::Color(std::string_view name, const glm::vec4 &color)
	{
		BeginTable(name);
		Value("Red", color.r);
		Value("Green", color.g);
		Value("Blue", color.b);
		Value("Alpha", color.a);
		return EndTable();
	}

	LuaSerializer& LuaSerializer::Color(std::string_view name, const Game::Color &color)
	{
		BeginTable(name);
		Value("Red", color.R);
		Value("Green", color.G);
		Value("Blue", color.B);
		Value("Alpha", color.A);
		return EndTable();
		
	}

	LuaSerializer & LuaSerializer::Vector(std::string_view name, const glm::vec3 &vec)
	{
		BeginTable(name);
		Value("x", vec.x);
		Value("y", vec.y);
		Value("z", vec.z);
		return EndTable();
	}
	
	LuaSerializer & LuaSerializer::Vector(std::string_view name, const glm::vec4 &vec)
	{
		BeginTable(name);
		Value("x", vec.x);
		Value("y", vec.y);
		Value("z", vec.z);
		Value("w", vec.w);
		return EndTable();
	}

	LuaSerializer& LuaSerializer::Size(std::string_view name, const glm::vec2 &size)
	{
		BeginTable(name);
		Value("Width", size.x);
		Value("Height", size.y);
		return EndTable();
	}

	LuaSerializer & LuaSerializer::PrivValueName(std::string_view name)
	{
		SANITY_CHECK()
			
		m_File << m_Begin;
		if (!name.empty())
		{
			m_File << name << " = ";
		}
		return *this;
	}

	LuaSerializer& LuaSerializer::PrivStrValue(std::string_view name, std::string_view value)
	{
		return PrivValueName(name).PrivValue(value);
	}
}
