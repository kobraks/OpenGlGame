project "Lua54"
    kind "SharedLib"
    language "C"
    staticruntime "off"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
		"src/**.h",
		"src/**.c"
    }
	
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
