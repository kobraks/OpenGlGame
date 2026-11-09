#pragma once

namespace Engine::Utils {
	class FreeImageRuntime {
	public:
		FreeImageRuntime();
		~FreeImageRuntime();

		FreeImageRuntime(const FreeImageRuntime&) = delete;
		FreeImageRuntime& operator=(const FreeImageRuntime&) = delete;
		
		FreeImageRuntime(FreeImageRuntime&&) = delete;
		FreeImageRuntime& operator=(FreeImageRuntime&&) = delete;
	};
}