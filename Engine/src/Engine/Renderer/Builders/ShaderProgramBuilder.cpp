#include "pch.h"
#include "ShaderProgramBuilder.h"

#include "Engine/Renderer/ShaderSource.h"

namespace Engine {
	ShaderProgramBuilder& ShaderProgramBuilder::Label(const std::string& label) {
		m_Label = label;
		return *this;
	}

	ShaderProgramBuilder& ShaderProgramBuilder::AddStage(Ref<ShaderStage> stage) {
		if (stage && stage->IsCompiled()) {
			m_Stages.emplace_back(stage);
		} else {
			LOG_WARN("Attempted to add uncompiled shader stage: {}", stage ? stage->Label() : "<null>");
		}

		return *this;
	}

	ShaderProgramBuilder& ShaderProgramBuilder::Vert(Ref<ShaderSource> source, const std::string& label) {
		return AddStage(ShaderStage::CreateVertex(source, label));
	}

	ShaderProgramBuilder& ShaderProgramBuilder::Frag(Ref<ShaderSource> source, const std::string& label) {
		return AddStage(ShaderStage::CreateFragment(source, label));
	}

	ShaderProgramBuilder& ShaderProgramBuilder::Geom(Ref<ShaderSource> source, const std::string& label) {
		return AddStage(ShaderStage::CreateGeometry(source, label));
	}

	ShaderProgramBuilder& ShaderProgramBuilder::Compute(Ref<ShaderSource> source, const std::string& label) {
		return AddStage(ShaderStage::CreateCompute(source, label));
	}

	ShaderProgramBuilder& ShaderProgramBuilder::Control(Ref<ShaderSource> source, const std::string& label) {
		return AddStage(ShaderStage::CreateControl(source, label));
	}

	ShaderProgramBuilder& ShaderProgramBuilder::Eval(Ref<ShaderSource> source, const std::string& label) {
		return AddStage(ShaderStage::CreateEvaluation(source, label));
	}

	ShaderProgramBuilder& ShaderProgramBuilder::Clear() {
		m_Label.clear();
		m_Stages.clear();

		return *this;
	}

	Ref<ShaderProgram> ShaderProgramBuilder::Build() const {
		auto [program, result] = BuildWithResult();

		if (!result) {
			LOG_ERROR("ShaderProgram [{}] failed to link:\n{}", m_Label, result.LogMessage);
		}

		return program;
	}

	std::pair<Ref<ShaderProgram>, ShaderLinkResult> ShaderProgramBuilder::BuildWithResult() const {
		auto program = ShaderProgram::Create(m_Label);

		for (const auto& stage : m_Stages) {
			program->Attach(stage);
		}

		auto result = program->Link();

		return { program, result };
	}
}
