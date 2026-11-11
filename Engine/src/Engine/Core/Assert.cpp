#include "pch.h"

#include "Assert.h"

namespace Engine::Detail {
	[[nodiscard]] constexpr std::string_view GetFileName(std::string_view path) noexcept {
		const auto separatorPosition = path.find_last_of("/\\");
		return separatorPosition == std::string_view::npos ? path : path.substr(separatorPosition + 1);
	}

	void WriteAssertionFailure(const Ref<spdlog::logger>& logger, std::string_view condition,
	                           std::source_location location, std::string_view additionalMessage) {
		const std::string message = additionalMessage.empty()
			                            ? fmt::format("Assertion '{}' failed at {}:{}", condition,
			                                          GetFileName(location.file_name()), location.line())
			                            : fmt::format("Assertion '{}' failed at {}:{}: {}", condition,
			                                          GetFileName(location.file_name()), location.line(),
			                                          additionalMessage);

		if (logger) {
			logger->log(spdlog::source_loc{
				            location.file_name(), static_cast<int>(location.line()), location.function_name()
			            }, spdlog::level::err, "{}", message);
		}
		else {
			std::fputs(message.c_str(), stderr);
			std::fputs("\n", stderr);
		}
	}
}
