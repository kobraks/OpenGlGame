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
		"SPDLOG_FMT_EXTERNAL",
        "SPDLOG_COMPILED_LIB",
		"GAME_DEFINE_DISCRETE_GPU_PREFERENCE"
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
		"Fmt"
	}

	filter "system:windows"
		systemversion "latest"
		
	filter "action:vs*"
		externalanglebrackets "On"
		externalwarnings "Off" --/external:W0
		-- buildoptions { "/external:W0" }

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

		defines { "GAME_DEBUG", "DEBUG" }

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

		defines { "GAME_RELEASE", "NDEBUG" }

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"

		defines { "GAME_Dist", "NDEBUG" }

