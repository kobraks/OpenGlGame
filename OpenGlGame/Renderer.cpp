#include "pch.h"
#include "Renderer.h"

#include "GLCheck.h"
#include "VertexArray.h"
#include "Camera.h"
#include "ShaderProgram.h"
#include "Model.h"

namespace Game
{
	Scope<Renderer::SceneData> Renderer::s_SceneData = MakeScope<SceneData>();

	void Renderer::BeginScene(const Camera &camera)
	{
		s_SceneData->ViewProjectionMatrix = camera.GetViewProjection();
	}

	void Renderer::EndScene() {}

	void Renderer::SetShader(const Pointer<ShaderProgram> &shader)
	{
		s_SceneData->Shader = shader;

		if(!shader)
			return;

		shader->Use();
		shader->UniformValue("u_ProjectionViewMatrix", s_SceneData->ViewProjectionMatrix);
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height) { }

	void Renderer::Draw(const Pointer<VertexArray> &array, Primitive primitive, const int64_t count)
	{
		ASSERT(array, "Uninitialized Vertex Array");

		array->Bind();

		GL_CHECK(
		         glDrawElements(static_cast<GLenum>(primitive), count < 0 ? array->GetIndexBuffer()->Count() : static_cast<GLsizei>(count), static_cast<GLenum>(DataType::UnsignedInt)
			       , nullptr)
		        );
	}

	void Renderer::Draw(const Pointer<VertexArray> &array, const std::vector<uint32_t> &indices, Primitive primitive, int64_t count)
	{
		ASSERT(array, "Uninitialized Vertex Array");

		array->Bind();

		GL_CHECK(
		         glDrawElements(static_cast<GLenum>(primitive), count < 0 ? static_cast<int32_t>(indices.size()) : static_cast<GLsizei>(count), static_cast<GLenum>(DataType::
			         UnsignedInt), indices.data())
		        );

		// glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());
	}

	void Renderer::Draw(const Pointer<VertexArray> &vertexArray, const glm::mat4 &transform)
	{
		if(s_SceneData->Shader)
		{
			s_SceneData->Shader->UniformValue("u_Transform", transform);
			s_SceneData->Shader->UniformValue("u_InvertedTransform", glm::inverse(transform));
		}
		else
		{
			ASSERT(false, "You have not set any shader program");
		}

		Draw(vertexArray, Primitive::Triangles, -1);
	}

	void Renderer::Draw(const Pointer<VertexArray> &vertexArray, const std::vector<uint32_t> &indices, const glm::mat4 &transform)
	{
		if(s_SceneData->Shader)
		{
			s_SceneData->Shader->UniformValue("u_Transform", transform);
			s_SceneData->Shader->UniformValue("u_InvertedTransform", glm::inverse(transform));
		}
		else
		{
			ASSERT(false, "You have not set any shader program");
		}

		Draw(vertexArray, indices, Primitive::Triangles, -1);
	}

	void Renderer::Draw(const Pointer<Model> &model, const glm::mat4 &transform)
	{
		const auto matrix = transform * model->GetTransform() ;

		for(const auto &mesh : *model)
			Draw(mesh, matrix);
	}

	void Renderer::Draw(const Pointer<Mesh> &mesh, const glm::mat4 &transform)
	{
		BindMaterial(mesh->GetMaterial());
		Draw(mesh->GetVertexArray(), transform * mesh->GetTransform());
	}

	void Renderer::SetLight(const Light &light)
	{
		const auto lightInfo = light.GetInfo();

		if (lightInfo.Type == LightType::Unknown)
			return;
		
		ASSERT(lightInfo.Index < MAX_LIGHTS, "You want to bing too many lights");
		BindLight(lightInfo);
	}

	void Renderer::BindLights()
	{
		for(const auto &light : s_SceneData->Lights)
			BindLight(light);
	}

	void Renderer::BindMaterial(const Pointer<Material> &material)
	{
		if(!material)
		{
			return;
		}

		int32_t textureUnit = 0;
		// BindMaterialTexture(s_SceneData->Shader, material->Diffuse, "u_DiffuseTex", textureUnit);
		// BindMaterialTexture(s_SceneData->Shader, material->Specular, "u_SpecularTex", textureUnit);
		// BindMaterialTexture(s_SceneData->Shader, material->Ambient, "u_AmbientTex", textureUnit);
		// BindMaterialTexture(s_SceneData->Shader, material->Height, "u_HeightTex", textureUnit);

		auto &shader = s_SceneData->Shader;

		if(material->SpecularTexture)
		{
			shader->UniformValue("u_Material.SpecularTex", *material->SpecularTexture, textureUnit++);
			shader->UniformValue("u_Material.HasSpecularTex", true);
		}
		else
			shader->UniformValue("u_Material.HasSpecularTex", false);

		if(material->DiffuseTexture)
		{
			shader->UniformValue("u_Material.DiffuseTex", *material->DiffuseTexture, textureUnit++);
			shader->UniformValue("u_Material.HasDiffuseTex", true);
		}
		else
			shader->UniformValue("u_Material.HasDiffuseTex", false);

		shader->UniformValue("u_Material.Diffuse", material->DiffuseColor);
		shader->UniformValue("u_Material.Specular", material->SpecularColor);
		shader->UniformValue("u_Material.Shininess", material->Shininess);
	}

	void Renderer::BindLight(const LightInfo &light)
	{
		if(light.Index >= MAX_LIGHTS || light.Type == LightType::Unknown)
			return;

		s_SceneData->Lights[light.Index] = light;
		BindLight(s_SceneData->Shader, light);
	}

	void Renderer::BindLight(Pointer<ShaderProgram> &shader, const LightInfo &light)
	{
		if(shader)
		{
			const std::string name = fmt::format("u_Light[{}].", light.Index);

			shader->UniformValue(fmt::format("{}{}", name, "Active"), light.Active);
			shader->UniformValue(fmt::format("{}{}", name, "Position"), light.Position);
			shader->UniformValue(fmt::format("{}{}", name, "Direction"), light.Direction);
			shader->UniformValue(fmt::format("{}{}", name, "Ambient"), glm::vec3(light.AmbientColor));
			shader->UniformValue(fmt::format("{}{}", name, "Diffuse"), glm::vec3(light.DiffuseColor));
			shader->UniformValue(fmt::format("{}{}", name, "Specular"), glm::vec3(light.SpecularColor));
			shader->UniformValue(fmt::format("{}{}", name, "CutOff"), light.CutOff);
			shader->UniformValue(fmt::format("{}{}", name, "OuterCutOff"), light.OuterCutOff);
			shader->UniformValue(fmt::format("{}{}", name, "Constant"), light.Constant);
			shader->UniformValue(fmt::format("{}{}", name, "Linear"), light.Linear);
			shader->UniformValue(fmt::format("{}{}", name, "Quadratic"), light.Quadratic);
			shader->UniformValue(fmt::format("{}{}", name, "Type"), static_cast<int32_t>(light.Type));
		}
		else
		{
			ASSERT(false, "You have not set any shader program");
		}
	}

	void Renderer::BindMaterialTexture(
		Pointer<ShaderProgram> shader,
		const std::vector<Pointer<Texture>> &textures,
		const std::string &name,
		int32_t &textureUnit
		)
	{
		ASSERT(shader, "No shader bound");
		if(!shader)
			return;

		for(const auto &texture : textures)
		{
			if(texture)
			{
				shader->UniformValue(fmt::format("{}{}", name, textureUnit == 0 ? "" : std::to_string(textureUnit)), *texture, textureUnit);

				textureUnit++;
			}
		}
	}
}
