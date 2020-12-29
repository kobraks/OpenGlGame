#pragma once
#include <sol/state.hpp>

namespace Game
{
	class LuaRegister
	{
	public:
		virtual ~LuaRegister() = default;

		virtual void Register(sol::state& state) = 0;
	};
}
