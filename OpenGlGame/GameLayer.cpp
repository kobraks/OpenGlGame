#include "pch.h"
#include "GameLayer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <queue>

#include "KeyEvent.h"

#include "Light.h"

#include "Mouse.h"
#include "Keyboard.h"
#include "Model.h"
#include "ShaderProgram.h"
#include "Shader.h"
#include "Renderer.h"
#include "PerspectiveCamera.h"
#include "Application.h"
#include "ModelLoader.h"

#include "ImGuiUtils.h"
#include "ImGuiGuard.h"

#include "Transformable.h"

namespace Game
{
	bool useMouse = false;

	Pointer<VertexShader> vertexShader;
	Pointer<FragmentShader> fragmentShader;
	Pointer<ShaderProgram> shader;

	ShaderSource vertexSource;
	ShaderSource fragmentSource;

	Pointer<PerspectiveCamera> camera;

	std::array<LightInfo, MAX_LIGHTS> lights;

	std::queue<LightInfo> lightQueue;

	glm::vec3 cameraDirection(9.f, 0.f, 1.f);

	Window *window;

	float lastX = 400;
	float lastY = 300;

	float sensitivity = 0.1f;

	float yaw       = 0.f;
	float pitch     = 0.f;
	bool firstMouse = true;

	float keySensitivity = 0.03f;
	bool useKeyboard     = false;

	bool firstLoop          = true;
	bool whiteColor         = false;
	bool onlyDiffuseTexture = false;
	bool captureMouse       = false;
	bool mouseHidden        = false;

	std::string vertexLog;
	std::string fragmentLog;
	std::string programLog;

	void CameraControl();
	void ObjectsControl();
	void LightsControl();
	void ShadersControl();

	class TransformModel
	{
	public:
		Pointer<Model> Model;

		std::string Name;

		glm::vec3 Position = glm::vec3(0.f, 0.f, 0.f);
		glm::vec3 Rotate   = glm::vec3(0.f, 0.f, 0.f);
		glm::vec3 Scale    = glm::vec3(1.f, 1.f, 1.f);
		glm::vec4 Color    = glm::vec4(1.f, 1.f, 1.f, 1.f);
		glm::vec3 Origin   = glm::vec3(0.f, 0.f, 0.f);

		bool Draw = true;

		void Render(Renderer &renderer) const
		{
			if(!Draw)
				return;

			glm::mat4 rotation = translate(glm::mat4(1.f), Origin);
			rotation           = rotate(rotation, glm::radians(Rotate.x), glm::vec3(1.f, 0.f, 0.f));
			rotation           = rotate(rotation, glm::radians(Rotate.y), glm::vec3(0.f, 1.f, 0.f));
			rotation           = rotate(rotation, glm::radians(Rotate.z), glm::vec3(0.f, 0.f, 1.f));
			rotation           = translate(rotation, -Origin);

			glm::mat4 matrix = translate(glm::mat4(1.f), Position);
			matrix           = scale(matrix, Scale);
			matrix *= rotation;
			renderer.Draw(Model, matrix);
			// renderer.Draw(Model);
		}
	};

	std::vector<TransformModel> models;

	GameLayer::GameLayer() : Layer("GameLayer") {}

	void GameLayer::OnAttach()
	{
		camera = MakePointer<PerspectiveCamera>(45.f, 800.f / 600.f, 0.01f, 100.f, glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 0.f, 0.f));

		Pointer<Model> cubeModel  = ModelLoader::Load("test.obj", "../Data/Models");
		Pointer<Model> planeModel = ModelLoader::Load("test2.obj", "../Data/Models");
		Pointer<Model> cowModel   = ModelLoader::Load("Cow.3DS", "../Data/Models");

		Pointer<Model> triangleModel = MakePointer<Model>();

		std::vector<Vertex> vert;
		vert.push_back(Vertex(glm::vec3(-0.5f, -0.5f, 0.0f)));
		vert.push_back(Vertex(glm::vec3(0.5f, -0.5f, 0.0f)));
		vert.push_back(Vertex(glm::vec3(0.0f, 0.5f, 0.0f)));

		std::vector<uint32_t> in;
		in.push_back(0);
		in.push_back(1);
		in.push_back(2);

		// Pointer<Mesh> mesh = MakePointer<Mesh>(vert, in);
		// triangleModel->AddMesh(mesh);

		// std::vector<Vertex> vertices;
		// vertices.push_back(Vertex({-1.f, -0.5f, 1.f}));
		// vertices.push_back(Vertex({1.f, 1.f, 1.f}));
		// vertices.push_back(Vertex({1.f, -1.f, 1.f}));

		// std::vector<uint32_t> indices = {1, 2, 3};

		// Mesh *m = new Mesh("test", vertices, indices);

		// Pointer<Mesh> mesh = Pointer<Mesh>(m);
		// cubeModel->AddMesh(mesh);

		// models.push_back({planeModel, "Plane"});
		models.push_back({cubeModel, "Cube"});
		// models.push_back({triangleModel, "Triangle"});
		models.push_back({cowModel, "CowModel", {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.001f, 0.001f, 0.001f}});

		vertexSource   = ShaderSource::Load("shader.vert", "../Data/Shaders");
		fragmentSource = ShaderSource::Load("shader.frag", "../Data/Shaders");

		vertexShader = MakePointer<VertexShader>();
		vertexShader->SetSource(vertexSource);
		vertexShader->Compile();
		vertexLog = vertexShader->GetLog();

		fragmentShader = MakePointer<FragmentShader>();
		fragmentShader->SetSource(fragmentSource);
		fragmentShader->Compile();
		fragmentLog = fragmentShader->GetLog();

		shader = MakePointer<ShaderProgram>("Testing shader");
		shader->Attach(vertexShader);
		shader->Attach(fragmentShader);
		shader->Link();
		programLog = shader->GetLog();

		window = &Application::Get().GetWindow();

		uint32_t i = 0;
		for(auto &light : lights)
		{
			light.Active = false;
			light.Index  = i++;
		}

		lights[0].Active    = true;
		lights[0].Type      = LightType::Directional;
		lights[0].Direction = glm::vec3(0.f, -1.f, 0.f);
	}

	void GameLayer::OnDetach()
	{
		Renderer renderer;
		renderer.SetShader(nullptr);

		models.clear();

		fragmentShader = nullptr;
		vertexShader   = nullptr;

		shader = nullptr;
		window = nullptr;
	}

	void GameLayer::OnUpdate()
	{
		if(shader->IsLinked())
		{
			if(useKeyboard)
			{
				auto pos = camera->GetPosition();

				if(Keyboard::IsKeyPressed(Key::A, *window))
					pos -= keySensitivity * camera->GetRight();
				if(Keyboard::IsKeyPressed(Key::D, *window))
					pos += keySensitivity * camera->GetRight();

				if(Keyboard::IsKeyPressed(Key::Up, *window))
					pos += keySensitivity * camera->GetUp();
				if(Keyboard::IsKeyPressed(Key::Down, *window))
					pos -= keySensitivity * camera->GetUp();

				if(Keyboard::IsKeyPressed(Key::W, *window))
					pos += keySensitivity * camera->GetFront();
				if(Keyboard::IsKeyPressed(Key::S, *window))
					pos -= keySensitivity * camera->GetFront();

				camera->SetPosition(pos);
			}

			if(useMouse)
			{
				const auto &pos = Mouse::GetPosition(*window);

				if(firstMouse)
				{
					firstMouse = false;

					lastX = pos.X;
					lastY = pos.Y;
				}

				float xOffset = static_cast<float>(pos.X) - lastX;
				float yOffset = lastY - static_cast<float>(pos.Y);

				xOffset *= sensitivity;
				yOffset *= sensitivity;

				camera->Rotate(glm::vec3(yOffset, xOffset, 0));

				lastX = static_cast<float>(pos.X);
				lastY = static_cast<float>(pos.Y);
			}

			if(captureMouse)
			{
				Mouse::SetPosition(400, 300, *window);
				lastX = 400;
				lastY = 300;
			}
			Renderer renderer;
			renderer.BeginScene(*camera);
			renderer.SetShader(shader);
			shader->UniformValue("u_CameraPos", camera->GetPosition());
			shader->UniformValue("u_WhiteColor", whiteColor);
			shader->UniformValue("u_OnlyDiffuseTexture", onlyDiffuseTexture);

			if(firstLoop)
			{
				firstLoop = false;
				shader->UniformValue("u_CalcLights", true);
				renderer.BindLights();
			}

			for(const auto &light : lights)
			{
				renderer.BindLight(light);
			}

			for(const auto &model : models)
				model.Render(renderer);

			renderer.EndScene();
		}
	}

	void GameLayer::OnConstUpdate(const Time &timeStep) {}

	void GameLayer::OnImGuiRender()
	{
		ImGui::Begin("Camera");

		CameraControl();

		if(ImGui::TreeNode("ObjectsControl"))
		{
			ObjectsControl();
			ImGui::TreePop();
		}

		LightsControl();

		ImGui::End();
		ImGui::Begin("ShaderWindow");

		ShadersControl();

		ImGui::End();
	}

	void GameLayer::OnEvent(Event &event)
	{
		if(KeyReleasedEvent::GetStaticType() == event.GetEventType())
		{
			auto keyEvent = dynamic_cast<KeyReleasedEvent*>(&event);

			if(keyEvent->GetKeyCode() == Key::F5)
				captureMouse = !captureMouse;

			if(keyEvent->GetKeyCode() == Key::F6)
			{
				mouseHidden = !mouseHidden;
				window->SetCursorMode(mouseHidden ? CursorMode::Disabled : CursorMode::Normal);
			}

			if(keyEvent->GetKeyCode() == Key::F7)
				useMouse = !useMouse;

			if(keyEvent->GetKeyCode() == Key::F8)
				useKeyboard = !useKeyboard;
		}
	}

	void CameraControl()
	{
		if(ImGui::TreeNode("CameraControls"))
		{
			glm::vec3 position = camera->GetPosition();
			glm::vec3 wordUp   = camera->GetWordUp();
			glm::vec3 up       = camera->GetUp();
			glm::vec3 front    = camera->GetFront();
			glm::vec3 right    = camera->GetRight();


			float fov         = camera->GetFov();
			float nearPlane   = camera->GetNearPlane();
			float farPlane    = camera->GetFarPlane();
			float aspectRatio = camera->GetAspectRatio();

			ImGui::Checkbox("Use mouse", &useMouse);
			ImGui::Checkbox("Enable moving", &useKeyboard);
			ImGui::Checkbox("Mouse capturing", &captureMouse);

			if(ImGui::Checkbox("Mouse Hidden", &mouseHidden))
			{
				if(mouseHidden)
					window->SetCursorMode(CursorMode::Hidden);
				else
					window->SetCursorMode(CursorMode::Normal);
			}

			ImGui::NewLine();

			ImGui::DragFloat("Mouse sensitivity", &sensitivity, 0.01f, 1.f, 0.01f);
			ImGui::DragFloat("keyboard sensitivity", &keySensitivity, 0.01f);

			ImGui::NewLine();
			ImGui::Separator();
			ImGui::NewLine();
			ImGui::InputFloat3("Camera Front", &front.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat3("Camera Right", &right.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat3("Camera Up", &up.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::DragFloat3("Camera position", &position.x, 0.01f);

			ImGui::NewLine();
			ImGui::Separator();
			ImGui::NewLine();

			ImGui::DragFloat3("Word up", &wordUp.x, 0.01f, -1.f, 1.f);

			if(ImGui::Button("Reset word up"))
				wordUp = glm::vec3(0.f, 1.f, 0.f);

			ImGui::NewLine();
			ImGui::Separator();
			ImGui::NewLine();

			ImGui::DragFloat("Fov", &fov);
			ImGui::DragFloat("Near plane", &nearPlane);
			ImGui::DragFloat("Far plane", &farPlane);
			ImGui::DragFloat("Aspect ratio", &aspectRatio);

			if(fov != camera->GetFov())
				camera->SetFov(fov);

			if(aspectRatio != camera->GetAspectRatio())
				camera->SetAspectRatio(aspectRatio);

			if(farPlane != camera->GetFarPlane())
				camera->SetFarPlane(farPlane);

			if(nearPlane != camera->GetNearPlane())
				camera->SetNearPlane(nearPlane);

			if(camera->GetPosition() != position)
				camera->SetPosition(position);

			if(camera->GetWordUp() != wordUp)
				camera->SetUp(normalize(wordUp));

			ImGui::TreePop();
		}
	}

	void MaterialControl(Pointer<Material> material)
	{
		if(ImGui::TreeNode(material->Name.c_str()))
		{
			ImGui::DragFloat("Shininess", &material->Shininess, 0.01f);
			ImGui::NewLine();
			ImGui::Separator();
			ImGui::NewLine();
			ImGui::ColorPicker3("DiffuseColor: ", &material->DiffuseColor.x);
			ImGui::NewLine();
			ImGui::Separator();
			ImGui::NewLine();
			ImGui::ColorPicker3("SpecularColor: ", &material->SpecularColor.x);
			ImGui::NewLine();
			ImGui::Separator();
			ImGui::NewLine();

			auto texture = material->DiffuseTexture;
			if(texture)
			{
				auto size = texture->Size();
				glm::vec2 s{static_cast<float>(size.Width), static_cast<float>(size.Height)};

				ImGui::InputFloat2("Size", &s.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
				Texture::IdType id = texture->Id();
				ImGui::Image((void*)static_cast<intptr_t>(id), {400, 400});
			}

			texture = material->SpecularTexture;
			if(texture)
			{
				auto size = texture->Size();
				glm::vec2 s{static_cast<float>(size.Width), static_cast<float>(size.Height)};

				ImGui::InputFloat2("Size", &s.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
				Texture::IdType id = texture->Id();
				ImGui::Image((void*)static_cast<intptr_t>(id), {400, 400});
			}

			ImGui::TreePop();
		}
	}

	void ObjectsControl()
	{
		for(auto &model : models)
		{
			if(ImGui::TreeNode(fmt::format("{}", model.Name).c_str()))
			{
				ImGui::Checkbox(fmt::format("{}, draw", model.Name).c_str(), &model.Draw);
				ImGui::DragFloat3(fmt::format("{} Position", model.Name).c_str(), &model.Position.x, 0.01f);
				ImGui::DragFloat3(fmt::format("{} Scale", model.Name).c_str(), &model.Scale.x, 0.1f);
				ImGui::DragFloat3(fmt::format("{} Rotate", model.Name).c_str(), &model.Rotate.x, 1.00f);
				ImGui::DragFloat3(fmt::format("{} RotationOrigin", model.Name).c_str(), &model.Origin.x, 0.1f);

				if(ImGui::TreeNode("MaterialControl"))
				{
					for(auto &mesh : *model.Model)
						MaterialControl(mesh->GetMaterial());

					ImGui::TreePop();
				}

				ImGui::TreePop();
			}
		}
	}

	void LightControl(uint32_t i, LightInfo &light)
	{
		const char *items[] = {"Unknown", "Spot", "Point", "Directional"};

		if(ImGui::TreeNode(fmt::format("Light {}", i).c_str()))
		{
			ImGui::Checkbox("Active", &light.Active);

			ImGui::DragFloat3("Position", &light.Position.x);
			ImGui::DragFloat3("Direction", &light.Direction.x);

			ImGui::DragFloat("Linear", &light.Linear, 0.01f);
			ImGui::DragFloat("Constant", &light.Constant, 0.01f);
			ImGui::DragFloat("Quadratic", &light.Quadratic, 0.01f);

			ImGui::DragFloat("CutOff", &light.CutOff, 0.01f);
			ImGui::DragFloat("OuterCutOff", &light.OuterCutOff, 0.01f);

			int32_t type = static_cast<int32_t>(light.Type);
			ImGui::Combo("Light type", &type, items, IM_ARRAYSIZE(items));
			light.Type = static_cast<LightType>(type);


			ImGui::ColorPicker3("Diffuse", &light.DiffuseColor.x, ImGuiColorEditFlags_DisplayRGB);
			ImGui::ColorPicker3("Ambient", &light.AmbientColor.x, ImGuiColorEditFlags_DisplayRGB);
			ImGui::ColorPicker3("Specular", &light.SpecularColor.x, ImGuiColorEditFlags_DisplayRGB);

			ImGui::TreePop();
		}
	}

	void LightsControl()
	{
		if(ImGui::TreeNode("LightInfo"))
		{
			uint32_t i = 0;
			for(auto &light : lights)
			{
				LightControl(i++, light);
			}

			ImGui::TreePop();
		}
	}

	void ProcessCapabilities();

	void ShadersControl()
	{
		if(ImGui::TreeNode("VertexShader"))
		{
			bool check = vertexShader->IsCompiled();

			std::string source = vertexSource.GetSource();
			InputTextMultiline("Source", source, ImVec2(0, 0), ImGuiInputTextFlags_ReadOnly);
			InputTextMultiline("Log", vertexLog, ImVec2(0, 0), ImGuiInputTextFlags_ReadOnly);

			ImGui::Checkbox("Is compiled", &check);
			ImGui::SameLine();
			if(ImGui::Button("Recompile"))
			{
				vertexShader->Compile();
				vertexLog = vertexShader->GetLog();
			}
			ImGui::SameLine();
			if(ImGui::Button("ReloadSource"))
			{
				vertexSource = ShaderSource::Load("shader.vert", "../Data/Shaders");
				vertexShader->SetSource(vertexSource);
			}

			ImGui::TreePop();
		}
		if(ImGui::TreeNode("FragmentShader"))
		{
			bool check = fragmentShader->IsCompiled();

			std::string source = fragmentSource.GetSource();
			InputTextMultiline("Source", source, ImVec2(0, 0), ImGuiInputTextFlags_ReadOnly);
			InputTextMultiline("Log", fragmentLog, ImVec2(0, 0), ImGuiInputTextFlags_ReadOnly);

			ImGui::Checkbox("Is compiled", &check);
			ImGui::SameLine();
			if(ImGui::Button("Recompile"))
			{
				fragmentShader->Compile();
				fragmentLog = fragmentShader->GetLog();
			}
			ImGui::SameLine();
			if(ImGui::Button("ReloadSource"))
			{
				fragmentSource = ShaderSource::Load("shader.frag", "../Data/Shaders");
				fragmentShader->SetSource(fragmentSource);
			}

			ImGui::TreePop();
		}
		if(ImGui::TreeNode("ShaderProgram"))
		{
			InputTextMultiline("Log", programLog, ImVec2(0, 0), ImGuiInputTextFlags_ReadOnly);

			bool check = shader->IsLinked();
			ImGui::Checkbox("Is linked", &check);
			ImGui::SameLine();

			check = shader->IsAttached(fragmentShader);
			ImGui::Checkbox("Fragment shader attached", &check);

			ImGui::SameLine();
			check = shader->IsAttached(vertexShader);
			ImGui::Checkbox("Vertex shader attached", &check);

			if(ImGui::Button("ReLink"))
			{
				shader->Link();
				programLog = shader->GetLog();
			}

			ImGui::Checkbox("White color", &whiteColor);
			ImGui::Checkbox("Only diffuse texture", &onlyDiffuseTexture);

			if(ImGui::TreeNode("Uniforms"))
			{
				const auto &activeUniforms = shader->GetActiveUniforms();
				for(size_t i = 0; i < activeUniforms.size(); ++i)
				{
					const auto &uniform = activeUniforms[i];
					Text("{} Uniform:", i);
					Text("  Name: {}, Size: {}, Type: {}", uniform.Name, uniform.Size, static_cast<const uint32_t>(uniform.Type));
				}

				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		if(ImGui::TreeNode("OpenGlCapabilities"))
		{
			ProcessCapabilities();
			ImGui::TreePop();
		}
	}

#define IS_ENABLED(c)\
	isEnabled = functions.IsEnabled(Capability::c);\
	if(ImGui::Checkbox(fmt::format("Is {} enabled", #c).c_str(), &isEnabled))\
	{\
		if (isEnabled) \
			functions.Enable(Capability::c);\
		else\
			functions.Disable(Capability::c);\
	}

	int cast(PolygonFacing facing)
	{
		switch(facing)
		{
			case PolygonFacing::Back: return 0;
			case PolygonFacing::Front: return 1;
			case PolygonFacing::FrontBack: return 2;
		}
	}

	int cast(FrontFace facing)
	{
		switch(facing)
		{
			case FrontFace::ClockWise: return 0;
			case FrontFace::CounterClockWise: return 1;
		}
	}

	void ProcessCapabilities()
	{
		auto functions = window->GetFunctions();

		bool isEnabled = false;

		const char *facingItems[]        = {"ClockWise", "CounterClockWise"};
		const char *polygonFacingItems[] = {"Back", "Front", "FrontAndBack"};

		int item = cast(functions.GetFrontFace());
		ImGui::Combo("FrontFacing", &item, facingItems, IM_ARRAYSIZE(facingItems));
		if(item != cast(functions.GetFrontFace()))
		{
			if(item == 0)
				functions.SetFrontFace(FrontFace::ClockWise);
			else if(item == 1)
				functions.SetFrontFace(FrontFace::CounterClockWise);
		}

		item = cast(functions.GetPolygonFacing());
		ImGui::Combo("PolygonFacing", &item, polygonFacingItems, IM_ARRAYSIZE(polygonFacingItems));
		if(item != cast(functions.GetPolygonFacing()))
		{
			if(item == 2)
				functions.SetPolygonFacing(PolygonFacing::FrontBack);
			else if(item == 0)
				functions.SetPolygonFacing(PolygonFacing::Back);
			else if(item == 1)
				functions.SetPolygonFacing(PolygonFacing::Front);
		}

		IS_ENABLED(Blend);
		IS_ENABLED(ClipDistance0);
		IS_ENABLED(ClipDistance1);
		IS_ENABLED(ClipDistance2);
		IS_ENABLED(ClipDistance3);
		IS_ENABLED(ClipDistance4);
		IS_ENABLED(ClipDistance5);
		IS_ENABLED(ClipDistance6);
		IS_ENABLED(ClipDistance7);
		IS_ENABLED(ColorLogicOp);
		IS_ENABLED(CullFace);
		IS_ENABLED(DebugOutput);
		IS_ENABLED(DebugOutputSynchronous);
		IS_ENABLED(DepthClamp);
		IS_ENABLED(DepthTest);
		IS_ENABLED(Dither);
		IS_ENABLED(FramebufferSrgb);
		IS_ENABLED(LineSmooth);
		IS_ENABLED(Multisample);
		IS_ENABLED(PolygonSmooth);
		IS_ENABLED(PolygonOffsetFill);
		IS_ENABLED(PolygonOffsetLine);
		IS_ENABLED(PolygonOffsetPoint);
		IS_ENABLED(ProgramPointSize);
		IS_ENABLED(PrimitiveRestart);
		IS_ENABLED(PrimitiveRestartFixedIndex);
		IS_ENABLED(RasterizerDiscard);
		IS_ENABLED(SampleAlphaToCoverage);
		IS_ENABLED(SampleAlphaToOne);
		IS_ENABLED(SampleShading);
		IS_ENABLED(SampleMask);
		IS_ENABLED(ScissorTest);
		IS_ENABLED(StencilTest);
		IS_ENABLED(TextureCubeMapSeamless);
	}
}
