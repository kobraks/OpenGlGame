newoption { trigger = "assimp-shared", description = "Build Assimp as a shared library (DLL/.so)", default = "off" }
newoption { trigger = "assimp-double", description = "Enable ASSIMP_DOUBLE_PRECISION", default = "off"  }
newoption { trigger = "assimp-no-export", description = "Disable Assimp export functionality", default = "off"  }

newoption { trigger = "assimp-usd", description = "Enable USD importer (opt-in)", default = "off"  }
newoption { trigger = "assimp-vrml", description = "Enable VRML importer (opt-in)", default = "off"  }
newoption { trigger = "assimp-m3d", description = "Enable M3D importer/exporter (opt-in)", default = "off"  }

newoption { trigger = "assimp-c4d", description = "Enable C4D importer (opt-in)", default = "off"  }
-- newoption { trigger = "assimp-x", description = "Enable X importer (opt-in)", default = "off"  }
-- newoption { trigger = "assimp-xgl", description = "Enable xgl importer (opt-in)", default = "off"  }
-- newoption { trigger = "assimp-3mf", description = "Enable 3mf importer (opt-in)", default = "off"  }
-- newoption { trigger = "assimp-amf", description = "Enable amf importer (opt-in)", default = "off"  }
-- newoption { trigger = "assimp-ifc", description = "Enable ifc importer (opt-in)", default = "off"  }

newoption { trigger = "assimp-build-zlib", description = "Build boundled zlib (recommended on Win)", default = "on"  }
newoption { trigger = "assimp-static-crt", description = "MSVC: /MT /MTd (static runtime)", default = "off"  }

newoption { trigger = "assimp-tools", description = "Build assimp_cmd / tools (optional)", default = "off"  }
newoption { trigger = "assimp-tests", description = "Build test suite (optional)", default = "off"  }

newoption { trigger = "assimp-zipio", description = "Enable zip archive IO (minizip/unzip sources)", default = "on" }

newoption { trigger = "zlib-asm686", description = "Enable i686 assembly (zlib)", default = "off" }
newoption { trigger = "zlib-amd64", description = "Enable amd64 assembly (zlib)", default = "off"}

local function optOn(n) return _OPTIONS[n] == "on" end
local assimpRoot = path.join(_MAIN_SCRIPT_DIR, "vendor/Assimp") --path.getabsolute(_SCRIPT_DIR)

local genDir = path.join(_MAIN_SCRIPT_DIR, "Build/Generated/Assimp") --path.join(assimpRoot, "generated")
local genInc = path.join(genDir, "include")

local function ensureDir(p) os.mkdir(p) end
local function writeFile(p, s)
    local f = io.open(p, "wb")
    if f then f:write(s) f:close() end
end

local function ensureGeneratedDirs()
    ensureDir(genDir)
    ensureDir(genInc)
    ensureDir(path.join(genDir, "assimp"))
    ensureDir(path.join(genInc, "assimp"))
end

local function configureCMakeHeader(inPath, outPath, valueSubst, onOff)
    if not os.isfile(inPath) then
        error('Missing template: "' .. inPath .. '"')
    end

    local f = io.open(inPath, "rb")
    local s = f:read("*a")
    f:close()

    -- 1) replace @VAR@ tokens first (so cmakedefine lines that contain @...@ become valid)
    s = s:gsub("@([%w_]+)@", function(k)
        local v = valueSubst[k]
        if v == nil then return "0" end
        return tostring(v)
    end)

    -- 2) #cmakedefine01 NAME ... -> #define NAME 0/1
    s = s:gsub("[^\r\n]*#cmakedefine01%s+([%w_]+)[^\r\n]*", function(name)
        local v = onOff[name] and 1 or 0
        return "#define " .. name .. " " .. v
    end)

    -- 3) #cmakedefine NAME ... -> ether define or undef
    -- If enabled, we define it as 1 (safe default) unless the line already had a value after substitution.
    s = s:gsub("[^\r\n]*#cmakedefine%s+([%w_]+)%s*([^\r\n]*)", function(name, rest)
        if onOff[name] then
            -- If template provided a value (after @VAR@ subsitution), keep it; otherwise define 1
            if rest ~= nil and #rest > 0 then
                return "#define " .. name .. " " .. rest
            end
            return "#define " .. name .. " 1"
        end
        return "/* #undef " .. name .. " */"
    end)

    writeFile(outPath, s)
end

local function generateAssimpRevisionH()
    local inPath = path.join(assimpRoot, "include/assimp/revision.h.in")

    if not inPath or not os.isfile(inPath) then
        error ("Missing revision template: include/assimp/revision.h.in")
    end

    local outPath = path.join(genInc, "assimp", "revision.h")

    local nullredir = (os.host() == "windows") and "2>nul" or "2>/dev/null"
    local branch = os.outputof('git -C "' .. assimpRoot .. '" rev-parse --abbrev-ref HEAD ' .. nullredir) or ""
    local hash = os.outputof('git -C "' .. assimpRoot .. '" rev-parse --short=8 HEAD ' .. nullredir) or ""

    branch = branch:gsub("%s+", "")
    hash = hash:gsub("%s+", "")

    if #hash == 0 then hash = "0" end

    local valueSubst = {
        GIT_BRANCH = branch,
        GIT_COMMIT_HASH = hash,
    }

    local onOff = {}

    configureCMakeHeader(inPath, outPath, valueSubst, onOff)
end

local function generateAssimpConfigH()
    local inPath = path.join(assimpRoot, "include/assimp/config.h.in")
    local outPath = path.join(genInc, "assimp", "config.h")

    local onOff = {
        ASSIMP_DOUBLE_PRECISION = (_OPTIONS["assimp-double"] == "on"),
        ASSIMP_BUILD_NO_EXPORT = (_OPTIONS["assimp-no-export"] == "on"),
    }

    local valueSubst = {

    }

    configureCMakeHeader(inPath, outPath, valueSubst, onOff)
end

local function generateZlibHeaders()
    local inPath = path.join(assimpRoot, "contrib/zlib/zconf.h.in")
    local outDir = path.join(genDir, "zlib")
    local outPath = path.join(outDir, "zconf.h")

    ensureDir(outDir)
    
    local isWindows = (os.host() == "windows")

    local onOff = {}

    local valueSubst = {
        HAVE_SYS_TYPES_H = 1,
        HAVE_STDINT_H = 1,
        HAVE_STDDEF_H = 1,
        Z_HAVE_UNISTD_H = isWindows and 0 or 1,
        HAVE_FSEEKO = isWindows and 0 or 1,
    }
    configureCMakeHeader(inPath, outPath, valueSubst, onOff)
end

local function generateAssimpPCH()
    local outPath = path.join(genInc, "AssimpPCH.h")

    writeFile(outPath, [[
#pragma once

#include <assimp/config.h>
#include <assimp/revision.h>
#include <assimp/defs.h>

#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <algorithm>
#include <memory>
#include <utility>

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>

]])
end

local function generateAssimpPCHSource()
    local outDir = genDir
    ensureDir(outDir)
    writeFile(path.join(outDir, "AssimpPCH.cpp"), [[
#include "AssimpPCH.h"
]])
end

local function assimpGenerateAll()
    ensureGeneratedDirs()
    generateAssimpRevisionH()
    generateAssimpConfigH()
    generateAssimpPCH()
    generateAssimpPCHSource()
    if optOn("assimp-build-zlib") then generateZlibHeaders() end
end

local function addAssimpContrib()
    local c = path.join(assimpRoot, "contrib")

    includedirs {
        c,

        -- header-only or mostly-headers
        path.join(c, "rapidjson/include"),
        path.join(c, "utf8cpp/source"),

        path.join(c, "pugixml/src"),
        path.join(c, "openddlparser/include"),
        path.join(c, "poly2tri"),
        path.join(c, "poly2tri/poly2tri"),
        path.join(c, "clipper"),
        path.join(c, "open3dgc"),
    }

    files {
        -- pugixml is not header-only
        path.join(c, "pugixml/src/pugixml.cpp"),
        path.join(c, "pugixml/src/pugiconfig.hpp"),
        path.join(c, "pugixml/src/pugixml.hpp"),

        -- those are typically built from sources in Assimp builds
        path.join(c, "openddlparser/code/**.cpp"),
        path.join(c, "openddlparser/code/**.c"),
        path.join(c, "openddlparser/include/**.h"),

        path.join(c, "poly2tri/poly2tri/**.cc"),
        path.join(c, "poly2tri/poly2tri/**.cpp"),
        path.join(c, "poly2tri/poly2tri/**.h"),

        path.join(c, "clipper/**.cpp"),
        path.join(c, "clipper/**.hpp"),

        path.join(c, "open3dgc/**.cpp"),
        path.join(c, "open3dgc/**.h"),
    }

    if optOn("assimp-zipio") then
        includedirs {
            path.join(c, "unzip")
        }

        files {
            path.join(c, "unzip/**.c"),
            path.join(c, "unzip/**.h"),
        }
    end
end

assimpGenerateAll()

project "assimp"
    kind(optOn("assimp-shared") and "SharedLib" or "StaticLib")
    language "C++"
    cppdialect "C++17"
    cdialect "C99"
    staticruntime (optOn("assimp-static-crt") and "On" or "Off")

    -- pchheader "AssimpPCH.h"
    -- pchsource "AssimpPCH.cpp"

    -- forceincludes { "AssimpPCH.h" }

    location (path.join(_MAIN_SCRIPT_DIR, "Build/Projects/Assimp"))

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        path.join(genInc, "AssimpPCH.h"),
        path.join(genDir, "AssimpPCH.cpp"),

        -- Root-level core compilation units
        path.join(assimpRoot, "code/*.cpp"),
        path.join(assimpRoot, "code/*.c"),

        -- Core subsystems
        path.join(assimpRoot, "code/Common/**.cpp"),
        path.join(assimpRoot, "code/Common/**.c"),

        path.join(assimpRoot, "code/PostProcessing/**.cpp"),
        path.join(assimpRoot, "code/PostProcessing/**.c"),

        path.join(assimpRoot, "code/Material/**.cpp"),
        path.join(assimpRoot, "code/Material/**.c"),

        path.join(assimpRoot, "code/Geometry/**.cpp"),
        path.join(assimpRoot, "code/Geometry/**.c"),

        path.join(assimpRoot, "code/CApi/**.cpp"),
        path.join(assimpRoot, "code/CApi/**.c"),
        
        -- Public headers (for IDE browsing)
        path.join(assimpRoot, "include/**.h"),
        path.join(assimpRoot, "include/**.hpp"),
    }

    -- include all importers 
    files {
        path.join(assimpRoot, "code/AssetLib/**.cpp"),
        path.join(assimpRoot, "code/AssetLib/**.c"),
        path.join(assimpRoot, "code/AssetLib/**.h"),
        path.join(assimpRoot, "code/AssetLib/**.inl"),
    }

    includedirs {
        genInc, -- generated/include
        assimpRoot,
        path.join(assimpRoot, "include"),
        path.join(assimpRoot, "code"),
    }

    addAssimpContrib()

    defines {
        "OPENDDLPARSER_BUILD",

        "RAPIDJSON_HAS_STDSTRING=1",
        "RAPIDJSON_NOMEMBERITERATORCLASS",

        "MINIZ_USE_UNALIGNED_LOADS_AND_STORES=0",
        "ASSIMP_IMPORTER_GLTF_USE_OPEN3DGC=1",

        "ASSIMP_BUILD_NO_USD_IMPORTER",
        "ASSIMP_BUILD_NO_M3D_IMPORTER",
        "ASSIMP_BUILD_NO_M3D_EXPORTER",
        "ASSIMP_BUILD_NO_VRML_IMPORTER",

        "ASSIMP_BUILD_NO_C4D_IMPORTER",
        -- "ASSIMP_BUILD_NO_X_IMPORTER",
        -- "ASSIMP_BUILD_NO_XGL_IMPORTER",
        -- "ASSIMP_BUILD_NO_3MF_IMPORTER",
        -- "ASSIMP_BUILD_NO_AMF_IMPORTER",
        -- "ASSIMP_BUILD_NO_IFC_IMPORTER",
    }

    removefiles { 
        path.join(assimpRoot, "code/AssetLib/IFC/IFCReaderGen_4.*"),
    }

    if optOn("assimp-c4d") then
        undefines { "ASSIMP_BUILD_NO_C4D_IMPORTER" }
    else
        removefiles { path.join(assimpRoot, "code/AssetLib/C4D/**") }
    end

    if optOn("assimp-usd") then
        undefines { "ASSIMP_BUILD_NO_USD_IMPORTER"}
    else
        removefiles { path.join(assimpRoot, "code/AssetLib/USD/**") }
    end

    if optOn("assimp-vrml") then
        undefines { "ASSIMP_BUILD_NO_VRML_IMPORTER" }
    else
        removefiles { path.join(assimpRoot, "code/AssetLib/VRML/**") }
    end

    -- if optOn("assimp-x") then
    --     undefines { "ASSIMP_BUILD_NO_X_IMPORTER" }
    -- else
    --     removefiles { path.join(assimpRoot, "code/AssetLib/X/**") }
    -- end

    -- if optOn("assimp-xgl") then
    --     undefines { "ASSIMP_BUILD_NO_XGL_IMPORTER" }
    -- else
    --     removefiles { path.join(assimpRoot, "code/AssetLib/XGL/**") }
    -- end

    -- if optOn("assimp-3mf") then
    --     undefines { "ASSIMP_BUILD_NO_3MF_IMPORTER" }
    -- else
    --     removefiles { path.join(assimpRoot, "code/AssetLib/3MF/**") }
    -- end
    
    -- if optOn("assimp-amf") then
    --     undefines { "ASSIMP_BUILD_NO_AMF_IMPORTER" }
    -- else
    --     removefiles { path.join(assimpRoot, "code/AssetLib/AMF/**") }
    -- end

    -- if optOn("assimp-ifc") then
    --     undefines { "ASSIMP_BUILD_NO_IFC_IMPORTER" }
    -- else
    --     removefiles { path.join(assimpRoot, "code/AssetLib/IFC/**") }
    -- end

    if optOn("assimp-m3d") then
        undefines { "ASSIMP_BUILD_NO_M3D_IMPORTER", "ASSIMP_BUILD_NO_M3D_EXPORTER" }
    else
        removefiles { path.join(assimpRoot, "code/AssetLib/M3D/**") }
    end

    if optOn("assimp-double") then
        defines { "ASSIMP_DOUBLE_PRECISION" }
    end

    if optOn("assimp-no-export") then
        defines { "ASSIMP_BUILD_NO_EXPORT" }

        removefiles {
            path.join(assimpRoot, "code/**/**Exporter**.cpp"),
            path.join(assimpRoot, "code/**/**Exporter**.h"),
            path.join(assimpRoot, "code/**/**AssetWriter**.cpp"),
            path.join(assimpRoot, "code/**/**AssetWriter**.h"),
            path.join(assimpRoot, "code/**/**AssetWriter**.inl"),

            path.join(assimpRoot, "code/AssetLib/**/**Exporter**"),
        }
    end

    filter "configurations:Debug"
        targetsuffix "d"
    filter{}

    filter "system:windows"
        defines { "WIN32_LEAN_AND_MEAN", "NOMINMAX" }
        multiprocessorcompile "on"
        buildoptions { "/bigobj" }

        if optOn("assimp-static-crt") then
            defines { "_CRT_SECURE_NO_WARNINGS", "_SCL_SECURE_NO_WARNINGS" }
        end
    filter{}

    filter "toolset:gcc or toolset:clang"
        buildoptions { "-fvisibility=hidden", "-fno-strict-aliasing", "-Wall" }
    filter{}

    if optOn("assimp-build-zlib") then
        links { "zlibstatic" }
        includedirs {
            path.join(genDir, "zlib"),
            path.join(assimpRoot, "contrib/zlib"),
            path.join(assimpRoot, "contrib/zlib/include")
        }
    else
        defines { "ASSIMP_BUILD_NO_OWN_ZLIB" }
        filter "system:windows"
            links { "zlib" }
        filter "system:linux or macosx"
            links { "z" }
        filter {}
    end

if optOn("assimp-build-zlib") then
    project "zlibstatic"
        kind "StaticLib"
        language "C"
        cdialect "C99"

        staticruntime (optOn("assimp-static-crt") and "On" or "Off")

        location (path.join(_MAIN_SCRIPT_DIR, "Build/Projects/zlibstatic"))
        
        targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

        local zconfGenDir = path.join(genDir, "zlib")
        local zlibRoot = path.join(assimpRoot, "contrib", "zlib")

        files {
            path.join(zlibRoot, "adler32.c"),
            path.join(zlibRoot, "compress.c"),
            path.join(zlibRoot, "crc32.c"),
            path.join(zlibRoot, "deflate.c"),
            path.join(zlibRoot, "gzclose.c"),
            path.join(zlibRoot, "gzlib.c"),
            path.join(zlibRoot, "gzread.c"),
            path.join(zlibRoot, "gzwrite.c"),
            path.join(zlibRoot, "inflate.c"),
            path.join(zlibRoot, "infback.c"),
            path.join(zlibRoot, "inftrees.c"),
            path.join(zlibRoot, "inffast.c"),
            path.join(zlibRoot, "trees.c"),
            path.join(zlibRoot, "uncompr.c"),
            path.join(zlibRoot, "zutil.c"),

            path.join(zlibRoot, "zlib.h"),
            path.join(zconfGenDir, "zconf.h"),

            path.join(zlibRoot, "crc32.h"),
            path.join(zlibRoot, "deflate.h"),
            path.join(zlibRoot, "gzguts.h"),
            path.join(zlibRoot, "inffast.h"),
            path.join(zlibRoot, "inffixed.h"),
            path.join(zlibRoot, "inflate.h"),
            path.join(zlibRoot, "inftrees.h"),
            path.join(zlibRoot, "trees.h"),
            path.join(zlibRoot, "zutil.h"),
        }

        includedirs {
            zconfGenDir,
            zlibRoot
        }

        filter "system:windows"
            defines { 
                "_CRT_NONSTDC_NO_DEPRECATE",
                "_CRT_SECURE_NO_WARNINGS",
                "NO_FSEEKO"
            }

            buildoptions { "/wd4131", "/wd4127", "/wd4244" }
        filter {}
end