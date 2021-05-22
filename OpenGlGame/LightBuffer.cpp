#include "pch.h"
#include "LightBuffer.h"

namespace Game
{
	/*
	 * struct Light //TODO -Reorder stuff for better memory usage
	 * {
	 *	 bool Active; // Offset: 0; Size: 4
	 *	 int Type; //Offset: 4; Size: 4
	 *
	 *	 vec3 Position;// Offset: 16 (because ?) Size: 12, Padding 4
	 *	 vec3 Direction;// Offset: 32, Size: 12, Padding 4
	 *
	 *	 vec3 Diffuse;// Offset: 48, Size: 12, Padding 4
	 *	 vec3 Ambient;// Offset: 64, Size: 12, Padding 4
	 *	 vec3 Specular;// Offset: 80, Size: 12, Padding 0 // For some reason
	 *
	 *	 float CutOff;// Offset 92, Size 4,
	 *	 float OuterCutOff;// Offset 96, Size 4,
	 *
	 *	 float Constant;// Offset 100, Size 4
	 *	 float Linear;// Offset 104, Size 4
	 *	 float Quadratic;// Offset 108, Size 4
	 *
	 *	 bool HasLightCookie;// Offset 112, Size 4
	 *	 //Padding 12
	 * };
	 */

	template <typename T>
	constexpr size_t SizeOfType()
	{
		size_t i    = 0;
		size_t size = 0;

		while(size < sizeof(T))
		{
			size += sizeof(float);
			i += 1;
		}

		if(i > 1 && i % 2 != 0)
			size += sizeof(float);

		return size;
	}

	constexpr size_t SizeOfStruct(size_t baseSize)
	{
		size_t sum = 0;
		while(sum <= baseSize)
			sum += sizeof(float) * 4;

		return sum;
	}

	template <typename T>
	void SetElement(char *buff, const T &value, size_t &offset)
	{
		std::memcpy(buff + offset, &value, sizeof(T));

		offset += SizeOfType<T>();
	}


	constexpr size_t NumOfElements = 8 + 5 * 4;
	constexpr size_t StructSize    = SizeOfStruct(sizeof(float) * NumOfElements);


	LightBuffer::LightBuffer() : UniformBuffer(SizeOfStruct(sizeof(float) * (7 + 5 * 4)) * MAX_LIGHTS, BufferUsage::DynamicDraw)
	{
		for(auto &light : m_Lights)
			light.SetActive(false);
	}

	void LightBuffer::SetLight(uint32_t index, const Light &light)
	{
		SetLight(index, light);

		m_Lights[index] = light;
	}

	Light LightBuffer::GetLight(uint32_t index)
	{
		ASSERT(index < m_Lights.size(), "Light out of range");

		if(index >= m_Lights.size())
			throw std::out_of_range("Light out of range");
		
		return m_Lights[index];
	}

	void LightBuffer::Bind() const
	{
		UniformBuffer::Bind();
	}

	void LightBuffer::Unbind() const
	{
		UniformBuffer::Unbind();
	}

	void LightBuffer::SetLight(uint32_t index, const LightInfo &light)
	{
		ASSERT(index < m_Lights.size(), "Binding light out of range");

		if(index >= m_Lights.size())
			throw std::out_of_range("Light out of range");

		constexpr size_t size = StructSize;
		char buff[size];

		size_t offset = 0;
		SetElement(buff, light.Active, offset);
		SetElement(buff, light.Type, offset);

		offset += 16;

		SetElement(buff, light.Position, offset);
		SetElement(buff, light.Direction, offset);

		SetElement(buff, light.DiffuseColor, offset);
		SetElement(buff, light.AmbientColor, offset);
		SetElement(buff, light.SpecularColor, offset);

		offset -= 4;
		SetElement(buff, light.CutOff, offset);
		SetElement(buff, light.OuterCutOff, offset);

		SetElement(buff, light.Constant, offset);
		SetElement(buff, light.Linear, offset);
		SetElement(buff, light.Quadratic, offset);

		if(light.CookieTexture)
			SetElement(buff, true, offset);
		else
			SetElement(buff, false, offset);

		Set(buff, size, size * index);
	}
}
