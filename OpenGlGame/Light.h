#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Game
{
	constexpr uint32_t MAX_LIGHTS = 10;
	
	enum class LightType : int32_t
	{
		Unknown = 0,
		Spot = 1,
		Point = 2,
		Directional = 3
	};
	
	struct LightInfo
	{
		uint32_t Index = 0;
		
		glm::vec3 Position = glm::vec3(0.f);
		glm::vec3 Direction = glm::vec3(0.f, 0.f, 1.f);

		glm::vec4 AmbientColor = glm::vec4(1.f);
		glm::vec4 DiffuseColor = glm::vec4(1.f);
		glm::vec4 SpecularColor = glm::vec4(1.f);

		LightType Type = LightType::Unknown;
		
		bool Active = false;

		float CutOff = 0.f;
		float OuterCutOff = 0.f;

		float Constant = 0.f;
		float Linear = 0.f;
		float Quadratic = 0.f;
	};

	class Light
	{
	public:
		Light() = default;
		virtual ~Light() = default;

		virtual const LightInfo &GetInfo() const = 0;
	};
}
