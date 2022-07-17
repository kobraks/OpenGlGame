include "./vendor/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

workspace "OpenGlGame"
	architecture "x86_64"
	startproject "Editor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
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

group "Dependencies"
	include "vendor/premake"
	include "vendor/Glad"
	include "vendor/Lua"
	include "vendor/fmt"
	include "vendor/GLFW"
	include "vendor/FreeImage"
	include "vendor/ImGui"
--	include "Hazel/vendor/yaml-cpp"
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
