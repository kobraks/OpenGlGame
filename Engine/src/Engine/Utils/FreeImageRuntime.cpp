#include "pch.h"
#include "FreeImageRuntime.h"

#include <FreeImage.h>

namespace Engine::Utils {
	FreeImageRuntime::FreeImageRuntime() {
		// Initialize FreeImage library
		FreeImage_Initialise(TRUE);
	}
	FreeImageRuntime::~FreeImageRuntime() {
		// Deinitialize FreeImage library
		FreeImage_DeInitialise();
	}
}