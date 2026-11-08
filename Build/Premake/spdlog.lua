local SPDLogRoot = path.join(_MAIN_SCRIPT_DIR, "vendor/spdlog")

project "spdlog"
    kind "StaticLib"
    language "C++"
    cppdialect "C++latest"
    staticruntime "off"

    location (path.join(_MAIN_SCRIPT_DIR, "Build/Projects/spdlog"))

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    defines 
    {
        "SPDLOG_FMT_EXTERNAL",
        "SPDLOG_COMPILED_LIB",
    }

    files {
        path.join(SPDLogRoot, "src/**.cpp"),
        --"src/spdlog.cpp",
        --"src/stdout_sinks.cpp",
        --"src/color_sinks.cpp",
        --"src/file_sinks.cpp",
        --"src/async.cpp",
        --"src/cfg.cpp",

        path.join(SPDLogRoot, "include/**.h"),
    }

    includedirs
    {
        path.join(SPDLogRoot, "include"),
        "%{IncludeDir.Fmt}",
    }

    links
    {
        "fmt",
    }

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
