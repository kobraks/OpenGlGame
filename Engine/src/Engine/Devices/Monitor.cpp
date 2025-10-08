#include "pch.h"
#include "Engine/Devices/Monitor.h"

#include "Engine/Devices/MonitorRegistry.h"

#include <GLFW/glfw3.h>
#include <glm/common.hpp>

#include <set>

namespace Engine {
	namespace Utils {
		static bool Equal(GLFWvidmode a, VideoMode b) {
			return a.width == static_cast<int>(b.Size.X)
				&& a.height == static_cast<int>(b.Size.Y)
				&& a.redBits == b.RedBits
				&& a.greenBits == b.GreenBits
				&& a.blueBits == b.BlueBits
				&& a.refreshRate == b.RefreshRate;
		}

		static Ref<VideoMode> CreateVideoMode(GLFWvidmode mode) {
			auto result = MakeRef<VideoMode>();

			result->BlueBits = mode.blueBits;
			result->GreenBits = mode.greenBits;
			result->RedBits = mode.redBits;
			result->RefreshRate = mode.refreshRate;

			result->Size = Vector2u(static_cast<Vector2u::ValueType>(mode.width),
				static_cast<Vector2u::ValueType>(mode.height));

			LOG_ENGINE_DEBUG("Created video mode: {:l}", *result);

			return result;
		}

		static std::pair<std::vector<Ref<VideoMode>>, Ref<VideoMode>> CreateVideoModes(GLFWmonitor* monitor) {
			std::vector<Ref<VideoMode>> modes;
			Ref<VideoMode> current = nullptr;

			int count = 0;
			const GLFWvidmode* vModes = glfwGetVideoModes(monitor, &count);
			const GLFWvidmode* currentMode = glfwGetVideoMode(monitor);

			ENGINE_ASSERT(count != 0);

			if (count == 0)
				throw std::runtime_error("Unable to get video modes for monitor");

			modes.reserve(static_cast<std::vector<VideoMode>::size_type>(count));

			for (int i = 0; i < count; ++i) {
				modes.emplace_back(CreateVideoMode(vModes[i]));

				if (Equal(*currentMode, *modes[i]))
					current = modes[i];

			}

			return { std::move(modes), current };
		}
	}

	GammaRamp::GammaRamp(uint32_t size) {
		Size = size;

		Red = MakeRef<uint16_t[]>(size);
		Green = MakeRef<uint16_t[]>(size);
		Blue = MakeRef<uint16_t[]>(size);
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

	const std::vector<Ref<VideoMode>>& Monitor::GetVideoModes() const {
		return m_VideoModes;
	}

	Ref<VideoMode> Monitor::FindClosestMode(const Vector2u& size, int refreshRate) const {
		Ref<VideoMode> best = nullptr;
		uint32_t bestScore = UINT32_MAX;

		for (const auto& mode : m_VideoModes) {
			const uint32_t score = glm::abs(static_cast<int32_t>(mode->Size.X) - static_cast<int32_t>(size.X))
				+ glm::abs(static_cast<int32_t>(mode->Size.Y) - static_cast<int32_t>(size.Y))
				+ glm::abs(mode->RefreshRate - refreshRate) * 2;

			if (score < bestScore) {
				best = mode;
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

		gammaRamp.blue = ramp.Blue.get();
		gammaRamp.green = ramp.Green.get();
		gammaRamp.red = ramp.Red.get();

		glfwSetGammaRamp(static_cast<GLFWmonitor*>(m_NativePointer), &gammaRamp);
	}

	GammaRamp Monitor::GetGammaRamp() const {
		const auto ramp = glfwGetGammaRamp(static_cast<GLFWmonitor*>(m_NativePointer));

		if (!ramp || ramp->size == 0) return{};

		GammaRamp gammaRamp(ramp->size);

		std::memcpy(gammaRamp.Red.get(), ramp->red, ramp->size * sizeof(uint16_t));
		std::memcpy(gammaRamp.Green.get(), ramp->green, ramp->size * sizeof(uint16_t));
		std::memcpy(gammaRamp.Blue.get(), ramp->blue, ramp->size * sizeof(uint16_t));

		return gammaRamp;
	}

	bool Monitor::IsPrimary() const {
		return MonitorRegistry::Get().GetPrimary().get() == this;
	}

	void Monitor::Invalidate() {
		m_NativePointer = nullptr;
		m_UserData = nullptr;
	}

	Ref<Monitor> Monitor::Create(void* pointer) {
		auto newMonitor = Ref<Monitor>(new Monitor());

		if (!pointer)
			return newMonitor;

		GLFWmonitor* glfwMonitor = static_cast<GLFWmonitor*>(pointer);

		int x, y;
		glfwGetMonitorPhysicalSize(glfwMonitor, &x, &y);

		newMonitor->m_Size = Vector2u(static_cast<uint32_t>(x), static_cast<uint32_t>(y));

		glfwGetMonitorContentScale(glfwMonitor, &newMonitor->m_Scale.X, &newMonitor->m_Scale.Y);
		glfwGetMonitorPos(glfwMonitor, &newMonitor->m_Pos.X, &newMonitor->m_Pos.Y);
		glfwGetMonitorWorkarea(glfwMonitor, &newMonitor->m_WorkArea.X, &newMonitor->m_WorkArea.Y,
		                       &newMonitor->m_WorkArea.Width, &newMonitor->m_WorkArea.Height);

		newMonitor->m_Name = glfwGetMonitorName(glfwMonitor);
		newMonitor->m_UserData = glfwGetMonitorUserPointer(glfwMonitor);
		newMonitor->m_NativePointer = pointer;

		auto [modes, current] = Utils::CreateVideoModes(glfwMonitor);
		newMonitor->m_VideoMode = current;
		newMonitor->m_VideoModes = std::move(modes);

		return newMonitor;
	}

	/*void Monitor::RegisterCallbacks() {
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
	} */
}
