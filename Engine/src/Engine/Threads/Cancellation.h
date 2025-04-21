#pragma once
#include "Engine/Core/Base.h"

#include <atomic>
#include <memory>

namespace Engine {
	class CancellationToken {
	public:
		CancellationToken() = default;
		explicit CancellationToken(Ref<std::atomic_bool> flag);

		bool IsCancelled() const {
			return m_Flag && *m_Flag;
		}

		explicit operator bool() const { return m_Flag != nullptr; }

	private:
		Ref<std::atomic_bool> m_Flag;
		friend class CancellationSource;
	};

	class CancellationSource {
	public:
		CancellationSource();
		CancellationToken GetToken() const {
			return CancellationToken(m_Flag);
		}

		void Cancel() const {
			*m_Flag = true;
		}

	private:
		Ref<std::atomic_bool> m_Flag;
	};
}