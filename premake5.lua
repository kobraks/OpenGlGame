include "./vendor/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

if not BOOST then
    error("BOOST_DIR environment variable not set!")
end

workspace "OpenGlGame"
	architecture "x86_64"
	startproject "Editor"

	configurations
	{
		"Debug",
		"Release",
	}

	solution_items
	{
		".editorconfig"
	}

	flags
	{
		"MultiProcessorCompile"
	}
	
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

filter ("action:vs*")
	buildoptions {"/utf-8"}

group "Dependencies"
	include "vendor/premake"
	include "vendor/Glad"
	include "vendor/Lua"
	include "vendor/fmt"
	include "vendor/GLFW"
	include "vendor/FreeImage"
	include "vendor/ImGui"
    include "vendor/spdlog"
    include "vendor/yaml-cpp"
	include "vendor/Assimp"
group ""

group "Core"
	include "Engine"
group ""

group "Tools"
	include "Editor"
group ""

--group "Misc"
--	include "Sandbox"
--group ""