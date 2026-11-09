project "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++23"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "src/pch.cpp"
	
	files
	{
		"src/**.h",
		"src/**.cpp",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE",
		"SPDLOG_FMT_EXTERNAL",
        "SPDLOG_COMPILED_LIB",
		"YAML_CPP_STATIC_DEFINE",
		"FREEIMAGE_LIB"
	}

	includedirs
	{
		"src",
		"%{wks.location}/vendor",
		"%{IncludeDir.Boost}",
		"%{IncludeDir.Spdlog}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.Glm}",
		"%{IncludeDir.Lua}",
		"%{IncludeDir.Sol2}",
		"%{IncludeDir.Fmt}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.FreeImage}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.Entt}",
		"%{IncludeDir.Yaml}",
		"%{IncludeDir.Assimp}",
		"%{IncludeDir.AssimpGen}",
	}

	links
	{
		"Glad",
		"GLFW",
		"Lua54",
		"fmt",
		"ImGui",
		"Spdlog",
		"FreeImageLib",
		"yaml-cpp",
		"assimp",
		"ImGuizmo",
		
		"opengl32.lib",
	}
	
	filter "action:vs*"
		externalanglebrackets "On" --Enable treating <> as external
		externalwarnings "Off" --Disable external warnings /external:W0
		warnings "Extra"

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "GAME_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "GAME_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "GAME_DIST"
		runtime "Release"
		optimize "on"
