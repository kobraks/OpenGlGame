#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Renderer/Sampler.h"

namespace Engine {

	class SamplerBuilder {
	public:
		SamplerBuilder() = default;

		SamplerBuilder& LinearRepeat();
		SamplerBuilder& LinearClamp();
		SamplerBuilder& NearestRepeat();
		SamplerBuilder& ShadowCompare();
		SamplerBuilder& HighAnisotropy(uint32_t x = 8);

		SamplerBuilder& MinFilter(FilterMode filter);
		SamplerBuilder& MagFilter(FilterMode filter);

		SamplerBuilder& WrapS(WrapMode s);
		SamplerBuilder& WrapT(WrapMode t);
		SamplerBuilder& WrapR(WrapMode r);

		SamplerBuilder& Wrap(WrapMode s, WrapMode t, WrapMode r = WrapMode::Repeat);

		SamplerBuilder& LodBias(float bias);
		SamplerBuilder& LodRange(float minLod, float maxLod);

		SamplerBuilder& Anisotropy(float aniso);

		SamplerBuilder& BorderColor(const Color& color);

		SamplerBuilder& Compare(CompareFunction func);
		SamplerBuilder& CompareEnabled(bool enabled = true);

		SamplerBuilder& Label(const std::string& label);

		SamplerBuilder& Clear();

		const SamplerSpec& Spec() const { return m_Spec; }
		Ref<Sampler> Build() const;
	private:
		static bool HasAniso();
		static float MaxAniso();

		SamplerSpec m_Spec;
	};
}