#pragma once
#include "Engine/Renderer/ShaderProgram.h"

namespace Engine {
	class ShaderProgramBuilder {
	public:
		ShaderProgramBuilder& Label(const std::string& label);

		ShaderProgramBuilder& AddStage(Ref<ShaderStage> stage);
		ShaderProgramBuilder& Vert(Ref<ShaderSource> source, const std::string& label);
		ShaderProgramBuilder& Frag(Ref<ShaderSource> source, const std::string& label);
		ShaderProgramBuilder& Geom(Ref<ShaderSource> source, const std::string& label);
		ShaderProgramBuilder& Compute(Ref<ShaderSource> source, const std::string& label);
		ShaderProgramBuilder& Control(Ref<ShaderSource> source, const std::string& label);
		ShaderProgramBuilder& Eval(Ref<ShaderSource> source, const std::string& label);

		ShaderProgramBuilder& Clear();

		Ref<ShaderProgram> Build() const;
		std::pair<Ref<ShaderProgram>, ShaderLinkResult> BuildWithResult() const;

	private:
		std::vector<Ref<ShaderStage>> m_Stages;
		std::string m_Label;

	};
}