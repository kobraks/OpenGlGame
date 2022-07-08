#include "pch.h"
#include "Component.h"

#include "Assert.h"

#include "Defines.h"
#include "ModelLoader.h"
#include "Model.h"
#include "Renderer.h"
#include "Application.h"
#include "FileWatcher.h"
#include "LuaUtils.h"

#include <sol/sol.hpp>

#include "Entity.h"

namespace Game
{
	static int LuaRegisterValue(lua_State *L)
	{
		//Stack:
		//1 - Global table 
		//2 - New key
		//3 - Value
		
		//Checking for correct arguments
		ASSERT(lua_gettop(L) == 3, fmt::format("Not enough arguments expected: \"{}\" got: \"{}\"", 3, lua_gettop(L)));
		luaL_checkany(L, 1);
		luaL_checkany(L, 2);
		luaL_checkany(L, 3);

		//Geting __Object value from metatable
		
		//Geting metatable of current table
		lua_getmetatable(L, 1);
		lua_pushstring(L, "__Object");
		lua_gettable(L, -2);

		//Converting __Object to LuaScriptComponent
		LuaScriptComponent *data = static_cast<LuaScriptComponent *>(lua_touserdata(L, -1));
		//Registering new user value
		data->UserValues.emplace_back(LuaGetString(L, 2));

		//Seting table
		lua_rawset(L, 1);

		return 0;
	}
	
	static void DisableFunctions(sol::environment& env)
	{
		//Source http://lua-users.org/wiki/SandBoxes

		//TODO MY OWN DEBUG FUNCTIONALITY
		env["debug"] = sol::nil;
		
		env["os"] = sol::nil;
		env["io"] = sol::nil;
		env["string.dump"] = sol::nil;
		env["package"] = sol::nil;
		env["module"] = sol::nil;
		env["setmetatable"] = sol::nil;
		env["setfenv"] = sol::nil;
		
		env["collectgarbage"] = sol::nil;
		env["dofile"] = sol::nil;
		env["getfenv"] = sol::nil;
		env["getmetatable"] = sol::nil;
		env["load"] = sol::nil;
		env["loadfile"] = sol::nil;
		env["loadstring"] = sol::nil;

		env["print"] = env["Log"]["Info"];

		env["rawequal"] = sol::nil;
		env["rawget"] = sol::nil;
		env["rawset"] = sol::nil;
	}

	Component::Component(std::string name, Entity *entity) : m_Name(std::move(name))
	{
		m_Scene = entity->m_Scene;
		m_EntityId = entity->m_EntityHandle;
	}

	Entity Component::GetEntity() const
	{
		return Entity{m_EntityId, m_Scene};
	}

	void LuaScriptComponent::OpenFile(const std::string &fileName)
	{
		ScriptPath = fileName;
		LOG_INFO("Opening \"{}\"", fileName);

		const std::string path = SCRIPTS_PATH "/" + fileName;
		
		try
		{
			sol::state &state = Application::Get().GetLua();
			Environment = sol::environment(state, sol::create, state.globals());
			DisableFunctions(Environment);
			Environment[sol::metatable_key]["__Object"] = static_cast<void*>(this);
			Environment[sol::metatable_key][sol::meta_function::new_index] = LuaRegisterValue;

			DoFile(state, path, Environment);
			
			FileWatcher::Watch(
			                     path,
			                     [&](std::string file, FileStatus status)
			                     {
				                     if(status == FileStatus::Modified)
				                     {
					                     LOG_DEBUG("Script \"{}\" got modified", file);
					                     OpenFile(ScriptPath);
				                     }
			                     }
			                    );
		}
		catch(std::exception &ex)
		{
			LOG_ERROR("Exception thrown: {}", ex.what());
		}
	}

	void ModelComponent::LoadModel(const std::string &fileName)
	{
		ModelPath = fileName;
		Model     = ModelLoader::Load(fileName, MODELS_PATH);
	}

	void ModelComponent::Draw(const glm::mat4 &transform) const
	{
		Renderer::Draw(Model, transform);
	}
}
