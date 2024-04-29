#include "pch.h"
#include "Engine/Devices/Monitor.h"

#include "Engine/Core/Window.h"

#include <GLFW/glfw3.h>

namespace Engine {
	std::vector<Scope<Monitor>> Monitor::s_Monitors;
	Monitor *Monitor::s_PrimaryMonitor;
	bool Monitor::s_Initialized = false;

	static Scope<VideoMode> CreateVideMode(GLFWvidmode mode) {
		auto result = MakeScope<VideoMode>();

		result->BlueBits = mode.blueBits;
		result->GreenBits = mode.greenBits;
		result->RedBits = mode.redBits;
		result->RefreshRate = mode.refreshRate;

		result->Size = Vector2(static_cast<Vector2u::ValueType>(mode.width), static_cast<Vector2u::ValueType>(mode.height));

		return result;
	}

	static std::vector<Scope<VideoMode>> CreateVideoModes(GLFWmonitor *monitor) {
		std::vector<Scope<VideoMode>> modes;

		int count = 0;
		const GLFWvidmode *vModes = glfwGetVideoModes(monitor, &count);

		ENGINE_ASSERT(count != 0);

		if (count == 0)
			throw std::runtime_error("Unable to get video modes for monitor");

		modes.reserve(static_cast<std::vector<VideoMode>::size_type>(count));

		for (int i = 0; i < count; ++i)
			modes.emplace_back(CreateVideMode(vModes[i]));

		return modes;
	}

	GammaRamp::~GammaRamp() {
		delete[] Red;
		delete[] Green;
		delete[] Blue;

		Red = Green = Blue = nullptr;
	}

	const std::vector<Scope<VideoMode>> &Monitor::GetVideoModes() const {
		return m_VideoModes;
	}

	void Monitor::SetUserData(void *userData) {
		m_UserData = userData;
		glfwSetMonitorUserPointer(static_cast<GLFWmonitor *>(m_NativePointer), m_UserData);
	}

	void Monitor::SetGamma(float gamma) {
		glfwSetGamma(static_cast<GLFWmonitor *>(m_NativePointer), gamma);
	}

	void Monitor::SetGammaRamp(const GammaRamp &ramp) {
		GLFWgammaramp gammaRamp;
		gammaRamp.size = ramp.Size;

		ENGINE_ASSERT(ramp.Blue && ramp.Green && ramp.Red);

		if (!ramp.Blue || !ramp.Green || !ramp.Red)
			return;

		gammaRamp.blue = ramp.Blue;
		gammaRamp.green = ramp.Green;
		gammaRamp.red = ramp.Red;

		glfwSetGammaRamp(static_cast<GLFWmonitor *>(m_NativePointer), &gammaRamp);
	}

	GammaRamp Monitor::GetGammaRamp() const {
		GammaRamp gammaRamp;
		const auto ramp = glfwGetGammaRamp(static_cast<GLFWmonitor *>(m_NativePointer));

		gammaRamp.Size = ramp->size;
		gammaRamp.Red = new uint16_t[ramp->size];
		gammaRamp.Green = new uint16_t[ramp->size];
		gammaRamp.Blue = new uint16_t[ramp->size];

		memcpy_s(gammaRamp.Red, gammaRamp.Size * sizeof(uint16_t), ramp->red, ramp->size * sizeof(decltype(ramp->red)));
		memcpy_s(gammaRamp.Green, gammaRamp.Size * sizeof(uint16_t), ramp->green, ramp->size * sizeof(decltype(ramp->green)));
		memcpy_s(gammaRamp.Blue, gammaRamp.Size * sizeof(uint16_t), ramp->blue, ramp->size * sizeof(decltype(ramp->blue)));

		return gammaRamp;
	}

	Monitor* Monitor::GetPrimary() {
		Populate();

		return s_PrimaryMonitor;
	}

	Monitor* Monitor::Get(const size_t monitor) {
		Populate();

		return s_Monitors[monitor].get();
	}

	const std::vector<Scope<Monitor>> &Monitor::GetAll() {
		Populate();

		return s_Monitors;
	}

	Scope<Monitor> Monitor::Create(void *pointer) {
		auto monitor = Scope<Monitor>(new Monitor());

		if (!pointer)
			return monitor;

		int x, y;
		glfwGetMonitorPhysicalSize(static_cast<GLFWmonitor *>(pointer), &x, &y);

		monitor->m_Size = Vector2u(static_cast<uint32_t>(x), static_cast<uint32_t>(y));

		glfwGetMonitorContentScale(static_cast<GLFWmonitor *>(pointer), &monitor->m_Scale.X, &monitor->m_Scale.Y);
		glfwGetMonitorPos(static_cast<GLFWmonitor *>(pointer), &monitor->m_Pos.X, &monitor->m_Pos.Y);
		glfwGetMonitorWorkarea(static_cast<GLFWmonitor *>(pointer), &monitor->m_WorkArea.X, &monitor->m_WorkArea.Y, &monitor->m_WorkArea.Width, &monitor->m_WorkArea.Height);

		monitor->m_Name = glfwGetMonitorName(static_cast<GLFWmonitor *>(pointer));
		monitor->m_UserData = glfwGetMonitorUserPointer(static_cast<GLFWmonitor *>(pointer));
		monitor->m_VideoModes = CreateVideoModes(static_cast<GLFWmonitor *>(pointer));
		monitor->m_NativePointer = pointer;

		return monitor;
	}

	void Monitor::Populate() {
		if (!s_Initialized) {
			Window::InitializeGlfw();
			s_Initialized = true;
		}
		else
			return;

		int count = 0;
		const auto monitors = glfwGetMonitors(&count);
		const auto PriMonitor = glfwGetPrimaryMonitor();

		for (int i = 0; i < count; ++i) {
			s_Monitors.emplace_back(Create(monitors[i]));
			if (monitors[i] == PriMonitor)
				s_PrimaryMonitor = s_Monitors[s_Monitors.size() - 1].get();
		}
	}


}