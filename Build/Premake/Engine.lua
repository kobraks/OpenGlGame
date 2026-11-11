local EngineRoot = path.join(_MAIN_SCRIPT_DIR, "Engine")

project "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++23"
	staticruntime "off"

	location (path.join(_MAIN_SCRIPT_DIR, "Build/Projects/Engine"))

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource (path.join(EngineRoot, "src/pch.cpp"))
	
	files {
		path.join(EngineRoot, "src/**.h"),
		path.join(EngineRoot, "src/**.cpp"),
	}

	defines {
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE",
		"SPDLOG_FMT_EXTERNAL",
        "SPDLOG_COMPILED_LIB",
		"YAML_CPP_STATIC_DEFINE",
		"FREEIMAGE_LIB",
	}

	includedirs {
		path.join(EngineRoot, "src"),
	}

	externalincludedirs {
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

		buildoptions { "/Zc:preprocessor" }

	filter "system:windows"
		systemversion "latest"

		defines { 
			"NOMINMAX",
			"WIN32_LEAN_AND_MEAN",
		}

	filter "configurations:Debug"
		defines { "ENGINE_DEBUG", "ENGINE_ENABLE_OPENGL_DEBUG" }

	filter "configurations:Release"
		defines { "ENGINE_RELEASE" }
