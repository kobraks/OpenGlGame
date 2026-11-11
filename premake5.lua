include "./vendor/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

if not BOOST then
    error("BOOST_DIR environment variable not set!")
end

workspace "OpenGlGame"
	architecture "x86_64"
	startproject "Editor"

	configurations {
		"Debug",
		"Release",
	}

	solution_items {
		".editorconfig"
	}

	location "."

	--removed as premake5 5.0.0 released
	--flags
	--{
	--	"MultiProcessorCompile"
	--}

	multiprocessorcompile "on"
	staticruntime "off"
	
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

filter ("action:vs*")
	buildoptions {"/utf-8"}

filter ("system:windows", "action:vs*")
	characterset "Unicode"

filter ("configurations:Debug")
	runtime "Debug"
	symbols "On"
	optimize "Off"

	defines { "DEBUG" }

filter ("configurations:Release")
	runtime "Release"
	symbols "Off"
	optimize "On"

	defines { "NDEBUG" }

group "Dependencies"
	include "vendor/premake"
	include "Build/Premake/Glad"
	include "Build/Premake/Lua"
	include "Build/Premake/fmt"
	include "Build/Premake/GLFW"
	include "Build/Premake/FreeImage"
	include "Build/Premake/ImGui"
	include "Build/Premake/ImGuizmo"
	include "Build/Premake/glm"
    include "Build/Premake/spdlog"
    include "Build/Premake/yaml-cpp"
	include "Build/Premake/Assimp"
group ""

group "Core"
	include "Build/Premake/Engine"
group ""

group "Tools"
	include "Build/Premake/Editor"
group ""

--group "Misc"
--	include "Sandbox"
--group ""