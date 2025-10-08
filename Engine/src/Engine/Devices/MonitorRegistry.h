#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Events/Event.h"

#include <vector>
#include <unordered_map>
#include <shared_mutex>

namespace Engine {
	class Monitor;
	class Window;

	class MonitorRegistry {
	public:
		using EventCallbackFunction = std::move_only_function<void(Event&)>;

		static MonitorRegistry& Get();

		void Initialize();
		void Shutdown();

		[[nodiscard]] std::vector<Ref<Monitor>> GetAll() const;
		[[nodiscard]] Ref<Monitor> GetPrimary() const;
		[[nodiscard]] Ref<Monitor> Get(std::size_t index) const;
		[[nodiscard]] std::size_t Count() const;

		[[nodiscard]] Ref<Monitor> FindByHandle(const void* nativeHandle) const;

		void SetEventCallback(EventCallbackFunction&& callback) { m_EventCallback = std::move(callback); }

		void Refresh();
	protected:
		void RegisterCallbacks();
		Ref<Monitor> AddNewMonitorUnlocked(void* pointer);
		Ref<Monitor> RemoveMonitorUnlocked(void* pointer);

	private:
		bool m_Initialized = false;

		mutable std::shared_mutex m_Mutex;

		std::vector<Ref<Monitor>> m_Monitors;
		std::unordered_map<const void*, Ref<Monitor>> m_ByHandle;

		Ref<Monitor> m_Primary;
		EventCallbackFunction m_EventCallback;
	};
}