#include "pch.h"
#include "Sampler.h"

#include "Engine/Utils/Renderer/GLEnumConverters.h"

#include "glad/glad.h"

namespace Engine {
	namespace Utils {
		static Sampler::IDType CreateSampler() {
			GLuint id;
			glCreateSamplers(1, &id);
			return static_cast<Sampler::IDType>(id);
		}

		static bool HasAsioExt() {
			return GLAD_GL_EXT_texture_filter_anisotropic != 0;
		}
	}

	Sampler::Sampler()
		: m_GLState(MakeRef<GLState>()) {
	}

	Ref<Sampler> Sampler::Create(const SamplerSpec& spec) {
		Ref<Sampler> sampler = Ref<Sampler>(new Sampler());

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

		spec.Min = FilterMode::LinearMipmapLinear;
		spec.Mag = FilterMode::Linear;

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

		spec.Min = FilterMode::Nearest;
		spec.Mag = FilterMode::Nearest;

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

		spec.Min = FilterMode::Linear;
		spec.Mag = FilterMode::Linear;

		spec.WrapS = spec.WrapT = spec.WrapR = WrapMode::ClampBorder;
		spec.BorderColor = Color::White;

		spec.CompareEnabled = true;
		spec.Compare = CompareFunction::LEqual;

		return spec;
	}

	Sampler::GLState::GLState() {
		ID = Utils::CreateSampler();
		if (ID == 0) {
			LOG_ENGINE_ERROR("Failed to create sampler!");
		}
	}

	Sampler::GLState::~GLState() {
		glDeleteSamplers(1, &ID);
	}

	void Sampler::Initialize(const SamplerSpec& spec) {
		m_GLState->Specs = spec;

		// Filters
		Parameter(GL_TEXTURE_MIN_FILTER, static_cast<int32_t>(Utils::EnumToGLConstant(spec.Min)));
		Parameter(GL_TEXTURE_MIN_FILTER, static_cast<int32_t>(Utils::EnumToGLConstant(spec.Mag)));

		// Wrap
		Parameter(GL_TEXTURE_WRAP_S, static_cast<int32_t>(Utils::EnumToGLConstant(spec.WrapS)));
		Parameter(GL_TEXTURE_WRAP_T, static_cast<int32_t>(Utils::EnumToGLConstant(spec.WrapT)));
		Parameter(GL_TEXTURE_WRAP_R, static_cast<int32_t>(Utils::EnumToGLConstant(spec.WrapR)));

		// LOD
		Parameter(GL_TEXTURE_LOD_BIAS, spec.LodBias);
		Parameter(GL_TEXTURE_MIN_LOD, spec.MinLod);
		Parameter(GL_TEXTURE_MAX_LOD, spec.MaxLod);

		// Anisotropy
		if (Utils::HasAsioExt()) {
			const float clamped = std::max(1.0f, std::min(spec.Anisotropy, QueryMaxAnisotropy()));
			Parameter(GL_TEXTURE_MAX_ANISOTROPY, clamped);

		}
		else if (spec.Anisotropy != 1.0f) {
			LOG_ENGINE_WARN("Anisotropic filtering not supported!");
		}

		// Compare (for shadow samplers)
		Parameter(GL_TEXTURE_COMPARE_MODE, spec.CompareEnabled ? GL_COMPARE_REF_TO_TEXTURE : GL_NONE);
		if (spec.CompareEnabled) {
			Parameter(GL_TEXTURE_COMPARE_FUNC, static_cast<int32_t>(Utils::EnumToGLConstant(spec.Compare)));
		}

		Parameter(GL_TEXTURE_BORDER_COLOR, spec.BorderColor);

		if (!spec.Label.empty())
			glObjectLabel(GL_SAMPLER, m_GLState->ID, -1, spec.Label.c_str());
	}

	float Sampler::QueryMaxAnisotropy() {
		static bool initialized = false;
		static float maxAniso = 1.0f;

		if (!initialized) {
			initialized = true;
			if (Utils::HasAsioExt()) {
				glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
			}
			else{
				LOG_ENGINE_WARN("Anisotropic filtering not supported!");
			}
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
