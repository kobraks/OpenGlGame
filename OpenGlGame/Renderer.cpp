#include "pch.h"
#include "Renderer.h"

#include "GLCheck.h"
#include "VertexArray.h"
#include "Camera.h"
#include "ShaderProgram.h"
#include "Model.h"

#include "Buffer.h"

namespace Game
{
	Scope<Renderer::SceneData> Renderer::s_SceneData = MakeScope<SceneData>();

	void Renderer::BeginScene()
	{
		Init();
	}

	void Renderer::BeginScene(const Camera &camera, const glm::mat4 &transform)
	{
		BeginScene();

		glm::mat4 viewProj = camera.GetProjection() * glm::inverse(transform);
	}

	void Renderer::EndScene() {}

	void Renderer::SetShader(const Pointer<ShaderProgram> &shader)
	{
		s_SceneData->Shader = shader;

		if(!shader)
			return;

		shader->Use();

		shader->UniformValue("u_ProjectionViewMatrix", s_SceneData->ViewProjectionMatrix);
		shader->UniformValue("u_ViewMatrix", s_SceneData->ViewMatrix);
		shader->UniformValue("u_Projection", s_SceneData->ProjectionMatrix);
		const auto index = s_SceneData->Shader->GetUniformBlockIndex("u_Lights");
		if(index != ShaderProgram::INVALID_UNIFORM_BLOCK_INDEX)
		{
			s_SceneData->LightsBuffer->Bind();
			GL_CHECK(glBindBufferBase(GL_UNIFORM_BUFFER, index, s_SceneData->LightsBuffer->Id()));
		}
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height) { }

	void Renderer::Draw(const Pointer<VertexArray> &array, Primitive primitive, const int64_t count)
	{
		ASSERT(array, "Uninitialized Vertex Array");

		array->Bind();

		GL_CHECK(
		         glDrawElements(static_cast<GLenum>(primitive), count < 0 ? array->GetIndexBuffer()->Count() : static_cast<GLsizei>(count), static_cast<GLenum>(
			         DataType::UnsignedInt) , nullptr)
		        );
	}

	void Renderer::Draw(const Pointer<VertexArray> &array, const std::vector<uint32_t> &indices, Primitive primitive, int64_t count)
	{
		ASSERT(array, "Uninitialized Vertex Array");

		array->Bind();

		GL_CHECK(
		         glDrawElements(static_cast<GLenum>(primitive), count < 0 ? static_cast<int32_t>(indices.size()) : static_cast<GLsizei>(count), static_cast<
			         GLenum>(DataType:: UnsignedInt), indices.data())
		        );

		// glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());
	}

	void Renderer::Draw(const Pointer<VertexArray> &vertexArray, const glm::mat4 &transform)
	{
		if(s_SceneData->Shader)
		{
			s_SceneData->Shader->UniformValue("u_Transform", transform);
			s_SceneData->Shader->UniformValue("u_InvertedTransform", inverse(transform));
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
			s_SceneData->Shader->UniformValue("u_InvertedTransform", inverse(transform));
		}
		else
		{
			ASSERT(false, "You have not set any shader program");
		}

		Draw(vertexArray, indices, Primitive::Triangles, -1);
	}

	void Renderer::Draw(const Pointer<Model> &model, const glm::mat4 &transform)
	{
		const auto matrix = transform * model->GetTransform();

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

		if(lightInfo.Type == LightType::Unknown)
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
		shader->UniformValue("u_Material.ShininessStrength", material->ShininessStrength);
	}

	constexpr size_t SizeOfStruct(size_t baseSize)
	{
		size_t sum = 0;
		while(sum < baseSize)
			sum += sizeof(float) * 4;

		return sum;
	}

	void Renderer::Init()
	{
		static bool init = false;

		if(!init)
		{
			constexpr size_t size = SizeOfStruct(sizeof(float) * (7 + 5 * 4)) * MAX_LIGHTS;

			s_SceneData->LightsBuffer = MakePointer<UniformBuffer>(size, BufferUsage::DynamicDraw);

			for(auto &light : s_SceneData->Lights)
				light.Active = false;
		}
	}

	template <typename T>
	constexpr size_t SizeOfType()
	{
		size_t i    = 0;
		size_t size = 0;
		while(size < sizeof(T))
		{
			size += sizeof(float);
			i ++;
		}

		if (i > 1 && i % 2 != 0)
			size += sizeof(float);
		
		return size;
	}

	template <typename T>
	void Set(char *buff, const T &value, size_t &offset)
	{
		std::memcpy(buff + offset, &value, sizeof(T));

		offset += SizeOfType<T>();
	}

	void Renderer::BindLight(Pointer<UniformBuffer> buffer, const LightInfo &light)
	{
		constexpr size_t size = SizeOfStruct(sizeof(float) * (7 + 5 * 4)); //Size of one struct of LightInfo in shader
		char buff[size];

		size_t offset = 0;
		Set(buff, light.Active, offset);
		Set(buff, light.Type, offset);
		offset = 16;

		Set(buff, light.Position, offset);
		Set(buff, light.Direction, offset);

		Set(buff, light.DiffuseColor, offset);
		Set(buff, light.AmbientColor, offset);
		Set(buff, light.SpecularColor, offset);

		offset -= 4;
		Set(buff, light.CutOff, offset);
		Set(buff, light.OuterCutOff, offset);
		
		Set(buff, light.Constant, offset);
		Set(buff, light.Linear, offset);
		Set(buff, light.Quadratic, offset);

		buffer->Set(buff, size, size * light.Index);
	}

	void Renderer::BindLight(const LightInfo &light)
	{
		if(light.Index >= MAX_LIGHTS || light.Type == LightType::Unknown)
			return;

		s_SceneData->Lights[light.Index] = light;
		BindLight(s_SceneData->LightsBuffer, light);
	}

	/*void Renderer::BindLight(Pointer<ShaderProgram> &shader, const LightInfo &light)
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
	}*/

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
