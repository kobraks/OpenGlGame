#include "pch.h"
#include "ShaderStage.h"

#include "Engine/Renderer/ShaderSource.h"

#include <glad/glad.h>

#include <boost/algorithm/string.hpp>

namespace Engine {
	namespace Utils {
		GLenum ToGLType(ShaderStage::Type type) {
			switch (type) {
			case ShaderStage::Type::Vertex:
				return GL_VERTEX_SHADER;
			case ShaderStage::Type::Fragment:
				return GL_FRAGMENT_SHADER;
			case ShaderStage::Type::Geometry:
				return GL_GEOMETRY_SHADER;
			case ShaderStage::Type::Compute:
				return GL_COMPUTE_SHADER;
			case ShaderStage::Type::Control:
				return GL_TESS_CONTROL_SHADER;
			case ShaderStage::Type::Evaluation:
				return GL_TESS_EVALUATION_SHADER;
			default:
				break;
			}

			ENGINE_ASSERT(false);
			throw std::runtime_error("Unknown shader type");
		}

		constexpr std::string_view ShaderTypeString(ShaderStage::Type type) {
			switch (type) {
			case ShaderStage::Type::Vertex:
				return "Vertex";
			case ShaderStage::Type::Fragment:
				return "Fragment";
			case ShaderStage::Type::Geometry:
				return "Geometry";
			case ShaderStage::Type::Compute:
				return "Compute";
			case ShaderStage::Type::Control:
				return "Tessellation Control";
			case ShaderStage::Type::Evaluation:
				return "Tessellation Evaluation";
			default:
				break;
			}

			return "Unknown";
		}
	}

	Ref<ShaderStage> ShaderStage::Create(Type type, Ref<ShaderSource> source, const std::string& label) {
		Ref<ShaderStage> Shader = Ref<ShaderStage>(new ShaderStage(type));
		Shader->SetLabel(label);

		LOG_GL_DEBUG("Creating {} {} Shader stage", label, Utils::ShaderTypeString(type));
		LOG_GL_DEBUG("{} {} Shader stage id: {}", label, Utils::ShaderTypeString(type), Shader->m_GLState->Shader);

		if (source) {
			Shader->SetSource(source);
			Shader->Compile();
		}


		return Shader;
	}

	Ref<ShaderStage> ShaderStage::CreateVertex(Ref<ShaderSource> source, const std::string& label) {
		return Create(Type::Vertex, source, label);
	}

	Ref<ShaderStage> ShaderStage::CreateFragment(Ref<ShaderSource> source, const std::string& label) {
		return Create(Type::Fragment, source, label);
	}

	Ref<ShaderStage> ShaderStage::CreateGeometry(Ref<ShaderSource> source, const std::string& label) {
		return Create(Type::Geometry, source, label);
	}

	Ref<ShaderStage> ShaderStage::CreateCompute(Ref<ShaderSource> source, const std::string& label) {
		return Create(Type::Compute, source, label);
	}

	Ref<ShaderStage> ShaderStage::CreateControl(Ref<ShaderSource> source, const std::string& label) {
		return Create(Type::Control, source, label);
	}

	Ref<ShaderStage> ShaderStage::CreateEvaluation(Ref<ShaderSource> source, const std::string& label) {
		return Create(Type::Evaluation, source, label);
	}

	void ShaderStage::SetLabel(const std::string &label) {
		if (label.empty())
			return;

		m_GLState->Label = label;
		glObjectLabel(GL_SHADER, static_cast<IDType>(*this), -1, m_GLState->Label.c_str());
	}

	ShaderCompileResult ShaderStage::Compile() {
		LOG_GL_DEBUG("Compiling {} [id: {}] {} Shader", m_GLState->Label, m_GLState->Shader, Utils::ShaderTypeString(m_GLState->Type));
		glCompileShader(static_cast<IDType>(*this));

		ShaderCompileResult result;
		int status = GetParameter(GL_COMPILE_STATUS);

		if (status == GL_FALSE) {
			LOG_GL_ERROR("Failed to compile [id: {}] {} {} Shader", m_GLState->Shader, m_GLState->Label, Utils::ShaderTypeString(m_GLState->Type));
			result.Success = m_GLState->Compiled = false;
		}
		else {
			LOG_GL_DEBUG("Successfully compiled [id: {}] {} {} Shader", m_GLState->Shader, m_GLState->Label, Utils::ShaderTypeString(m_GLState->Type));
			result.Success = m_GLState->Compiled = true;
		}

		FetchLog();
		result.LogMessage = m_GLState->LogMessage;

		if (!result.LogMessage.empty()) {
			if (!result.Success)
				LOG_GL_ERROR("Compilation failed [Id: {}] '{}': {}", m_GLState->Shader, m_GLState->Label, result.LogMessage);
			else
				LOG_GL_DEBUG("Compilation log [Id: {}] '{}': {}", m_GLState->Shader, m_GLState->Label, result.LogMessage);
		}

		return result;
	}

	void ShaderStage::SetSource(Ref<ShaderSource> source) {
		if (!source)
			return;

		const auto cstr = source->Source().data();

		LOG_GL_DEBUG("Attaching source code to {} {} shader: {}", m_GLState->Label, Utils::ShaderTypeString(m_GLState->Type), m_GLState->Shader);
		LOG_GL_TRACE("Source code: \n{}\nEND", cstr);

		m_GLState->Source = source;

		glShaderSource(static_cast<IDType>(*this), 1, &cstr, nullptr);

		FetchLog();
	}

	int ShaderStage::GetParameter(uint32_t pName) const {
		int value = 0;
		GetParameter(pName, &value);

		return value;
	}

	void ShaderStage::GetParameter(uint32_t pName, int* params) const {
		glGetShaderiv(static_cast<IDType>(*this), pName, params);
	}

	void ShaderStage::FetchLog() {
		int length = GetParameter(GL_INFO_LOG_LENGTH);

		if (length > 0) {
			m_GLState->LogMessage.resize(length + 1, 0);

			glGetShaderInfoLog(static_cast<IDType>(*this), length, nullptr, m_GLState->LogMessage.data());

			boost::trim(m_GLState->LogMessage);
		}
		else
			m_GLState->LogMessage.resize(1, 0);

		if (!m_GLState->LogMessage.empty() && m_GLState->LogMessage.back() == '\0')
			m_GLState->LogMessage.pop_back();
	}


	std::string_view ShaderStage::TypeToString() const {
		return Utils::ShaderTypeString(m_GLState->Type);
	}

	std::string_view ShaderStage::TypeToString(Type type) {
		return Utils::ShaderTypeString(type);
	}

	ShaderStage::ShaderStage(Type type) : m_GLState(MakeRef<GLState>(type)) {
	}

	ShaderStage::GLState::GLState(ShaderStage::Type type) {
		Shader = glCreateShader(Utils::ToGLType(type));
		ENGINE_ASSERT(Shader != 0, "Failed to create shader!");
	}

	ShaderStage::GLState::~GLState() {
		glDeleteShader(Shader);
	}
}
