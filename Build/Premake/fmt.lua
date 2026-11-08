local FmtRoot = path.join(_MAIN_SCRIPT_DIR, "vendor/fmt")

project "fmt"
	kind "StaticLib"
	language "C++"
	cppdialect "C++latest"
	staticruntime "off"

	location (path.join(_MAIN_SCRIPT_DIR, "Build/Projects/fmt"))

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		path.join(FmtRoot, "include/fmt/**.h"),
		path.join(FmtRoot, "src/format.cc"),
		path.join(FmtRoot, "src/os.cc"),
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
	}

	includedirs
	{
		path.join(FmtRoot, "include"),
	}

	links
	{
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

        defines { "DEBUG" }

        links
        {
        }

    filter "configurations:Release"
        runtime "Release"
        optimize "On"
        symbols "Off"

        defines { "NDEBUG" }

        links
        {
        }

    filter "configurations:Dist"
        runtime "Release"
        optimize "on"
        symbols "Off"

        defines { "NDEBUG" }

        links
        {
        }