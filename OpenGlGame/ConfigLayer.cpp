#include "pch.h"
#include "ConfigLayer.h"

#include "imgui.h"
#include "ImGuiUtils.h"
#include "Application.h"
#include "KeyEvent.h"

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

			const auto monitors    = app.GetWindow().GetMonitors();
			const auto currentMode = app.GetWindow().GetVideoMode();

			for(const auto &monitor : monitors)
			{
				if(TreeNodeEx("Monitor: {}", 0, monitor.Name))
				{
					Text("Position: {}, {}", monitor.Position.X, monitor.Position.Y);
					Text("Scale: {}, {}", monitor.Scale.X, monitor.Scale.Y);
					Text(
					     "WorkSpace: X: {}, Y: {}, Width: {}, Height: {}",
					     monitor.WorkArea.X,
					     monitor.WorkArea.Y,
					     monitor.WorkArea.Width,
					     monitor.WorkArea.Height
					    );
					const auto &mode = monitor.CurrentVideoMode;
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
						for(size_t i = 0; i < monitor.VideoModes.size(); ++i)
						{
							const auto &mode         = monitor.VideoModes[i];
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
