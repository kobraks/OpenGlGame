#include "pch.h"
#include "Monitor.h"

#include "Window.h"

namespace Game
{
	std::vector<Monitor> Monitor::s_Monitors;
	Monitor Monitor::s_PrimaryMonitor;
	bool Monitor::s_Initialized = false;

	static VideoMode CreateVideoMode(GLFWvidmode videoMode)
	{
		VideoMode mode;

		mode.BlueBits = videoMode.blueBits;
		mode.GreenBits = videoMode.greenBits;
		mode.RedBits = videoMode.redBits;
		mode.RefreshRate = videoMode.refreshRate;

		mode.Size = Vector2u(static_cast<Vector2u::ValueType>(videoMode.width),
		                     static_cast<Vector2u::ValueType>(videoMode.height));

		return mode;
	}

	std::vector<VideoMode> CreateVideoModes(GLFWmonitor* monitor)
	{
		std::vector<VideoMode> modes;

		int count = 0;
		const GLFWvidmode* videoModes = glfwGetVideoModes(monitor, &count);

		modes.reserve(static_cast<std::vector<VideoMode>::size_type>(count));

		for (int i = 0; i < count; ++i)
			modes.emplace_back(CreateVideoMode(videoModes[i]));

		return modes;
	}

	GammaRamp::~GammaRamp()
	{
		delete[] Red;
		delete[] Green;
		delete[] Blue;

		Red = Green = Blue = nullptr;
	}

	void Monitor::SetUserData(void* userData)
	{
		glfwSetMonitorUserPointer(m_NativePointer, userData);
	}

	void Monitor::SetGamma(float gamma)
	{
		glfwSetGamma(m_NativePointer, gamma);
	}

	void Monitor::SetGammaRamp(const GammaRamp& ramp)
	{
		GLFWgammaramp gammaRamp;
		gammaRamp.size = ramp.Size;

		if (!ramp.Blue || !ramp.Green || !ramp.Red)
			return;

		gammaRamp.blue = ramp.Blue;
		gammaRamp.green = ramp.Green;
		gammaRamp.red = ramp.Red;

		glfwSetGammaRamp(m_NativePointer, &gammaRamp);
	}

	GammaRamp Monitor::GetGammaRamp() const
	{
		GammaRamp gammaRamp;
		auto ramp = glfwGetGammaRamp(m_NativePointer);

		gammaRamp.Size = ramp->size;

		gammaRamp.Red = new uint16_t[ramp->size];
		gammaRamp.Green = new uint16_t[ramp->size];
		gammaRamp.Blue = new uint16_t[ramp->size];

		std::memcpy(gammaRamp.Red, ramp->red, ramp->size);
		std::memcpy(gammaRamp.Green, ramp->green, ramp->size);
		std::memcpy(gammaRamp.Blue, ramp->blue, ramp->size);

		return gammaRamp;
	}

	Monitor Monitor::GetPrimary()
	{
		Populate();

		return s_PrimaryMonitor;
	}

	Monitor Monitor::Get(size_t monitor)
	{
		Populate();

		return s_Monitors[monitor];
	}

	std::vector<Monitor> Monitor::GetAll()
	{
		Populate();

		return s_Monitors;
	}

	Monitor Monitor::Create(GLFWmonitor* pointer)
	{
		Monitor monitor;

		if (!pointer)
			return monitor;

		int x, y;
		glfwGetMonitorPhysicalSize(pointer, &x, &y);

		monitor.m_Size = Vector2u(static_cast<Vector2u::ValueType>(x), static_cast<Vector2u::ValueType>(y));

		glfwGetMonitorContentScale(pointer, &monitor.m_Scale.X, &monitor.m_Scale.Y);
		glfwGetMonitorPos(pointer, &monitor.m_Position.X, &monitor.m_Position.Y);
		glfwGetMonitorWorkarea(pointer, &monitor.m_WorkArea.X, &monitor.m_WorkArea.Y, &monitor.m_WorkArea.Width, &monitor.m_WorkArea.Height);

		monitor.m_Name = glfwGetMonitorName(pointer);
		monitor.m_UserData = glfwGetMonitorUserPointer(pointer);
		monitor.m_VideoModes = CreateVideoModes(pointer);
		monitor.m_VideoMode = CreateVideoMode(*glfwGetVideoMode(pointer));
		monitor.m_NativePointer = pointer;

		return monitor;
	}

	void Monitor::Populate()
	{
		if (!s_Initialized)
		{
			Window::InitializeGlfw();
			s_Initialized = true;
		}
		else
			return;

		int count = 0;
		GLFWmonitor** monitors = glfwGetMonitors(&count);

		for (int i = 0; i < count; ++i)
			s_Monitors.emplace_back(Create(monitors[i]));

		s_PrimaryMonitor = Create(glfwGetPrimaryMonitor());
	}
}
