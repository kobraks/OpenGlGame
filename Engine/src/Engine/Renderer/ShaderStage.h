#pragma once
#include "Engine/Core/Base.h"

#include <string>

namespace Engine {
	class ShaderSource;

	struct ShaderCompileResult {
		bool Success;
		std::string LogMessage;

		explicit operator bool() const { return Success; }
	};

	class ShaderStage {
	public:
		enum class Type {
			Unknown = 0,
			Vertex,
			Fragment,
			Geometry,
			Compute,
			Control,
			Evaluation
		};

		using IDType = uint32_t;

		static Ref<ShaderStage> Create(Type type, Ref<ShaderSource> source = nullptr, const std::string& label = {});
		static Ref<ShaderStage> CreateVertex(Ref<ShaderSource> source = nullptr, const std::string& label = {});
		static Ref<ShaderStage> CreateFragment(Ref<ShaderSource> source = nullptr, const std::string& label = {});
		static Ref<ShaderStage> CreateGeometry(Ref<ShaderSource> source = nullptr, const std::string& label = {});
		static Ref<ShaderStage> CreateCompute(Ref<ShaderSource> source = nullptr, const std::string& label = {});
		static Ref<ShaderStage> CreateControl(Ref<ShaderSource> source = nullptr, const std::string& label = {});
		static Ref<ShaderStage> CreateEvaluation(Ref<ShaderSource> source = nullptr, const std::string& label = {});

		void SetLabel(const std::string& label);
		std::string_view Label() const { return m_GLState->Label; }

		ShaderCompileResult Compile();
		bool IsCompiled() const { return m_GLState->Compiled; }

		void SetSource(Ref<ShaderSource> source);
		Ref<ShaderSource> GetSource() const { return m_GLState->Source; }

		operator IDType() const { return m_GLState->Shader; }
		IDType ID() const { return m_GLState->Shader; }

		Type GetType() const { return m_GLState->Type; }

		std::string_view TypeToString() const;
		static std::string_view TypeToString(Type type);

		std::string_view Log() const { return m_GLState->LogMessage; }

	private:
		explicit ShaderStage(Type type);

		int GetParameter(uint32_t pName) const;
		void GetParameter(uint32_t pName, int* params) const;

		void FetchLog();

	private:

		class GLState {
		public:
			Ref<ShaderSource> Source;
			Type Type     = Type::Unknown;
			bool Compiled = false;
			IDType Shader = 0;

			std::string LogMessage = {};
			std::string Label = {};

			explicit GLState(ShaderStage::Type type);
			~GLState();
		};

		Ref<GLState> m_GLState;
	};
}
