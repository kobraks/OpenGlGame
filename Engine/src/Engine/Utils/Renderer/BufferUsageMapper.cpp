#include "pch.h"
#include "BufferUsageMapper.h"

namespace Engine {
	BufferUsage BufferUsageStrategy(const BufferUsageIntent& i) {
		if (i.cpuWrite && i.gpuReads) {
			if (i.isStreaming) return BufferUsage::StreamDraw;
			if (i.isDynamic) return BufferUsage::DynamicDraw;
			return BufferUsage::StaticDraw;
		}

		if (i.gpuWrite && i.cpuReads) {
			if (i.isStreaming) return BufferUsage::StreamRead;
			if (i.isDynamic) return BufferUsage::DynamicRead;
			return BufferUsage::StaticRead;
		}

		if (i.gpuWrite && i.gpuWrite) {
			if (i.isStreaming) return BufferUsage::StreamCopy;
			if (i.isDynamic) return BufferUsage::DynamicCopy;
			return BufferUsage::StaticCopy;
		}

		return BufferUsage::StaticDraw;
	}
}

namespace Engine::Utils {
	
}