#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/Time.h"
#include "Engine/Threads/Cancellation.h"

#include <functional>

namespace Engine {
	enum class TaskPriority : int32_t {
		Urgent = 0,
		High = 1,
		Normal = 2,
		Low = 3
	};

	enum class TaskTag {
		None,
		Render,
		Physic,
		IO
	};

	struct Task {
		TaskPriority Priority = TaskPriority::Normal;
		TaskTag Tag = TaskTag::None;

		std::optional<uint32_t> ThreadAffinity = std::nullopt;
		CancellationToken Token;

		std::move_only_function<void()> Job;

		bool operator<(const Task& other) const {
			return static_cast<int32_t>(Priority) < static_cast<int32_t>(other.Priority);
		}

		bool operator>(const Task& other) const {
			return static_cast<int32_t>(Priority) > static_cast<int32_t>(other.Priority);
		}

	};
}