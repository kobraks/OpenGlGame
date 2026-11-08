local YAMLCPPRoot = path.join(_MAIN_SCRIPT_DIR, "vendor/yaml-cpp")

project "yaml-cpp"
	kind "StaticLib"
	language "C++"

	location (path.join(_MAIN_SCRIPT_DIR, "Build/Projects/yaml-cpp"))

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		path.join(YAMLCPPRoot, "src/**.h"),
		path.join(YAMLCPPRoot, "src/**.cpp"),
		
		path.join(YAMLCPPRoot, "include/**.h")
	}

	includedirs
	{
		path.join(YAMLCPPRoot, "include")
	}

	defines {
		"YAML_CPP_STATIC_DEFINE"
		--"yaml_cpp_EXPORTS" --for dll
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++20"
		staticruntime "off"

	filter "system:linux"
		pic "On"
		systemversion "latest"
		cppdialect "C++20"
		staticruntime "off"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"