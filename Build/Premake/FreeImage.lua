include "./Source/ZLib"
include "./Source/OpenEXR"
include "./Source/LibJPEG"
include "./Source/LibTIFF4"
include "./Source/LibPNG"
include "./Source/LibOpenJPEG"
include "./Source/LibRawLite"
include "./Source/LibJXR"
include "./Source/LibWebP"

project "FreeImage"
	kind "SharedLib"
	language "C++"
    staticruntime "off"
	characterset "MBCS"
	
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
	
	files
    {
		"Source/*.h",
		"Source/FreeImage/PSDParser.h",
		"Source/FreeImage/J2KHelper.h",
		"Source/Metadata/*.h",
		"Source/FreeImageToolkit/Resize.h",
		
		"Source/FreeImage/*.c",
		"Source/FreeImage/*.cpp",
		"Source/Metadata/*.cpp",
		"Source/FreeImageToolkit/*.cpp",
    }
	
	removefiles
	{
	}
	
	defines
	{	
		"WIN32",
		"_WINDOWS",
		"_USRDLL",
		"OPJ_STATIC",
		"FREEIMAGE_EXPORTS",
		"LIBRAW_NODLL",
		"_CRT_SECURE_NO_DEPRECATE"
	}

    includedirs
    {
		"Source/ZLib",
		"Source/deprecationManager",
		"Source/FreeImage",
		"Source/OpenEXR",
		"Source/OpenEXR/Half",
		"Source/OpenEXR/Iex",
		"Source/OpenEXR/IlmImf",
		"Source/OpenEXR/Imath",
		"Source/OpenEXR/IlmThread",
		"Source/LibJXR/jxrgluelib",
		"Source/LibJXR/image/sys",
		"Source",
    }
	
	links
	{
		"ZLib",
		"LibJPEG",
		"LibJXR",
		"LibOpenJPEG",
		"LibPNG",
		"LibRAW",
		"LibTIFF4",
		"LibWebP",
		"OpenEXR",
	}
    
    filter "system:windows"
        systemversion "latest"
		files { "FreeImage.rc" }
		vpaths { ["Resources/*"] = { "*.rc" } }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

project "FreeImageLib"
    kind "StaticLib"
    language "C++"
    staticruntime "off"
	characterset "MBCS"
    
    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
		"Source/*.h",
		"Source/FreeImage/PSDParser.h",
		"Source/Metadata/*.h",
		"Source/FreeImageToolkit/Resize.h",

		"Source/FreeImage/*.cpp",
		"Source/Metadata/*.cpp",
		"Source/FreeImageToolkit/*.cpp",
    }
	
	removefiles
	{
	}
	
	defines
	{
		"WIN32",
		"OPJ_STATIC",
		"FREEIMAGE_LIB",
		"_CRT_SECURE_NO_DEPRECATE",
		"LIBRAW_NODLL",
	}

    includedirs
    {
		"Source/ZLib",
		"Source/deprecationManager",
		"Source/FreeImage",
		"Source/OpenEXR",
		"Source/OpenEXR/Half",
		"Source/OpenEXR/Iex",
		"Source/OpenEXR/IlmImf",
		"Source/OpenEXR/Imath",
		"Source/OpenEXR/IlmThread",
		"Source/LibJXR/jxrgluelib",
		"Source/LibJXR/image/sys",
		"Source",
    }
	
	links
	{
		"ZLib",
		"LibJPEG",
		"LibJXR",
		"LibOpenJPEG",
		"LibPNG",
		"LibRAW",
		"LibTIFF4",
		"LibWebP",
		"OpenEXR",
	}
    
    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
		
project "FreeImagePlus"
	kind "StaticLib"
	language "c++"
	staticruntime "off"
	characterset "MBCS"
    
    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
		"Wrapper/FreeImagePlus/*.h",
		"Wrapper/FreeImagePlus/src/*.cpp",
    }
	
	removefiles
	{
	}
	
	defines
	{
		"WIN32",
		"WIN32_LEAN_AND_MEAN",
		"VC_EXTRALEAN",
		"FIP_EXPORTS",
		"FREEIMAGE_LIB",
		"_LIB",
		"_CRT_SECURE_NO_DEPRECATE"
	}

    includedirs
    {
		"Wrapper/FreeImagePlus",
		"Source",
    }
	
	links
	{
		"FreeImageLib",
	}
    
    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"