#pragma once
#include <memory>
#include <unordered_map>
#include <optional>

#include "Types.h"
#include "Uniform.h"
#include "Shader.h"

namespace Game
{
	#define DEFAULT_PROGRAM_SHADER_NAME "Default"
	
	class ShaderProgram
	{
	public:
		using IdType = uint32_t;

	private:
		std::unordered_map<std::string, std::shared_ptr<UniformObject>> m_Uniforms;
		mutable std::unordered_map<std::string, int> m_Attributes;

		Pointer<IdType> m_Program = nullptr;

		bool m_Linked          = false;
		mutable bool m_Changed = true;
		std::string m_Name;
		
	public:
		explicit ShaderProgram(const std::string &name);
		ShaderProgram(IdType id, const std::string &name);
		ShaderProgram(const std::string &name, const Shader &shader);

		template <class ...Args>
		ShaderProgram(const std::string &name, const Shader &shader, Args &&... args) : ShaderProgram(name)
		{
			Attach(shader, std::forward<Args>(args)...);

			Link();

			Detach(shader, std::forward<Args>(args)...);
		}

		operator IdType() const { return *m_Program; }
		IdType ID() const { return *m_Program; }

		bool IsLinked() const { return m_Linked; }

		const std::string Name() const { return m_Name; };

		void Attach(const Shader &shader);
		void Detach(const Shader &shader);

		bool Link();

		void Use() const;

		void Flush() const;

		std::string GetLog() const;
		int GetAttributeLocation(const std::string &name) const;
		
		bool HasUniform(const std::string &name) const;

		static ShaderProgram* GetDefault();
	private:
		void Populate();

		Pointer<UniformObject> GetUniformObject(const std::string &name) const;
	public:
		template <typename Type>
		std::optional<UniformTable<Type>> GetUniform(const std::string &name) const
		{
			const auto uniform = GetUniformObject(name);
			if(uniform)
				return UniformTable<Type>(*uniform);
			return std::nullopt;
		}

	protected:
		template <class ...Args>
		void Attach(const Shader &shader, Args &&... args)
		{
			Attach(shader);
			Attach(std::forward<Args>(args)...);
		}

		template <class ...Args>
		void Detach(const Shader &shader, Args &&... args)
		{
			Detach(shader);
			Detach(std::forward<Args>(args)...);
		}
	};
}
