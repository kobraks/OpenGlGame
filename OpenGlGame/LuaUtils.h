#pragma once
#include <sol/as_args.hpp>
#include <sol/variadic_args.hpp>
#include <sol/table.hpp>
#include <sol/metatable.hpp>


#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Game
{
	std::string ToString(const sol::state_view &lua, const sol::object &object);
	std::string ToString(const sol::state_view &lua, const sol::table &table);
	std::string ToString(const sol::state_view &lua, sol::variadic_args::const_iterator begin, const sol::variadic_args::const_iterator &end);
	std::string ToString(const sol::state_view &lua, const sol::variadic_args &args);

	bool DoFile(sol::state &state, const std::string &fileName);
	bool DoFile(sol::state &state, const std::string &fileName, sol::environment &env);

	bool DoString(sol::state &state, const std::string &string);
	bool DoString(sol::state &state, const std::string &string, sol::environment &env);


	//Getting string from stack at given index
	std::string LuaGetString(lua_State *L, int index);

	glm::vec2 ReadVector2(const sol::table &vector);
	glm::vec3 ReadVector3(const sol::table &vector);
	glm::vec4 ReadVector4(const sol::table &vector);

	inline int Deny(lua_State *L)
	{
		return luaL_error(L, "Access denied");
	}

	template <typename Table, typename Table2, typename DenyFunction>
	void SetAsReadOnlyTable(Table &&table, Table2 &&metaTable, DenyFunction denyFunction)
	{
		metaTable[sol::meta_function::new_index] = denyFunction;
		metaTable[sol::meta_function::index]     = metaTable;

		table[sol::metatable_key] = metaTable;
	}
}
