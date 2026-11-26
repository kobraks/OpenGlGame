#pragma once
#include "Engine/Core/base.h"

#include "Engine/Serializer/StreamWriter.h"
#include "Engine/Serializer/StreamReader.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Engine {
	struct GLMSerializer {
		template<glm::length_t L, class T, glm::qualifier Q>
		static bool Serialize(StreamWriter& writer, const glm::vec<L,T,Q>& vec) {
			return writer.WriteData(glm::value_ptr(vec), L);
		}

		template<glm::length_t L, class T, glm::qualifier Q>
		static bool Deserialize(StreamReader& reader, glm::vec<L,T,Q>& vec) {
			glm::vec<L, T, Q> temp{};

			if (!reader.ReadData(glm::value_ptr(temp), sizeof(temp)))
				return false;
			
			vec = temp;
			return true;
		}

		template<glm::length_t Cols, glm::length_t Rows, class T, glm::qualifier Q>
		static bool Serialize(StreamWriter& writer, const glm::mat<Cols, Rows, T, Q>& mat) {
			return writer.WriteData(glm::value_ptr(mat), Cols * Rows);
		}

		template<glm::length_t Cols, glm::length_t Rows, class T, glm::qualifier Q>
		static bool Deserialize(StreamReader& reader, glm::mat<Cols, Rows, T, Q>& mat) {
			glm::mat<Cols, Rows, T, Q> temp{};

			if (!reader.ReadData(glm::value_ptr(temp), sizeof(temp)))
				return false;

			mat = temp;
			return true;
		}

		template<class T, glm::qualifier Q>
		static bool Serialize(StreamWriter& writer, const glm::quat<T, Q>& qat) {
			return writer.WriteData(glm::value_ptr(qat), 4);
		}

		template<class T, glm::qualifier Q>
		static bool Deserialize(StreamReader& reader, glm::quat<T, Q>& qat) {
			glm::quat<T, Q> temp{};

			if (!reader.ReadData(glm::value_ptr(temp), sizeof(temp)))
				return false;

			qat = temp;
			return true;
		}
	};

	template<glm::length_t L, class T, glm::qualifier Q>
	struct Serializer<glm::vec<L,T,Q>> { using Type = GLMSerializer; };

	template<glm::length_t C, glm::length_t R, class T, glm::qualifier Q>
	struct Serializer<glm::mat<C,R,T,Q>> { using Type = GLMSerializer; };

	template<class T, glm::qualifier Q>
	struct Serializer<glm::qua<T, Q>> { using Type = GLMSerializer; };
}