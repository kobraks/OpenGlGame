project "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
	}
	
	defines 
	{
		"SPDLOG_FMT_EXTERNAL"
	}
	
	includedirs
	{
		"%{wks.location}/Engine/src",
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
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.Entt}",
	}

	links
	{
		"Engine",
	}

	filter "system:windows"
		systemversion "latest"
		
	filter "action:vs*"
		buildoptions { "/external:anglebrackets" }

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
