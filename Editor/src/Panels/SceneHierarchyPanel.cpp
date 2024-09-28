#include "SceneHierarchyPanel.h"

#include "Engine/ImGui/ImGuiUtils.h"
#include "Engine/Scene/Components.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

#include <cstring>

namespace Game
{
	extern const std::filesystem::path g_AssetPath;

	static void DrawVec3Control(
		const std::string &label,
		glm::vec3 &values,
		float resetValue  = 0.f,
		float columnWidth = 100.f
		)
	{
		ImGuiIO &io   = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

		float lineHeight  = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.f;
		ImVec2 buttonSize = {lineHeight + 3.f, lineHeight};

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
		ImGui::PushFont(boldFont);
		if(ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
		ImGui::PushFont(boldFont);
		if(ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.35f, 0.9f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
		ImGui::PushFont(boldFont);
		if(ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	template <typename T, typename UIFunction>
	static void DrawComponent(const std::string &name, Engine::Entity entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
			ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

		if(entity.HasComponent<T>())
		{
			auto &component               = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar();

			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if(ImGui::Button("+", ImVec2{lineHeight, lineHeight}))
				ImGui::OpenPopup("ComponentSettings");

			bool removeComponent = false;
			if(ImGui::BeginPopup("ComponentSettings"))
			{
				if(ImGui::MenuItem("Remove Component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			if(open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}

			if(removeComponent)
				entity.RemoveComponent<T>();
		}
	}

	SceneHierarchyPanel::SceneHierarchyPanel(const Engine::Ref<Engine::Scene> &context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Engine::Ref<Engine::Scene> &context)
	{
		m_Context          = context;
		m_SelectionContext = {};
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		if(m_Context)
		{
			/*m_Context->m_Registry.each(
			                           [&](auto entityId)
			                           {
				                           Entity entity{entityId, m_Context.get()};
				                           DrawEntityNode(entity);
			                           }
			                          );*/

			if(ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
				m_SelectionContext = {};

			if(ImGui::BeginPopupContextWindow(
			                                  nullptr,
			                                  ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems
			                                 ))
			{
				if(ImGui::MenuItem("Create Empty Entity"))
					m_Context->CreateEntity("Empty Entity");

				ImGui::EndPopup();
			}
		}

		ImGui::End();

		ImGui::Begin("Properties");
		if(m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::SetSelectedEntity(Engine::Entity entity)
	{
		m_SelectionContext = entity;
	}

	void SceneHierarchyPanel::DrawEntityNode(Engine::Entity entity)
	{
		auto &tag = entity.GetComponent<Engine::TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) |
			ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		bool opened = ImGui::TreeNodeEx(
		                                (void*)static_cast<uint64_t>(static_cast<uint32_t>(entity)),
		                                flags,
		                                tag.c_str()
		                               );

		if(ImGui::IsItemClicked())
			m_SelectionContext = entity;

		bool entityDeleted = false;
		if(ImGui::BeginPopupContextItem())
		{
			if(ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

			ImGui::EndPopup();
		}

		if(opened)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			bool opened              = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());

			if(opened)
				ImGui::TreePop();
			ImGui::TreePop();
		}

		if(entityDeleted)
		{
			m_Context->DestroyEntity(entity);
			if(m_SelectionContext == entity)
				m_SelectionContext = {};
		}
	}

	void SceneHierarchyPanel::DrawComponents(Engine::Entity entity)
	{
		if(entity.HasComponent<Engine::TagComponent>())
		{
			auto &tag = entity.GetComponent<Engine::TagComponent>().Tag;

			Engine::InputText("##Tag", tag);
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if(ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if(ImGui::BeginPopup("AddComponent"))
		{
			DisplayAddComponentEntry<Engine::CameraComponent>("Camera");

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();


		DrawComponent<Engine::TransformComponent>(
		                                          "Transform",
		                                          entity,
		                                          [](auto &component)
		                                          {
			                                          DrawVec3Control("Translation", component.Translation);
			                                          glm::vec3 rotation = glm::degrees(component.Rotation);
			                                          DrawVec3Control("Rotation", rotation);
			                                          component.Rotation = radians(rotation);
			                                          DrawVec3Control("Scale", component.Scale, 1.f);
		                                          }
		                                         );

		DrawComponent<Engine::CameraComponent>(
		                                       "Camera",
		                                       entity,
		                                       [](auto &component)
		                                       {
			                                       auto &camera = component.Camera;

			                                       Engine::ToggleButton("Primary", &component.Primary);

			                                       const char *projectionTypeString[]      = {"Perspective", "Orthographic"};
			                                       const char *currentProjectionTypeString = projectionTypeString[static_cast<
				                                       int>(camera.GetProjectionType())];

			                                       if(ImGui::BeginCombo("Projection", currentProjectionTypeString))
			                                       {
				                                       for(int i = 0; i < 2; ++i)
				                                       {
					                                       bool isSelected = currentProjectionTypeString == projectionTypeString
						                                       [i];
					                                       if(ImGui::Selectable(projectionTypeString[i], isSelected))
					                                       {
						                                       currentProjectionTypeString = projectionTypeString[i];
						                                       camera.SetProjectionType(
						                                                                static_cast<Engine::SceneCamera::ProjectionType>
						                                                                (i)
						                                                               );
					                                       }

					                                       if(isSelected)
						                                       ImGui::SetItemDefaultFocus();
				                                       }

				                                       ImGui::EndCombo();
			                                       }

			                                       if(camera.GetProjectionType() == Engine::SceneCamera::ProjectionType::Perspective)
			                                       {
				                                       float perspectiveVerticalFov = glm::degrees(
				                                                                                   camera.GetPerspectiveVerticalFOV()
				                                                                                  );
				                                       if(ImGui::DragFloat("Vertical Fov", &perspectiveVerticalFov))
					                                       camera.SetPerspectiveVerticalFOV(perspectiveVerticalFov);

				                                       float perspectiveNear = camera.GetPerspectiveNearClip();
				                                       if(ImGui::DragFloat("Near", &perspectiveNear))
					                                       camera.SetPerspectiveNearClip(perspectiveNear);

				                                       float perspectiveFar = camera.GetPerspectiveFarClip();
				                                       if(ImGui::DragFloat("Far", &perspectiveFar))
					                                       camera.SetPerspectiveFarClip(perspectiveFar);
			                                       }

			                                       if(camera.GetProjectionType() == Engine::SceneCamera::ProjectionType::Orthographic)
			                                       {
				                                       float orthoSize = camera.GetOrthographicSize();
				                                       if(ImGui::DragFloat("Size", &orthoSize))
					                                       camera.SetOrthographicSize(orthoSize);

				                                       float orthoNear = camera.GetOrthographicNearClip();
				                                       if(ImGui::DragFloat("Near", &orthoNear))
					                                       camera.SetOrthographicNearClip(orthoNear);

				                                       float orthoFar = camera.GetOrthographicFarClip();
				                                       if(ImGui::DragFloat("Far", &orthoFar))
					                                       camera.SetOrthographicNearClip(orthoFar);

				                                       Engine::ToggleButton("Fixed Aspect Ratio", &component.FixedAspectRatio);

				                                       float aspectRatio = camera.GetAspectRatio();
				                                       if(ImGui::DragFloat(
				                                                           "AspectRatio",
				                                                           &aspectRatio,
				                                                           1,
				                                                           0,
				                                                           0,
				                                                           "%.3f",
				                                                           ImGuiSliderFlags_ReadOnly
				                                                          ))
					                                       camera.SetAspectRatio(aspectRatio);
			                                       }
		                                       }
		                                      );
	}

	template <typename T>
	void SceneHierarchyPanel::DisplayAddComponentEntry(const std::string &entryName)
	{
		if (!m_SelectionContext.HasComponent<T>())
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				m_SelectionContext.AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}

}
