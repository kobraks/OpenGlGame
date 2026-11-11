local EditorRoot = path.join(_MAIN_SCRIPT_DIR, "Editor")

project "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++23"
	staticruntime "off"

	location (path.join(_MAIN_SCRIPT_DIR, "Build/Projects/Editor"))

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	debugdir (path.join(_MAIN_SCRIPT_DIR, "Editor"))

	files {
		path.join(EditorRoot, "src/**.h"),
		path.join(EditorRoot, "src/**.cpp"),
	}
	
	defines {
		"SPDLOG_FMT_EXTERNAL",
        "SPDLOG_COMPILED_LIB",
		"YAML_CPP_STATIC_DEFINE",
		"GAME_DEFINE_DISCRETE_GPU_PREFERENCE",
	}
	
	includedirs {
		path.join(EditorRoot, "src"),
	}

	externalincludedirs {
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
		"%{IncludeDir.Yaml}",
	}

	links
	{
		"Engine",
		"fmt",
		"ImGuizmo"
	}

	filter "system:windows"
		systemversion "latest"

		defines { 
			"NOMINMAX",
			"WIN32_LEAN_AND_MEAN",
		}
		
	filter "action:vs*"
		externalanglebrackets "On"
		externalwarnings "Off" --/external:W0
		-- buildoptions { "/external:W0" }
		buildoptions { "/Zc:preprocessor" }

	filter "configurations:Debug"
		defines { "ENGINE_DEBUG", "ENGINE_ENABLE_OPENGL_DEBUG" }

	filter "configurations:Release"
		defines { "ENGINE_RELEASE" }

