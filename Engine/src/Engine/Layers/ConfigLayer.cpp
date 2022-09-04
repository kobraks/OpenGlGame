#include "pch.h"
#include "Engine/Layers/ConfigLayer.h"

#include "Engine/Core/Application.h"
#include "Engine/Events/KeyEvent.h"
#include "Engine/Devices/Monitor.h"

#include <imgui.h>
#include "Engine/ImGui/ImGuiGuard.h"
#include "Engine/ImGui/ImGuiUtils.h"

namespace Game
{
	ConfigLayer::ConfigLayer() : Layer("ConfigLayer") {}
	void ConfigLayer::OnEvent(Event &event)
	{
		if (event.GetEventType() == KeyReleasedEvent::GetStaticType())
		{
			auto e = dynamic_cast<KeyReleasedEvent*>(&event);
			if (e)
				if (e->GetKeyCode() == Key::F3)
					m_Show = !m_Show;
		}
		
	}
	void ConfigLayer::OnAttach() {}

	void ConfigLayer::OnImGuiRender()
	{
		ImGuiUniqueGuard<ImGuiMainWindow> guard(ImGuiMainWindowProps{"Config", m_Show});

		const auto &app = Application::Get();

		{
			ImGuiChildWindowProps props{"Monitors"};
			ImGuiUniqueGuard<ImGuiChildWindow> childGuard(props);

			const auto monitors    = Monitor::GetAll();
			const auto currentMode = app.GetWindow().GetVideoMode();

			for(const auto &monitor : monitors)
			{
				if(TreeNodeEx("Monitor: {}", 0, monitor.GetName()))
				{
					Text("Position: {}, {}", monitor.GetPosition().X, monitor.GetPosition().Y);
					Text("Scale: {}, {}", monitor.GetScale().X, monitor.GetScale().Y);
					Text(
					     "WorkSpace: X: {}, Y: {}, Width: {}, Height: {}",
					     monitor.GetWorkArea().X,
					     monitor.GetWorkArea().Y,
					     monitor.GetWorkArea().Width,
					     monitor.GetWorkArea().Height
					    );
					const auto &mode = monitor.GetVideoMode();
					Text(
					     "Current Video mode: {}x{} {}Hz R:{} G:{}, B:{}",
					     mode.Size.X,
					     mode.Size.Y,
					     mode.RefreshRate,
					     mode.RedBits,
					     mode.GreenBits,
					     mode.BlueBits
					    );
					if(TreeNodeEx("Video mods", ImGuiTreeNodeFlags_DefaultOpen))
					{
						for(size_t i = 0; i < monitor.GetVideoModes().size(); ++i)
						{
							const auto &mode         = monitor.GetVideoModes()[i];
							ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_OpenOnDoubleClick;

							if(mode == currentMode)
								flags |= ImGuiTreeNodeFlags_Selected;

							const bool treeOpened = TreeNodeEx(
							              "{}: {}x{} {}Hz R:{} G:{}, B:{}.",
							              flags,
							              i,
							              mode.Size.X,
							              mode.Size.Y,
							              mode.RefreshRate,
							              mode.RedBits,
							              mode.GreenBits,
							              mode.BlueBits
							             );
							if(ImGui::IsItemClicked())
								app.GetWindow().ToggleFullscreen(monitor, mode);

							if(treeOpened)
							{
								ImGui::TreePop();
							}
						}

						ImGui::TreePop();
					}

					ImGui::TreePop();
				}
			}
		}
	}
}
