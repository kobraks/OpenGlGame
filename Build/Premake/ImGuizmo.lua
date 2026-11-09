local ImGuizmoRoot = path.join(_MAIN_SCRIPT_DIR, "vendor/ImGuizmo")
local ImGuiRoot = path.join(_MAIN_SCRIPT_DIR, "vendor/ImGui")

project "ImGuizmo"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	location (path.join(_MAIN_SCRIPT_DIR, "Build/Projects/ImGuizmo"))

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		path.join(ImGuizmoRoot, "src/**.h"),
		path.join(ImGuizmoRoot, "src/**.cpp"),
	}

    includedirs {
        path.join(ImGuizmoRoot, "src"),
		ImGuiRoot
    }

	filter "system:windows"
		systemversion "latest"

	filter "system:linux"
		pic "On"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"