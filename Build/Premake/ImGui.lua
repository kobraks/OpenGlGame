local ImGuiRoot = path.join(_MAIN_SCRIPT_DIR, "vendor/ImGui")

project "ImGui"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	location (path.join(_MAIN_SCRIPT_DIR, "Build/Projects/ImGui"))

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		path.join(ImGuiRoot, "imconfig.h"),
		path.join(ImGuiRoot, "imgui.h"),
		path.join(ImGuiRoot, "imgui_internal.h"),
		path.join(ImGuiRoot, "imstb_rectpack.h"),
		path.join(ImGuiRoot, "imstb_textedit.h"),
		path.join(ImGuiRoot, "imstb_truetype.h"),

		path.join(ImGuiRoot, "imgui.cpp"),
		path.join(ImGuiRoot, "imgui_draw.cpp"),
		path.join(ImGuiRoot, "imgui_tables.cpp"),
		path.join(ImGuiRoot, "imgui_widgets.cpp"),
		path.join(ImGuiRoot, "imgui_demo.cpp")
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