project "Lua54"
    kind "StaticLib"
    language "C"
    staticruntime "off"
    
    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
		"src/**.h",
		"src/**.c"
    }

    removefiles{ "src/luac.c", "src/lua.c" }
	
	defines
	{
		"LUA_BUILD_AS_DLL"
	}

    includedirs
    {
        "src"
    }
    
    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
