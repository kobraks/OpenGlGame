project "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "src/pch.cpp"

	files
	{
		"src/**.h",
		"src/**.cpp",
		
		"%{IncludeDir.glm}/glm/**.hpp",
		"%{IncludeDir.glm}/glm/**.inl",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

	includedirs
	{
		"src",
		"%{IncludeDir.Spdlog}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.Glm}",
		"%{IncludeDir.Lua}",
		"%{IncludeDir.Sol2}",
		"%{IncludeDir.Fmt}",
	}

	links
	{
		"Glad",
		"Lua54",
		"Fmt",
		
		"opengl32.lib",
	}

	--filter "files:vendor/ImGuizmo/**.cpp"
	--flags { "NoPCH" }

	filter "system:windows"
		systemversion "latest"

		defines
		{
		}

		links
		{
		}

	filter "configurations:Debug"
		defines "GAME_DEBUG"
		runtime "Debug"
		symbols "on"

		links
		{
		}

	filter "configurations:Release"
		defines "GAME_RELEASE"
		runtime "Release"
		optimize "on"

		links
		{
		}

	filter "configurations:Dist"
		defines "GAME_DIST"
		runtime "Release"
		optimize "on"

		links
		{
		}
