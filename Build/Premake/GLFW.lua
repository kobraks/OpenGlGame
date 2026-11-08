local GLFWRoot = path.join(_MAIN_SCRIPT_DIR, "vendor/GLFW")

project "GLFW"
	kind "StaticLib"
	language "C"
	staticruntime "off"

	location (path.join(_MAIN_SCRIPT_DIR, "Build/Projects/GLFW"))

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		path.join(GLFWRoot, "include/GLFW/glfw3.h"),
		path.join(GLFWRoot, "include/GLFW/glfw3native.h"),

		path.join(GLFWRoot, "src/internal.h"),
		path.join(GLFWRoot, "src/platform.h"),
		path.join(GLFWRoot, "src/mappings.h"),
		
		path.join(GLFWRoot, "src/context.c"),
		path.join(GLFWRoot, "src/init.c"),
		path.join(GLFWRoot, "src/input.c"),
		path.join(GLFWRoot, "src/monitor.c"),
		path.join(GLFWRoot, "src/platform.c"),
		path.join(GLFWRoot, "src/vulkan.c"),
		path.join(GLFWRoot, "src/window.c"),
		
		path.join(GLFWRoot, "src/null_platform.h"),
		path.join(GLFWRoot, "src/null_joystick.h"),		
		path.join(GLFWRoot, "src/null_init.c"),
		path.join(GLFWRoot, "src/null_monitor.c"),
		path.join(GLFWRoot, "src/null_window.c"),
		path.join(GLFWRoot, "src/null_joystick.c"),
	}

	filter "system:linux"
		pic "On"

		systemversion "latest"
		
		files
		{
			path.join(GLFWRoot, "src/posix_time.h"),
			path.join(GLFWRoot, "src/posix_thread.h"),
			
			path.join(GLFWRoot, "src/posix_time.c"),
			path.join(GLFWRoot, "src/posix_thread.c"),
			path.join(GLFWRoot, "src/posix_module.c"),

			path.join(GLFWRoot, "src/x11_platform.h"),
			path.join(GLFWRoot, "src/xkb_unicode.h"),
			
			path.join(GLFWRoot, "src/x11_init.c"),
			path.join(GLFWRoot, "src/x11_monitor.c"),
			path.join(GLFWRoot, "src/x11_window.c"),
			
			path.join(GLFWRoot, "src/xkb_unicode.c"),
			path.join(GLFWRoot, "src/glx_context.c"),
			path.join(GLFWRoot, "src/egl_context.c"),
			path.join(GLFWRoot, "src/osmesa_context.c"),
			
			path.join(GLFWRoot, "src/posix_poll.h"),
			path.join(GLFWRoot, "src/posix_poll.c"),
			
			path.join(GLFWRoot, "src/linux_joystick.h"),
			path.join(GLFWRoot, "src/linux_joystick.c"),
		}

		defines
		{
			"_GLFW_X11"
		}

	filter "system:windows"
		systemversion "latest"

		files
		{
			path.join(GLFWRoot, "src/win32_time.h"),
			path.join(GLFWRoot, "src/win32_thread.h"),
			path.join(GLFWRoot, "src/win32_platform.h"),
			path.join(GLFWRoot, "src/win32_joystick.h"),
			
			path.join(GLFWRoot, "src/win32_module.c"),
			path.join(GLFWRoot, "src/win32_thread.c"),
			path.join(GLFWRoot, "src/win32_time.c"),
			
			path.join(GLFWRoot, "src/win32_init.c"),
			path.join(GLFWRoot, "src/win32_joystick.c"),
			path.join(GLFWRoot, "src/win32_monitor.c"),
			path.join(GLFWRoot, "src/win32_window.c"),
		
			path.join(GLFWRoot, "src/egl_context.c"),
			path.join(GLFWRoot, "src/wgl_context.c"),
			path.join(GLFWRoot, "src/osmesa_context.c"),
		}

		defines 
		{ 
			"_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS"
		}

		links
		{
			"Dwmapi.lib"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"
        symbols "off"