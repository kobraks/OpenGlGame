#pragma once

#include "Engine/Renderer/Geometry/Vertex.h"
#include "Engine/Serializer/Serializers/GLMSerializer.h"

namespace Engine {
	struct VertexSerializer {
		static bool Serialize(StreamWriter& writer, const Vertex& vertex) {
			return GLMSerializer::Serialize(writer, vertex.Position) &&
				GLMSerializer::Serialize(writer, vertex.TexCoords) &&
				GLMSerializer::Serialize(writer, vertex.Color) &&
				GLMSerializer::Serialize(writer, vertex.Normal) &&
				GLMSerializer::Serialize(writer, vertex.Tangent) &&
				GLMSerializer::Serialize(writer, vertex.Bitangent);
		}

		static bool Deserialize(StreamReader& reader, Vertex& vertex) {
			Vertex temp{};

			if (!GLMSerializer::Deserialize(reader, temp.Position) ||
				!GLMSerializer::Deserialize(reader, temp.TexCoords) ||
				!GLMSerializer::Deserialize(reader, temp.Color) ||
				!GLMSerializer::Deserialize(reader, temp.Normal) ||
				!GLMSerializer::Deserialize(reader, temp.Tangent) ||
				!GLMSerializer::Deserialize(reader, temp.Bitangent))
				return false;

			vertex = std::move(temp);
			return true;
		}
	};

	template<> struct Serializer<Vertex> { using Type = VertexSerializer; };
}