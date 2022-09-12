#pragma once
#include "Engine/Core/Base.h"

namespace Game
{
	class ShaderSource
	{
		std::string m_Source;

	public:
		ShaderSource() = default;
		ShaderSource(const std::string &source);

		static ShaderSource Load(const std::filesystem::path path);

		const std::string& Source() const { return m_Source; }
		void Source(const std::string& source);
	};
}
