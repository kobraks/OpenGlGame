local GladRoot = path.join(_MAIN_SCRIPT_DIR, "vendor/glad")

project "Glad"
    kind "StaticLib"
    language "C"
    staticruntime "off"
    
    location (path.join(_MAIN_SCRIPT_DIR, "Build/Projects/glad"))

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        path.join(GladRoot, "include/glad/glad.h"),
        path.join(GladRoot, "include/KHR/khrplatform.h"),
        path.join(GladRoot, "src/glad.c")
    }

    includedirs
    {
        path.join(GladRoot, "include")
    }
    
    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

        defines { "DEBUG" }

    filter "configurations:Release"
        runtime "Release"
        optimize "On"
        symbols "Off"

        defines { "NDEBUG" }
