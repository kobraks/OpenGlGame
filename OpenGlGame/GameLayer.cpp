#include "pch.h"
#include "GameLayer.h"

#include "Model.h"
#include "ShaderProgram.h"
#include "Shader.h"
#include "Renderer.h"
#include "PerspectiveCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Game
{
	Pointer<ShaderProgram> shader;
	Pointer<PerspectiveCamera> camera;

	class TransformModel
	{
	public:
		Pointer<Model> Model;

		std::string Name;

		glm::vec3 Position = glm::vec3(0.f, 0.f, 0.f);
		glm::vec3 Rotate   = glm::vec3(0.f, 0.f, 0.f);
		glm::vec3 Scale    = glm::vec3(1.f, 1.f, 1.f);
		glm::vec4 Color    = glm::vec4(1.f, 1.f, 1.f, 1.f);

		bool Draw = true;

		void Render(Renderer &renderer) const
		{
			if(!Draw)
				return;

			glm::mat4 matrix = translate(glm::mat4(1.f), Position);
			matrix           = scale(matrix, Scale);
			matrix           = rotate(matrix, glm::radians(Rotate.x), glm::vec3(1.f, 0.f, 0.f));
			matrix           = rotate(matrix, glm::radians(Rotate.y), glm::vec3(0.f, 1.f, 0.f));
			matrix           = rotate(matrix, glm::radians(Rotate.z), glm::vec3(0.f, 0.f, 1.f));

			shader->UniformValue("u_Color", Color);

			renderer.Draw(Model, matrix);
		}
	};

	std::vector<TransformModel> models;

	GameLayer::GameLayer() : Layer("GameLayer") {}

	void GameLayer::OnAttach()
	{
		camera = MakePointer<PerspectiveCamera>(45.f, 800.f / 600.f, 0.01f, 100.f, glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 0.f, 0.f));

		Pointer<Model> cubeModel;
		Pointer<Model> planeModel;
		Pointer<Model> triangleModel;

		cubeModel  = Model::Load("test.obj", "../Data/Models");
		planeModel = Model::Load("test2.obj", "../Data/Models");

		triangleModel = MakePointer<Model>();

		std::vector<Vertex> vert;
		vert.push_back(Vertex(glm::vec3(-0.5f, -0.5f, 0.0f)));
		vert.push_back(Vertex(glm::vec3(0.5f, -0.5f, 0.0f)));
		vert.push_back(Vertex(glm::vec3(0.0f, 0.5f, 0.0f)));

		std::vector<uint32_t> in;
		in.push_back(0);
		in.push_back(1);
		in.push_back(2);

		Pointer<Mesh> mesh = MakePointer<Mesh>(vert, in);
		triangleModel->AddMesh(mesh);

		// std::vector<Vertex> vertices;
		// vertices.push_back(Vertex({-1.f, -0.5f, 1.f}));
		// vertices.push_back(Vertex({1.f, 1.f, 1.f}));
		// vertices.push_back(Vertex({1.f, -1.f, 1.f}));

		// std::vector<uint32_t> indices = {1, 2, 3};

		// Mesh *m = new Mesh("test", vertices, indices);

		// Pointer<Mesh> mesh = Pointer<Mesh>(m);
		// cubeModel->AddMesh(mesh);
	
		models.push_back({planeModel, "Plane"});
		models.push_back({cubeModel, "Cube"});
		models.push_back({triangleModel, "Triangle"});

		const auto vertexShaderSource   = ShaderSource::Load("shader.vert", "../Data/Shaders");
		const auto fragmentShaderSource = ShaderSource::Load("shader.frag", "../Data/Shaders");

		Pointer<Shader> vertexShader = MakePointer<VertexShader>(vertexShaderSource);
		LOG_DEBUG(vertexShader->GetLog());

		Pointer<Shader> fragmentShader = MakePointer<FragmentShader>(fragmentShaderSource);
		LOG_DEBUG(fragmentShader->GetLog());
		
		shader = MakePointer<ShaderProgram>("TestingShader", vertexShader, fragmentShader);
	}

	void GameLayer::OnDetach()
	{
		Renderer renderer;
		renderer.SetShader(nullptr);

		models.clear();

		shader = nullptr;
	}

	void GameLayer::OnUpdate()
	{
		Renderer renderer;
		renderer.BeginScene(*camera);
		renderer.SetShader(shader);

		for (const auto& model : models)
			model.Render(renderer);
		
		renderer.EndScene();
	}

	void GameLayer::OnConstUpdate(const Time &timeStep) {}

	void GameLayer::OnImGuiRender()
	{
		ImGui::Begin("Camera");

		glm::vec3 position = camera->GetPosition();
		glm::vec3 target   = camera->GetTarget();
		glm::vec3 up       = camera->GetUp();

		float fov         = camera->GetFov();
		float nearPlane   = camera->GetNearPlane();
		float farPlane    = camera->GetFarPlane();
		float aspectRatio = camera->GetAspectRatio();

		ImGui::DragFloat3("Camera position", &position.x, 0.01f);
		ImGui::DragFloat3("Camera Target", &target.x, 0.01f, -10.f, 10.f);
		ImGui::DragFloat3("Camera Up", &up.x, 0.01f, -1.f, 1.f);

		ImGui::DragFloat("Fov", &fov);
		ImGui::DragFloat("Near plane", &nearPlane);
		ImGui::DragFloat("Far plane", &farPlane);
		ImGui::DragFloat("Aspect ratio", &aspectRatio);

		for (auto& model : models)
		{
			ImGui::Checkbox(fmt::format("{}, draw", model.Name).c_str(), &model.Draw);
			ImGui::DragFloat3(fmt::format("{} Position", model.Name).c_str(), &model.Position.x, 0.01f);
			ImGui::DragFloat3(fmt::format("{} Scale", model.Name).c_str(), &model.Scale.x, 0.1f);
			ImGui::DragFloat3(fmt::format("{} Rotate", model.Name).c_str(), &model.Rotate.x, 1.00f);
		}

		ImGui::End();

		if(fov != camera->GetFov())
			camera->SetFov(fov);

		if(aspectRatio != camera->GetAspectRatio())
			camera->SetAspectRatio(aspectRatio);

		if(farPlane != camera->GetFarPlane())
			camera->SetFarPlane(farPlane);

		if(nearPlane != camera->GetNearPlane())
			camera->SetNearPlane(nearPlane);

		camera->SetPosition(position);
		camera->SetTarget(target);
		camera->SetUp(up);
	}

	void GameLayer::OnEvent(Event &event) {}
}
