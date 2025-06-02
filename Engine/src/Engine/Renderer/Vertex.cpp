#include "pch.h"
#include "Vertex.h"

#include "Engine/Renderer/BufferLayout.h"

namespace Engine {
	BufferLayout CreateBufferLayout() {
		std::vector<BufferElement> buffer;
		buffer.resize(6);

		buffer[0] = BufferElement(ShaderDataType::Float3, "Position", false);
		buffer[1] = BufferElement(ShaderDataType::Float2, "TexCoords", false);
		buffer[2] = BufferElement(ShaderDataType::Float4, "Color", false);

		buffer[3] = BufferElement(ShaderDataType::Float3, "Normal", false);
		buffer[4] = BufferElement(ShaderDataType::Float3, "Tangent", false);
		buffer[5] = BufferElement(ShaderDataType::Float3, "Bitangent", false);

		return buffer;
	}

	BufferLayout Vertex::GetLayout() {
		static BufferLayout layout = CreateBufferLayout();
		return layout;
	}

}