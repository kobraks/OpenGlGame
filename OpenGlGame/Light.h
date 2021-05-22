#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Game
{
	class Texture;

	enum class LightType : int32_t
	{
		Unknown = 0,
		Spot = 1,
		Point = 2,
		Directional = 3
	};

	struct LightInfo
	{
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

		Pointer<Texture> CookieTexture;
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

		void SetActive(bool active = true)
		{
			m_Info.Active = active;
		}

		bool IsActive() const
		{
			return m_Info.Active;
		}

		glm::vec3 GetDiffuseColor() const { return m_Info.DiffuseColor; }
		glm::vec3 GetAmbientColor() const { return m_Info.AmbientColor; }
		glm::vec3 GetSpecularColor() const { return m_Info.SpecularColor; }

		void SetDiffuseColor(const glm::vec3 &diffuse) { m_Info.DiffuseColor = diffuse; }
		void SetAmbientColor(const glm::vec3 &ambient) { m_Info.AmbientColor = ambient; }
		void SetSpecularColor(const glm::vec3 &specular) { m_Info.SpecularColor = specular; }
	};

	class DirectionalLight: public Light
	{
	public:
		DirectionalLight()
		{
			m_Info.Type = LightType::Directional;
		}

		glm::vec3 GetDirection() const { return m_Info.Direction; }

		void SetDirection(const glm::vec3 &direction) { m_Info.Direction = direction; }
	};

	class PointLight: public Light
	{
	public:
		PointLight()
		{
			m_Info.Type = LightType::Point;
		}

		glm::vec3 GetPosition() const { return m_Info.Position; }

		float GetConstant() const { return m_Info.Constant; }
		float GetLinear() const { return m_Info.Linear; }
		float GetQuadratic() const { return m_Info.Quadratic; }

		void SetPosition(const glm::vec3 &position) { m_Info.Position = position; }

		void SetConstant(float constant) { m_Info.Constant = constant; }
		void SetLinear(float linear) { m_Info.Linear = linear; }
		void SetQuadratic(float quadratic) { m_Info.Quadratic = quadratic; }
	};

	class SpotLight: public PointLight
	{
	public:
		SpotLight()
		{
			m_Info.Type = LightType::Spot;
		}

		float GetOuterCutOff() const { return m_Info.OuterCutOff; }
		float GetCutOff() const { return m_Info.CutOff; }

		void SetCutOff(float cutOff) { m_Info.CutOff = cutOff; }
		void SetOuterCutOff(float outerCutOff) { m_Info.OuterCutOff = outerCutOff; }

		void SetTexture(Pointer<Texture> texture) { m_Info.CookieTexture = texture; }
	};
}
