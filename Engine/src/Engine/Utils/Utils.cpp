#include "pch.h"
#include "Utils.h"

#include <algorithm>

namespace Game
{
	std::string& TrimLeft(std::string &string)
	{
		string.erase(
		             string.begin(),
		             std::find_if(
		                          string.begin(),
		                          string.end(),
		                          [](char c)
		                          {
			                          return !std::isspace(c);
		                          }
		                         )
		            );

		return string;
	}

	std::string& TrimRight(std::string &string)
	{
		string.erase(
		             std::find_if(
		                          string.rbegin(),
		                          string.rend(),
		                          [](char c)
		                          {
			                          return !std::isspace(c);
		                          }
		                         ).base(),
		             string.end()
		            );

		return string;
	}

	std::string& Trim(std::string &string)
	{
		return TrimLeft(TrimRight(string));
	}

	std::string TrimLeftCopy(std::string string)
	{
		return TrimLeft(string);
	}

	std::string TrimRightCopy(std::string string)
	{
		return TrimRight(string);
	}

	std::string TrimCopy(std::string string)
	{
		return Trim(string);
	}

	std::string& ToLower(std::string &string)
	{
		std::transform(string.begin(), string.end(), string.begin(), tolower);
		return string;
	}

	std::string& ToUpper(std::string &string)
	{
		std::transform(string.begin(), string.end(), string.begin(), toupper);
		return string;
	}

	std::string ToUpperCopy(std::string string)
	{
		return ToUpper(string);
	}

	std::string ToLowerCopy(std::string string)
	{
		return ToLower(string);
	}
}
