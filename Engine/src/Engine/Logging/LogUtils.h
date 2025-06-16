#pragma once
#include "Engine/Core/Base.h"

#include "Engine/Core/Color.h"

#include <string_view>
#include <ImGui/imgui.h>

namespace Engine::Utils {
	static constexpr Color SelectTextColor(spdlog::level::level_enum level) {
		switch (level) {
		case spdlog::level::trace:
			return Color(204, 204, 204);
		case spdlog::level::debug:
			return Color(58, 150, 221);
		case spdlog::level::info:
			return Color(19, 161, 14);
		case spdlog::level::warn:
			return Color(249, 241, 165);
		case spdlog::level::err:;
		case spdlog::level::critical:
			return Color(255, 0, 0);
		case spdlog::level::off:
		case spdlog::level::n_levels: default:
			return Color(0, 0, 0, 0);
		}
	}

	static constexpr Color SelectBackgroundColor(spdlog::level::level_enum level) {
		switch (level) {
		case spdlog::level::trace:
			return Color(204, 204, 204);
		case spdlog::level::debug:
			return Color(58, 150, 221);
		case spdlog::level::info:
			return Color(19, 161, 14);
		case spdlog::level::warn:
			return Color(249, 241, 165);
		case spdlog::level::err:;
		case spdlog::level::critical:
			return Color(255, 0, 0);
		case spdlog::level::off:
		case spdlog::level::n_levels: default:
			return Color(0, 0, 0, 0);
		}
	}

	const std::array<std::string_view, 6>& GetSeverities();
	const std::array<std::string_view, 4>& GetKnownLoggersName();

	int32_t GetLoggerID(std::string_view loggerName);

	std::string GetTimeAsString(const spdlog::log_clock::time_point& time);

	bool FilterChanged(const std::string& filter, std::string& lastFilter);
	bool FilterChanged(int32_t filter, int32_t& lastFilter);
	bool FilterChanged(const ImGuiTextFilter& filter, std::string& lastFilter);
}