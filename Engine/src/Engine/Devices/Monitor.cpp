#include "pch.h"
#include "Engine/Devices/Monitor.h"

#include "Engine/Core/Window.h"
#include "Engine/Core/Application.h"

#include <GLFW/glfw3.h>
#include <glm/common.hpp>

#include <set>

#include "Engine/Events/ApplicationEvent.h"

namespace Engine {
	std::vector<Scope<Monitor>> Monitor::s_Monitors;
	Monitor* Monitor::s_PrimaryMonitor;
	bool Monitor::s_Initialized = false;
	bool Monitor::s_RegisteredCallbacks = false;

	static Scope<VideoMode> CreateVideoMode(GLFWvidmode mode) {
		auto result = MakeScope<VideoMode>();

		result->BlueBits = mode.blueBits;
		result->GreenBits = mode.greenBits;
		result->RedBits = mode.redBits;
		result->RefreshRate = mode.refreshRate;

		result->Size = Vector2(static_cast<Vector2u::ValueType>(mode.width),
		                       static_cast<Vector2u::ValueType>(mode.height));

		return result;
	}

	static std::vector<Scope<VideoMode>> CreateVideoModes(GLFWmonitor* monitor) {
		std::vector<Scope<VideoMode>> modes;

		int count = 0;
		const GLFWvidmode* vModes = glfwGetVideoModes(monitor, &count);

		ENGINE_ASSERT(count != 0);

		if (count == 0)
			throw std::runtime_error("Unable to get video modes for monitor");

		modes.reserve(static_cast<std::vector<VideoMode>::size_type>(count));

		for (int i = 0; i < count; ++i)
			modes.emplace_back(CreateVideoMode(vModes[i]));

		return modes;
	}

	GammaRamp::~GammaRamp() {
		delete[] Red;
		delete[] Green;
		delete[] Blue;

		Red = Green = Blue = nullptr;
	}

	Vector2f Monitor::GetDPI() const {
		return Vector2f{
			static_cast<float>(m_Size.X) / static_cast<float>(m_WorkArea.Height),
			static_cast<float>(m_Size.Y) / static_cast<float>(m_WorkArea.Width)
		};
	}

	Vector2f Monitor::GetContentScale() const {
		float xScale, yScale;
		glfwGetMonitorContentScale(GetNativeHandle<GLFWmonitor>(), &xScale, &yScale);
		return Vector2f{xScale, yScale};
	}

	const std::vector<Scope<VideoMode>>& Monitor::GetVideoModes() const {
		return m_VideoModes;
	}

	const VideoMode* Monitor::FindClosestMode(const Vector2u& size, int refreshRate) const {
		const VideoMode* best = nullptr;
		uint32_t bestScore = UINT32_MAX;

		for (const auto& mode : m_VideoModes) {
			const uint32_t score = glm::abs(static_cast<int32_t>(mode->Size.X) - static_cast<int32_t>(size.X))
				+ glm::abs(static_cast<int32_t>(mode->Size.Y) - static_cast<int32_t>(size.Y))
				+ glm::abs(mode->RefreshRate - refreshRate) * 2;

			if (score < bestScore) {
				best = mode.get();
				bestScore = score;
			}
		}

		return best;
	}

	std::vector<int32_t> Monitor::GetAvailableRefreshRates(const Vector2u& resolution) const {
		std::set<int32_t> rates;
		for (const auto& mode : m_VideoModes) {
			if (mode->Size == resolution)
				rates.insert(mode->RefreshRate);
		}

		return std::vector<int32_t>(rates.begin(), rates.end());
	}

	void Monitor::SetUserData(void* userData) {
		m_UserData = userData;
		glfwSetMonitorUserPointer(static_cast<GLFWmonitor*>(m_NativePointer), m_UserData);
	}

	void Monitor::SetGamma(float gamma) {
		glfwSetGamma(static_cast<GLFWmonitor*>(m_NativePointer), gamma);
	}

	void Monitor::SetGammaRamp(const GammaRamp& ramp) {
		GLFWgammaramp gammaRamp;
		gammaRamp.size = ramp.Size;

		ENGINE_ASSERT(ramp.Blue && ramp.Green && ramp.Red);

		if (!ramp.Blue || !ramp.Green || !ramp.Red)
			return;

		gammaRamp.blue = ramp.Blue;
		gammaRamp.green = ramp.Green;
		gammaRamp.red = ramp.Red;

		glfwSetGammaRamp(static_cast<GLFWmonitor*>(m_NativePointer), &gammaRamp);
	}

	GammaRamp Monitor::GetGammaRamp() const {
		GammaRamp gammaRamp;
		const auto ramp = glfwGetGammaRamp(static_cast<GLFWmonitor*>(m_NativePointer));

		gammaRamp.Size = ramp->size;
		gammaRamp.Red = new uint16_t[ramp->size];
		gammaRamp.Green = new uint16_t[ramp->size];
		gammaRamp.Blue = new uint16_t[ramp->size];

		memcpy_s(gammaRamp.Red, gammaRamp.Size * sizeof(uint16_t), ramp->red, ramp->size * sizeof(decltype(ramp->red)));
		memcpy_s(gammaRamp.Green, gammaRamp.Size * sizeof(uint16_t), ramp->green,
		         ramp->size * sizeof(decltype(ramp->green)));
		memcpy_s(gammaRamp.Blue, gammaRamp.Size * sizeof(uint16_t), ramp->blue,
		         ramp->size * sizeof(decltype(ramp->blue)));

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

	const std::vector<Scope<Monitor>>& Monitor::GetAll() {
		Populate();

		return s_Monitors;
	}

	Scope<Monitor> Monitor::Create(void* pointer) {
		auto monitor = Scope<Monitor>(new Monitor());

		if (!pointer)
			return monitor;

		int x, y;
		glfwGetMonitorPhysicalSize(static_cast<GLFWmonitor*>(pointer), &x, &y);

		monitor->m_Size = Vector2u(static_cast<uint32_t>(x), static_cast<uint32_t>(y));

		glfwGetMonitorContentScale(static_cast<GLFWmonitor*>(pointer), &monitor->m_Scale.X, &monitor->m_Scale.Y);
		glfwGetMonitorPos(static_cast<GLFWmonitor*>(pointer), &monitor->m_Pos.X, &monitor->m_Pos.Y);
		glfwGetMonitorWorkarea(static_cast<GLFWmonitor*>(pointer), &monitor->m_WorkArea.X, &monitor->m_WorkArea.Y,
		                       &monitor->m_WorkArea.Width, &monitor->m_WorkArea.Height);

		monitor->m_Name = glfwGetMonitorName(static_cast<GLFWmonitor*>(pointer));
		monitor->m_UserData = glfwGetMonitorUserPointer(static_cast<GLFWmonitor*>(pointer));
		monitor->m_VideoModes = CreateVideoModes(static_cast<GLFWmonitor*>(pointer));
		monitor->m_NativePointer = pointer;

		return monitor;
	}

	void Monitor::Populate() {
		if (s_Initialized)
			return;

		s_Initialized = true;
		Window::InitializeGlfw();
		RegisterCallbacks();

		int count = 0;
		const auto monitors = glfwGetMonitors(&count);
		const auto primaryMonitor = glfwGetPrimaryMonitor();

		for (int i = 0; i < count; ++i) {
			AddNewMonitor(monitors[i]);
		}
	}

	void Monitor::RegisterCallbacks() {
		if (s_RegisteredCallbacks)
			return;

		s_RegisteredCallbacks = true;

		glfwSetMonitorCallback([](GLFWmonitor* monitor, int event) {
			if (event == GLFW_CONNECTED) {
				auto newMonitor = AddNewMonitor(monitor);

				MonitorAddedEvent event(newMonitor->m_Name, s_Monitors.size() - 1);
				Application::Get().OnEvent(event); //TODO Crude fix think on something better
			}
			else if (event == GLFW_DISCONNECTED) {
				MonitorRemovedEvent event(glfwGetMonitorName(monitor));
				Application::Get().OnEvent(event); //TODO Crude fix think on something better
			}
			});
	}

	void Monitor::Refresh() {
		s_Monitors.clear();
		s_Initialized = false;
		Populate();
	}

	Monitor* Monitor::AddNewMonitor(void* pointer) {
		const auto primaryMonitor = glfwGetPrimaryMonitor();
		const auto monitor = s_Monitors.emplace_back(Create(pointer)).get();

		if (pointer == primaryMonitor)
			s_PrimaryMonitor = monitor;

		return monitor;
	}

	void Monitor::RemoveMonitor(void* pointer) {
		auto it = std::ranges::find_if(s_Monitors, [=](const Scope<Monitor>& monitor) { return monitor->GetNativeHandle() == pointer; });

		if (it != s_Monitors.end()) {
			auto monitor = std::move(*it);
			s_Monitors.erase(it);

			if (s_PrimaryMonitor->GetNativeHandle() == pointer) {
				const auto primaryMonitor = glfwGetPrimaryMonitor();

				for (const auto& monitor : s_Monitors) {
					if (monitor->GetNativeHandle<GLFWmonitor>() == primaryMonitor) {
						s_PrimaryMonitor = monitor.get();
						break;
					}
				}
			}
		}
	}
}
