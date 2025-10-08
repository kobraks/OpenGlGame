#include "pch.h"
#include "MonitorRegistry.h"

#include "Engine/Core/Window.h"
#include "Engine/Devices/Monitor.h"
#include "Engine/Events/ApplicationEvent.h"

#include "GLFW/glfw3.h"

namespace Engine {
	MonitorRegistry& MonitorRegistry::Get() {
		static MonitorRegistry instance;
		return instance;
	}

	void MonitorRegistry::Initialize() {
		std::lock_guard guard(m_Mutex);
		if (m_Initialized)
			return;

		LOG_ENGINE_INFO("Initializing monitor registry");

		m_Initialized = true;
		Window::InitializeGlfw();
		RegisterCallbacks();

		int count = 0;
		const auto monitors = glfwGetMonitors(&count);
		const auto primaryMonitor = glfwGetPrimaryMonitor();
		LOG_ENGINE_INFO("Found {} monitors", count);

		for (int i = 0; i < count; ++i) {
			if (const auto monitor = AddNewMonitorUnlocked(monitors[i]); monitor->GetNativeHandle() == primaryMonitor) {
				m_Primary = monitor;
			}
		}
	}

	void MonitorRegistry::Shutdown() {
		std::lock_guard guard(m_Mutex);
		glfwSetMonitorCallback(nullptr);

		for (auto& monitor : m_Monitors) {
			monitor->Invalidate();
		}

		m_Monitors.clear();
		m_ByHandle.clear();
		m_Primary.reset();

		m_Initialized = false;
	}

	std::vector<Ref<Monitor>> MonitorRegistry::GetAll() const {
		std::shared_lock lock(m_Mutex);
		return m_Monitors;
	}

	Ref<Monitor> MonitorRegistry::GetPrimary() const {
		std::shared_lock lock(m_Mutex);
		return m_Primary;
	}

	Ref<Monitor> MonitorRegistry::Get(std::size_t index) const {
		std::shared_lock lock(m_Mutex);
		if (index >= m_Monitors.size())
			return nullptr;

		return m_Monitors[index];
	}

	std::size_t MonitorRegistry::Count() const {
		std::shared_lock lock(m_Mutex);
		return m_Monitors.size();
	}

	Ref<Monitor> MonitorRegistry::FindByHandle(const void* nativeHandle) const {
		std::shared_lock lock(m_Mutex);

		if (const auto it = m_ByHandle.find(nativeHandle); it != m_ByHandle.end()) {
			return it->second;
		}

		return nullptr;
	}

	void MonitorRegistry::Refresh() {
		Shutdown();
		Initialize();
	}

	void MonitorRegistry::OnMonitorConnect(void* monitor) {
		std::lock_guard guard(m_Mutex);
		const auto added = AddNewMonitorUnlocked(monitor);
		if (glfwGetPrimaryMonitor() == monitor) {
			m_Primary = added;
		}
	}

	void MonitorRegistry::OnMonitorDisconnect(void* monitor) {
		std::lock_guard guard(m_Mutex);
		RemoveMonitorUnlocked(monitor);
	}

	void MonitorRegistry::RegisterCallbacks() {
		glfwSetMonitorCallback([](GLFWmonitor* monitor, int event) {
			auto& registry = MonitorRegistry::Get();
			if (event == GLFW_CONNECTED) {
				registry.OnMonitorConnect(monitor);
				if (registry.m_EventCallback) {
					MonitorAddedEvent event(glfwGetMonitorName(monitor), registry.m_Monitors.size() - 1);
					registry.m_EventCallback(event);
				}
			}
			else if (event == GLFW_DISCONNECTED) {
				registry.OnMonitorDisconnect(monitor);
				if (registry.m_EventCallback) {
					MonitorRemovedEvent event(glfwGetMonitorName(monitor));
					registry.m_EventCallback(event);
				}
			}
			});
	}

	Ref<Monitor> MonitorRegistry::AddNewMonitorUnlocked(void* pointer) {
		const auto newMonitor = Monitor::Create(pointer);
		m_ByHandle.emplace(std::pair<const void*, Ref<Monitor>>(pointer, newMonitor));
		m_Monitors.push_back(newMonitor);

		if (const auto vm = newMonitor->GetVideoMode()) {
			LOG_ENGINE_INFO("Monitor connected: \"{}\", VideoMode: {:l}", newMonitor->GetName(), *newMonitor->GetVideoMode());
		} else {
			LOG_ENGINE_INFO("Monitor connected: \"{}\", VideoMode: <unknown>", newMonitor->GetName());
		}

		return newMonitor;
	}

	Ref<Monitor> MonitorRegistry::RemoveMonitorUnlocked(void* pointer) {
		auto it = m_ByHandle.find(pointer);
		if (it == m_ByHandle.end())
			return nullptr;;

		Ref<Monitor> victim = it->second;
		const bool wasPrimary = m_Primary && m_Primary->GetNativeHandle() == pointer;

		LOG_ENGINE_INFO("Monitor disconnected: \"{}\"", victim->GetName());

		victim->Invalidate();
		m_ByHandle.erase(it);

		const auto vit = std::ranges::find_if(m_Monitors, [&](const Ref<Monitor>& r) { return r == victim; });
		if (vit != m_Monitors.end())
			m_Monitors.erase(vit);

		if (wasPrimary) {
			const auto primary = glfwGetPrimaryMonitor();
			m_Primary.reset();
			if (auto itp = m_ByHandle.find(primary); itp != m_ByHandle.end()) {
				m_Primary = itp->second;
			}
		}

		return victim;
	}
}
