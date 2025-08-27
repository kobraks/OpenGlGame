#include "pch.h"
#include "SamplerBuilder.h"

#include "Engine/Utils/Renderer/FilterModeUtils.h"

#include "glad/glad.h"

#include <algorithm>


namespace Engine {
	namespace Utils {
		static inline bool HasAnisoExt() {
			return GLAD_GL_EXT_texture_filter_anisotropic != 0;
		}
	}

	SamplerBuilder& SamplerBuilder::LinearRepeat() {
		m_Spec = Sampler::LinearRepeat();
		return *this;
	}

	SamplerBuilder& SamplerBuilder::LinearClamp() {
		m_Spec = Sampler::LinearClamp();
		return *this;
	}

	SamplerBuilder& SamplerBuilder::NearestRepeat() {
		m_Spec = Sampler::NearestRepeat();
		return *this;
	}

	SamplerBuilder& SamplerBuilder::ShadowCompare() {
		m_Spec = Sampler::ShadowCompare();
		return *this;
	}

	SamplerBuilder& SamplerBuilder::HighAnisotropy(uint32_t x) {
		m_Spec = Sampler::HighAnisotropy(x);
		return *this;
	}

	SamplerBuilder& SamplerBuilder::MinFilter(FilterMode filter) {
		m_Spec.Min = filter;
		return *this;
	}

	SamplerBuilder& SamplerBuilder::MagFilter(FilterMode filter) {
		m_Spec.Mag = Utils::SanitizeMag(filter);
		return *this;
	}

	SamplerBuilder& SamplerBuilder::WrapS(WrapMode s) {
		m_Spec.WrapS = s;
		return *this;
	}

	SamplerBuilder& SamplerBuilder::WrapT(WrapMode t) {
		m_Spec.WrapT = t;
		return *this;
	}

	SamplerBuilder& SamplerBuilder::WrapR(WrapMode r) {
		m_Spec.WrapR = r;
		return *this;
	}

	SamplerBuilder& SamplerBuilder::Wrap(WrapMode s, WrapMode t, WrapMode r) {
		m_Spec.WrapS = s;
		m_Spec.WrapT = t;
		m_Spec.WrapR = r;

		return *this;
	}

	SamplerBuilder& SamplerBuilder::LodBias(float bias) {
		m_Spec.LodBias = bias;
		return *this;
	}

	SamplerBuilder& SamplerBuilder::LodRange(float minLod, float maxLod) {
		if (minLod > maxLod) {
			std::swap(minLod, maxLod);
		}

		m_Spec.MinLod = minLod;
		m_Spec.MaxLod = maxLod;

		return *this;
	}

	SamplerBuilder& SamplerBuilder::Anisotropy(float aniso) {
		if (!HasAniso()) {
			m_Spec.Anisotropy = 1.0f;
			return *this;
		}

		m_Spec.Anisotropy = std::clamp(aniso, 1.0f, MaxAniso());
		return *this;
	}

	SamplerBuilder& SamplerBuilder::BorderColor(const Color& color) {
		m_Spec.BorderColor = color;
		return *this;
	}

	SamplerBuilder& SamplerBuilder::Compare(CompareFunction func) {
		m_Spec.CompareFun = func;
		return *this;
	}

	SamplerBuilder& SamplerBuilder::CompareEnabled(bool enabled) {
		m_Spec.CompareEnabled = enabled;
		return *this;
	}

	SamplerBuilder& SamplerBuilder::Label(const std::string& label) {
		m_Spec.Label = label;
		return *this;
	}

	SamplerBuilder& SamplerBuilder::Clear() {
		m_Spec = SamplerSpec();
		return *this;
	}

	Ref<Sampler> SamplerBuilder::Build() const {
		SamplerSpec spec = m_Spec;

		spec.Mag = Utils::SanitizeMag(spec.Mag);

		if (HasAniso()) {
			spec.Anisotropy = std::clamp(spec.Anisotropy, 1.f, MaxAniso());
		}
		else { spec.Anisotropy = 1.f;  }

		return Sampler::Create(spec);
	}

	bool SamplerBuilder::HasAniso() {
		return Utils::HasAnisoExt();
	}

	float SamplerBuilder::MaxAniso() {
		if (!Utils::HasAnisoExt()) {
			LOG_ENGINE_WARN("Anisotropic filtering not supported!");
			return 1.0f;
		}

		return Sampler::QueryMaxAnisotropy();
	}
}
