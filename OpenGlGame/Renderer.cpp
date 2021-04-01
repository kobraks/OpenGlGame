#include "pch.h"
#include "Renderer.h"

#include "GLCheck.h"
#include "VertexArray.h"
#include "Camera.h"
#include "ShaderProgram.h"

namespace Game
{
	Scope<Renderer::SceneData> Renderer::s_SceneData;

	void Renderer::BeginScane(const Camera &camera)
	{
		s_SceneData->ViewProjectionMatrix = camera.GetViewProjection();
	}

	void Renderer::EndScene() {}
	
	void Renderer::SetShader(const Pointer<ShaderProgram> &shader)
	{
		s_SceneData->Shader = shader;
		
		shader->UniformValue("u_ProjectionViewMatrix", s_SceneData->ViewProjectionMatrix);
		shader->Use();
	}

	void Renderer::Draw(const Pointer<VertexArray> &array, Primitive primitive, const int64_t count)
	{
		ASSERT(array, "Uninitialized Vertex Array");
		
		array->Bind();
		GL_CHECK(
		         glDrawElements(static_cast<GLenum>(primitive), count < 0 ? array->GetIndexBuffer()->Count() : count, static_cast<GLenum>(DataType::UnsignedInt),
			         nullptr)
		        );
	}

	void Renderer::Draw(const Pointer<VertexArray> &vertexArray, const glm::mat4 &transform)
	{
		if (s_SceneData->Shader)
		{
			s_SceneData->Shader->UniformValue("u_Transform", transform);
		}
		else
		{
			ASSERT(false, "You have not set any shader program");
		}
		
		Draw(vertexArray);
	}

	void Renderer::Draw(const Pointer<Model> &model, const glm::mat4 &transform) const
	{
	}
}
