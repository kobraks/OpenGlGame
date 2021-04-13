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
		LightType Type = LightType::Unknown;

		glm::vec3 Position  = glm::vec3(0.f);
		glm::vec3 Direction = glm::vec3(0.f, 0.f, 1.f);

		glm::vec3 AmbientColor  = glm::vec3(1.f);
		glm::vec3 DiffuseColor  = glm::vec3(1.f);
		glm::vec3 SpecularColor = glm::vec3(1.f);

		bool Active = false;

		float CutOff      = 0.f;
		float OuterCutOff = 0.f;

		float Constant  = 0.f;
		float Linear    = 0.f;
		float Quadratic = 0.f;
	};

	class Light
	{
	protected:
		LightInfo m_Info;

	public:
		Light()          = default;
		virtual ~Light() = default;

		LightInfo GetInfo() const
		{
			return m_Info;
		}
	};

	class DirectionalLight: public Light
	{
	public:
		DirectionalLight()
		{
			m_Info.Type = LightType::Directional;
		}

		glm::vec3 GetDirection() const { return m_Info.Direction; }

		glm::vec3 GetDiffuseColor() const { return m_Info.DiffuseColor; }
		glm::vec3 GetAmbientColor() const { return m_Info.AmbientColor; }
		glm::vec3 GetSpecularColor() const { return m_Info.SpecularColor; }

		void SetDirection(const glm::vec3 &direction) { m_Info.Direction = direction; }

		void SetDiffuseColor(const glm::vec3 &diffuse) { m_Info.DiffuseColor = diffuse; }
		void SetAmbientColor(const glm::vec3 &ambient) { m_Info.AmbientColor = ambient; }
		void SetSpecularColor(const glm::vec3 &specular) { m_Info.SpecularColor = specular; }
	};

	class PointLight: public Light
	{
	public:
		PointLight()
		{
			m_Info.Type = LightType::Point;
		}

		glm::vec3 GetPosition() const { return m_Info.Position; }

		glm::vec3 GetDiffuseColor() const { return m_Info.DiffuseColor; }
		glm::vec3 GetAmbientColor() const { return m_Info.AmbientColor; }
		glm::vec3 GetSpecularColor() const { return m_Info.SpecularColor; }

		float GetConstant() const { return m_Info.Constant; }
		float GetLinear() const { return m_Info.Linear; }
		float GetQuadratic() const { return m_Info.Quadratic; }

		void SetPosition(const glm::vec3 &position) { m_Info.Position = position; }

		void SetDiffuseColor(const glm::vec3 &diffuse) { m_Info.DiffuseColor = diffuse; }
		void SetAmbientColor(const glm::vec3 &ambient) { m_Info.AmbientColor = ambient; }
		void SetSpecularColor(const glm::vec3 &specular) { m_Info.SpecularColor = specular; }

		void SetConstant(float constant) { m_Info.Constant = constant; }
		void SetLinear(float linear) { m_Info.Linear = linear; }
		void SetQuadratic(float quadratic) { m_Info.Quadratic = quadratic; }
	};

	class SpotLight: public Light
	{
	public:
		SpotLight()
		{
			m_Info.Type = LightType::Spot;
		}

		glm::vec3 GetPosition() const { return m_Info.Position; }

		glm::vec3 GetDiffuseColor() const { return m_Info.DiffuseColor; }
		glm::vec3 GetAmbientColor() const { return m_Info.AmbientColor; }
		glm::vec3 GetSpecularColor() const { return m_Info.SpecularColor; }

		float GetConstant() const { return m_Info.Constant; }
		float GetLinear() const { return m_Info.Linear; }
		float GetQuadratic() const { return m_Info.Quadratic; }

		void SetPosition(const glm::vec3 &position) { m_Info.Position = position; }

		void SetDiffuseColor(const glm::vec3 &diffuse) { m_Info.DiffuseColor = diffuse; }
		void SetAmbientColor(const glm::vec3 &ambient) { m_Info.AmbientColor = ambient; }
		void SetSpecularColor(const glm::vec3 &specular) { m_Info.SpecularColor = specular; }

		void SetConstant(float constant) { m_Info.Constant = constant; }
		void SetLinear(float linear) { m_Info.Linear = linear; }
		void SetQuadratic(float quadratic) { m_Info.Quadratic = quadratic; }
	};
}
