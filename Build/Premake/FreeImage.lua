local FreeImageRoot = path.join(_MAIN_SCRIPT_DIR, "vendor/FreeImage")
local FreeImageCMakePath = path.join(FreeImageRoot, "CMakeLists.txt")
local FreeImageVersionHeader = path.join(FreeImageRoot, "Source/FreeImage.h")

local function ReadTextFile(FilePath)
	local FileHandle, ErrorMessage = io.open(FilePath, "rb")
	if not FileHandle then
		error("FreeImage: cannot read " .. FilePath .. ": " .. tostring(ErrorMessage))
	end

	local Content = FileHandle:read("*a")
	FileHandle:close()
	return Content
end

local CMakeContent = ReadTextFile(FreeImageCMakePath)

local SourceGroups = {
	"FreeImage_SOURCES",
	"FreeImage_LIBJPEG_SRCS",
	"FreeImage_LIBOPENJPEG_SRCS",
	"FreeImage_ZLIB_SRCS",
	"FreeImage_LIBPNG_SRCS",
	"FreeImage_LIBTIFF_SRCS",
	"FreeImage_JXR_SRCS",
	"FreeImage_WEBP_SRCS",
	"FreeImage_LIBRAWLITE_SRCS",
	"FreeImage_OPENEXR_HALF_SRCS",
	"FreeImage_IMATH_SRCS",
	"FreeImage_IEX_SRCS",
	"FreeImage_ILMTHREAD_SRCS",
	"FreeImage_OPENEXRCORE_SRCS",
	"FreeImage_LIBDEFLATE_SRCS",
	"FreeImage_ZSTD_SRCS",
	"FreeImage_OPENJPH_SRCS",
	"FreeImage_OPENEXR_SRCS"
}

local SourceExtensions = {
	c = true,
	cc = true,
	cpp = true,
	h = true,
	hh = true,
	hpp = true,
	inl = true
}

local function CollectFreeImageSources()
	local CollectedSources = {}
	local AddedPaths = {}

	for _, GroupName in ipairs(SourceGroups) do
		local GroupContent = CMakeContent:match("set%s*%(%s*" .. GroupName .. "%s+([^%)]+)%)")
		if not GroupContent then
			error("FreeImage: missing upstream CMake source group: " .. GroupName)
		end

		GroupContent = GroupContent:gsub("#[^\r\n]*", " ")
		for SourceEntry in GroupContent:gmatch("%S+") do
			SourceEntry = SourceEntry:gsub('^"', ""):gsub('"$', ""):gsub("^%./", "")
			local Extension = SourceEntry:match("%.([%a%d]+)$")

			if SourceEntry:sub(1, 7) == "Source/" and SourceExtensions[Extension] then
				local FullPath = path.join(FreeImageRoot, SourceEntry)
				if not AddedPaths[FullPath] then
					AddedPaths[FullPath] = true
					table.insert(CollectedSources, FullPath)
				end
			end
		end
	end

	return CollectedSources
end

local FreeImageSources = CollectFreeImageSources()

project "FreeImageLib"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	cdialect "C11"
	staticruntime "Off"

	location (path.join(_MAIN_SCRIPT_DIR, "Build/Projects", "FreeImageLib"))
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files (FreeImageSources)
	
	includedirs {
		path.join(FreeImageRoot, "Source"),
		path.join(FreeImageRoot, "Source/Metadata"),
		path.join(FreeImageRoot, "Source/FreeImageToolkit"),

		path.join(FreeImageRoot, "Source/LibJPEG"),
		path.join(FreeImageRoot, "Source/LibOpenJPEG"),
		path.join(FreeImageRoot, "Source/LibPNG"),
		path.join(FreeImageRoot, "Source/LibTIFF4"),

		path.join(FreeImageRoot, "Source/LibJXR"),
		path.join(FreeImageRoot, "Source/LibJXR/image/sys"),
		path.join(FreeImageRoot, "Source/LibJXR/common/include"),
		path.join(FreeImageRoot, "Source/LibJXR/jxrgluelib"),

		path.join(FreeImageRoot, "Source/LibWebP"),
		path.join(FreeImageRoot, "Source/LibWebP/src"),
		
		path.join(FreeImageRoot, "Source/LibRawLite"),
		path.join(FreeImageRoot, "Source/LibRawLite/dcraw"),
		path.join(FreeImageRoot, "Source/LibRawLite/internal"),
		path.join(FreeImageRoot, "Source/LibRawLite/libraw"),
		path.join(FreeImageRoot, "Source/LibRawLite/src"),

		path.join(FreeImageRoot, "Source/OpenEXR"),
		path.join(FreeImageRoot, "Source/OpenEXR/Half"),
		path.join(FreeImageRoot, "Source/OpenEXR/Iex"),
		path.join(FreeImageRoot, "Source/OpenEXR/IlmImf"),
		path.join(FreeImageRoot, "Source/OpenEXR/IlmThread"),
		path.join(FreeImageRoot, "Source/OpenEXR/OpenEXRCore"),
		path.join(FreeImageRoot, "Source/OPENEXR/Zstd/lib"),
		path.join(FreeImageRoot, "Source/OPENEXR/OpenJPH"),
		path.join(FreeImageRoot, "Source/OPENEXR/OpenJPH/openjph"),

		path.join(FreeImageRoot, "Source/LibDeflate"),
		path.join(FreeImageRoot, "Source/Imath"),
		path.join(FreeImageRoot, "Source/ZLib")
	}
	defines {
		"DISABLE_PERF_MEASUREMENT",
		"OPJ_STATIC",
		"NO_LCMS",
		"NO_WINDOWS",
		"INCLUDE_LIB_ZLIB",
		"INCLUDE_LIB_TIFF4",
		"INCLUDE_LIB_JXR",
		"INCLUDE_LIB_WEBP",
		"INCLUDE_LIB_RAW",
		"INCLUDE_LIB_OPENEXR",
		"LIBRAW_NODLL",
		"USE_X3FTOOLS",
		"PNG_ARM_NEON_OPT=0",
		"ZSTD_DISABLE_ASM",
		"_CRT_SECURE_NO_WARNINGS",
		"FREEIMAGE_LIB"
	}

	filter "system:windows"
		systemversion "latest"
		links { "ws2_32" }
		
		defines {
			"WIN32",
			"WIN32_LEAN_AND_MEAN"
		}
		
		files {
			path.join(FreeImageRoot, "Source/LibTIFF4/tif_win32.c"),
			path.join(FreeImageRoot, "FreeImage.rc")
		}

		filter "system:linux"
			defines { "HAVE_UNISTD_H", "_FILE_OFFSET_BITS=64", "__ANSI__" }
		
		filter "configurations:Debug"
			runtime "Debug"
			symbols "On"
				
		filter "configurations:Release"
			runtime "Release"
			optimize "On"

		filter "configurations:Dist"
			runtime "Release"
			optimize "On"