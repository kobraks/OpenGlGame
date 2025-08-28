#include "pch.h"
#include "Sampler.h"

#include "Engine/Utils/Renderer/GLEnumConverters.h"
#include "Engine/Utils/Renderer/FilterModeUtils.h"
#include "Engine/Utils/Renderer/EnumStringConverters.h"

#include "glad/glad.h"

namespace Engine {
	namespace Utils {
		static Sampler::IDType CreateSampler() {
			GLuint id;
			glCreateSamplers(1, &id);
			return static_cast<Sampler::IDType>(id);
		}

		static bool HasAnisoExt() {
			return GLAD_GL_EXT_texture_filter_anisotropic != 0;
		}
	}

	Sampler::Sampler()
		: m_GLState(MakeRef<GLState>()) {
	}

	Ref<Sampler> Sampler::Create(const SamplerSpec& spec) {
		Ref<Sampler> sampler = Ref<Sampler>(new Sampler());
		sampler->Initialize(spec);

		return sampler;
	}

	void Sampler::Bind(uint32_t slot) const {
		glBindSampler(static_cast<GLint>(slot), m_GLState->ID);
	}

	void Sampler::Unbind(uint32_t slot) const {
		glBindSampler(static_cast<GLint>(slot), 0);
	}

	void Sampler::SetLabel(const std::string& label) {
		if (label.empty() && m_GLState->Specs.Label.empty())
			return;

		glObjectLabel(GL_SAMPLER, m_GLState->ID, -1, label.c_str());
		m_GLState->Specs.Label = label;
	}

	SamplerSpec Sampler::LinearRepeat() {
		SamplerSpec spec;

		spec.MinFilterMode = FilterMode::LinearMipmapLinear;
		spec.MagFilterMode = FilterMode::Linear;

		spec.WrapS = spec.WrapR = spec.WrapT = WrapMode::Repeat;

		return spec;
	}

	SamplerSpec Sampler::LinearClamp() {
		auto spec = LinearRepeat();
		spec.WrapS = spec.WrapT = spec.WrapR = WrapMode::ClampEdge;

		return spec;
	}

	SamplerSpec Sampler::NearestRepeat() {
		SamplerSpec spec;

		spec.MinFilterMode = FilterMode::Nearest;
		spec.MagFilterMode = FilterMode::Nearest;

		spec.WrapS = spec.WrapR = spec.WrapT = WrapMode::Repeat;

		return spec;
	}

	SamplerSpec Sampler::HighAnisotropy(uint32_t x) {
		SamplerSpec spec = LinearRepeat();

		spec.Anisotropy = static_cast<float>(x);
		return spec;
	}

	SamplerSpec Sampler::ShadowCompare() {
		SamplerSpec spec;

		spec.MinFilterMode = FilterMode::Linear;
		spec.MagFilterMode = FilterMode::Linear;

		spec.WrapS = spec.WrapT = spec.WrapR = WrapMode::ClampBorder;
		spec.BorderColor = Color::White;

		spec.CompareEnabled = true;
		spec.CompareFun = CompareFunction::LEqual;

		return spec;
	}

	Sampler::GLState::GLState() {
		ID = Utils::CreateSampler();
		if (ID == 0) {
			LOG_ENGINE_ERROR("Failed to create sampler!");
		} else {
			LOG_GL_DEBUG("Created sampler with ID {}", ID);
		}
	}

	Sampler::GLState::~GLState() {
		if (ID)
			glDeleteSamplers(1, &ID);
	}

	void Sampler::Initialize(const SamplerSpec& spec) {
		m_GLState->Specs = spec;
		m_GLState->Specs.MagFilterMode = Utils::SanitizeMag(spec.MagFilterMode);
		const auto& specs = m_GLState->Specs;

		LOG_GL_DEBUG("Initializing sampler with (ID={}): MinFilterMode={}, MagFilterMode={}, WrapS={}, WrapT={}, WrapR={},"
			"Anisotropy={}, LodBias={}, MinLod={}, MaxLod={}, CompareEnabled={}, CompareFunction={}, BorderColor={}, Label='{}'",
			m_GLState->ID, specs.MinFilterMode, specs.MagFilterMode,
			specs.WrapS, specs.WrapT, specs.WrapR,
			specs.Anisotropy, specs.LodBias, specs.MinLod, specs.MaxLod,
			specs.CompareEnabled, specs.CompareFun, specs.BorderColor,
			specs.Label);

		// Filters
		Parameter(GL_TEXTURE_MIN_FILTER, static_cast<int32_t>(Utils::EnumToGLConstant(specs.MinFilterMode)));
		Parameter(GL_TEXTURE_MAG_FILTER, static_cast<int32_t>(Utils::EnumToGLConstant(specs.MagFilterMode)));

		// Wrap
		Parameter(GL_TEXTURE_WRAP_S, static_cast<int32_t>(Utils::EnumToGLConstant(specs.WrapS)));
		Parameter(GL_TEXTURE_WRAP_T, static_cast<int32_t>(Utils::EnumToGLConstant(specs.WrapT)));
		Parameter(GL_TEXTURE_WRAP_R, static_cast<int32_t>(Utils::EnumToGLConstant(specs.WrapR)));

		// LOD
		Parameter(GL_TEXTURE_LOD_BIAS, specs.LodBias);
		Parameter(GL_TEXTURE_MIN_LOD, specs.MinLod);
		Parameter(GL_TEXTURE_MAX_LOD, specs.MaxLod);

		// Anisotropy
		if (Utils::HasAnisoExt()) {
			const float clamped = std::max(1.0f, std::min(specs.Anisotropy, QueryMaxAnisotropy()));
			Parameter(GL_TEXTURE_MAX_ANISOTROPY_EXT, clamped);

		}
		else if (specs.Anisotropy != 1.0f) {
			LOG_ENGINE_WARN("Anisotropic filtering not supported!");
		}

		// Compare (for shadow samplers)
		Parameter(GL_TEXTURE_COMPARE_MODE, specs.CompareEnabled ? GL_COMPARE_REF_TO_TEXTURE : GL_NONE);
		if (specs.CompareEnabled) {
			Parameter(GL_TEXTURE_COMPARE_FUNC, static_cast<int32_t>(Utils::EnumToGLConstant(specs.CompareFun)));
		}

		Parameter(GL_TEXTURE_BORDER_COLOR, specs.BorderColor);

		if (!specs.Label.empty())
			glObjectLabel(GL_SAMPLER, m_GLState->ID, -1, specs.Label.c_str());
	}

	float Sampler::QueryMaxAnisotropy() {
		if (!Utils::HasAnisoExt()) {
			LOG_ENGINE_WARN("Anisotropic filtering not supported!");
			return 1.0f;
		}

		static bool initialized = false;
		static float maxAniso = 1.0f;

		if (!initialized) {
			initialized = true;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
		}

		return maxAniso;
	}

	void Sampler::Parameter(uint32_t name, int32_t value) {
		glSamplerParameteri(m_GLState->ID, name, value);
	}

	void Sampler::Parameter(uint32_t name, float value) {
		glSamplerParameterf(m_GLState->ID, name, value);
	}

	void Sampler::Parameter(uint32_t name, const Color& color) {
		const auto vec = color.ToFloat();

		glSamplerParameterfv(m_GLState->ID, name, &vec.r);
	}
}
