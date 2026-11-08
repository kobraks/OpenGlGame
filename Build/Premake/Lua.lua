local LuaRoot = path.join(_MAIN_SCRIPT_DIR, "vendor/lua")

project "Lua55"
    kind "StaticLib"
    language "C"
    staticruntime "off"

    location (path.join(_MAIN_SCRIPT_DIR, "Build/Projects/Lua"))
    
    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
		path.join(LuaRoot, "src/**.h"),
		path.join(LuaRoot, "src/**.c")
    }

    removefiles{ path.join(LuaRoot, "src/luac.c"), path.join(LuaRoot, "src/lua.c") }
	
	defines
	{
		"LUA_BUILD_AS_DLL"
	}

    includedirs
    {
        path.join(LuaRoot, "src")
    }
    
    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
