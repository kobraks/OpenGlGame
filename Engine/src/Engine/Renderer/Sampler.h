#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/Color.h"
#include "Engine/Renderer/GraphicEnums.h"

namespace Engine {
	struct SamplerSpec {
		// Filters
		FilterMode MinFilterMode = FilterMode::LinearMipmapLinear;
		FilterMode MagFilterMode = FilterMode::Linear;

		// Wrap
		WrapMode WrapS = WrapMode::Repeat;
		WrapMode WrapT = WrapMode::Repeat;
		WrapMode WrapR = WrapMode::Repeat;

		// LOD
		float LodBias = 0.f;
		float MinLod = -1000.f;
		float MaxLod = 1000.f;

		// Anisotropy (1.0f == off)
		float Anisotropy = 1.f;

		// Border color (used when Wrap* == ClampBorder)
		Color BorderColor = Color::Black;

		//Shadow comparison
		bool CompareEnabled = false;
		CompareFunction CompareFun = CompareFunction::LEqual;

		std::string Label;
	};

	class Sampler {
	public:
		using IDType = uint32_t;

		~Sampler() = default;

		static Ref<Sampler> Create(const SamplerSpec& spec = {});

		[[nodiscard]] explicit operator IDType() const { return m_GLState->ID; }
		[[nodiscard]] IDType RendererID() const { return m_GLState->ID; }

		void Bind(uint32_t slot) const;
		void Unbind(uint32_t slot) const;

		void SetLabel(const std::string& label);
		const SamplerSpec& Specs() const { return m_GLState->Specs; }

		static SamplerSpec LinearRepeat();
		static SamplerSpec LinearClamp();
		static SamplerSpec NearestRepeat();
		static SamplerSpec HighAnisotropy(uint32_t x = 8);
		static SamplerSpec ShadowCompare(); // Clamp to border + compare

		static float QueryMaxAnisotropy();
	private:
		struct GLState {
			IDType ID = 0;
			SamplerSpec Specs;

			GLState();
			~GLState();
		};

		Sampler();
		void Initialize(const SamplerSpec& spec);

		void Parameter(uint32_t name, int32_t value);
		void Parameter(uint32_t name, float value);
		void Parameter(uint32_t name, const Color& color);

		Ref<GLState> m_GLState;
	};
}