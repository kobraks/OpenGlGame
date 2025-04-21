#include "pch.h"
#include "Cancellation.h"

namespace Engine {
	CancellationToken::CancellationToken(Ref<std::atomic_bool> flag) : m_Flag(flag) {
		
	}

	CancellationSource::CancellationSource() : m_Flag(MakeRef<std::atomic_bool>(false)){
	}
}
