#pragma once
#include "Buffer.h"
#include "Light.h"

namespace Game
{
	class LightBuffer: protected UniformBuffer
	{
	public:
		constexpr static uint32_t MAX_LIGHTS = 10;
		
	private:
		std::array<Light, MAX_LIGHTS> m_Lights;

	public:
		LightBuffer();
		
		void SetLight(uint32_t index, const Light& light);
		Light GetLight(uint32_t index);

		void Bind() const;
		void Unbind() const;

	private:
		void SetLight(uint32_t index, const LightInfo& light);
	};
}
