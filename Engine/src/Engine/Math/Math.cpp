#include "pch.h"
#include "Engine/Math/Math.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace Game::Math
{
	bool DecomposeTransform(const glm::mat4 &transform, glm::vec3 &translation, glm::vec3 &rotation, glm::vec3 &scale)
	{
		using namespace glm;
		using T = float;

		mat4 localMatrix(transform);

		if(epsilonEqual(localMatrix[3][3], 0.f, epsilon<T>()))
			return false;

		if (epsilonNotEqual(localMatrix[0][3], 0.f, epsilon<T>()) ||
			epsilonNotEqual(localMatrix[1][3], 0.f, epsilon<T>()) || 
			epsilonNotEqual(localMatrix[2][3], 0.f, epsilon<T>()))
		{
			localMatrix[0][3] = localMatrix[1][3] = localMatrix[2][3] = 0.f;
			localMatrix[3][3] = 1.f;
		}

		translation = vec3(localMatrix[3]);
		localMatrix[3] = vec4(0, 0, 0, localMatrix[3].w);

		vec3 row[3], pdum3;

		for (length_t i = 0; i < 3; ++i)
			for (length_t j = 0; j < 3; ++j)
				row[i][j] = localMatrix[i][j];

		scale.x = length(row[0]);
		row[0] = detail::scale(row[0], 1.f);
		scale.y = length(row[1]);
		row[1] = detail::scale(row[1], 1.f);
		scale.z = length(row[2]);
		row[2] = detail::scale(row[2], 1.f);

#if 0
		pdum3 = cross(row[1], row[2]);
		if (dot(row[0], pdum3) < 0)
		{
			for (length_t i = 0; i < 3; ++i)
			{
				scale[i] *= -1.f;
				row[i] *= -1.f;
			}
		}
#endif

		rotation.y = asin(-row[0][2]);
		if (cos(rotation.y) != 0)
		{
			rotation.x = atan2(row[1][2], row[2][2]);
			rotation.z = atan2(row[0][1], row[0][0]);
		}
		else
		{
			rotation.x = atan2(-row[2][0], row[1][1]);
			rotation.z = 0.f;
		}

		return true;
	}

}