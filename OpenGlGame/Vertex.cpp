#include "pch.h"
#include "Vertex.h"

#include "Buffer.h"

#include <vector>

namespace Game
{
	BufferLayout CreateBufferLayout()
	{
		std::vector<BufferElement> buffer;
		buffer.resize(4);

		buffer[0] = BufferElement(ShaderDataType::Float3, "Position", false);
		buffer[1] = BufferElement(ShaderDataType::Float2, "TexCoords", false);
		buffer[2] = BufferElement(ShaderDataType::Float4, "Color", false);
		buffer[3] = BufferElement(ShaderDataType::Float3, "Normal", false);

		return BufferLayout(buffer);
	}
	
	BufferLayout Vertex::GetLayout()
	{
		static BufferLayout layout = CreateBufferLayout();
		return layout;
	}
}
