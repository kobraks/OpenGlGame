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
		
		"%{wks.location}/vendor/glm/**.hpp",
		"%{wks.location}/vendor/glm/**.inl",
		
		"%{wks.location}/vendor/ImGuizmo/ImGuizmo.h",
		"%{wks.location}/vendor/ImGuizmo/ImGuizmo.cpp",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE",
		"SPDLOG_FMT_EXTERNAL"
	}

	includedirs
	{
		"src",
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
	}

	links
	{
		"Glad",
		"GLFW",
		"Lua54",
		"Fmt",
		"ImGui",
		"FreeImageLib",
		
		"opengl32.lib",
	}

	filter "files:**/vendor/ImGuizmo/**.cpp" --Crude fix think about something else
	flags { "NoPCH" }

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
