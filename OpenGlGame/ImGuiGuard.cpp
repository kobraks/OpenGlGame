#include "pch.h"
#include "ImGuiGuard.h"

namespace Game
{
	ImGuiMainWindow::ImGuiMainWindow(const ImGuiMainWindowProps &props) : m_Props(props) {}

	ImGuiMainWindow::ImGuiMainWindow(
		const ImGuiMainWindowProps &props,
		const ImGuiWindowPosition &position
		) : ImGuiMainWindow(props)
	{
		m_PosSet = true;
		m_Pos    = position;
	}

	ImGuiMainWindow::ImGuiMainWindow(
		const ImGuiMainWindowProps &props,
		const ImGuiWindowPosition &position,
		const ImGuiWindowSize &size
		) : ImGuiMainWindow(props, position)
	{
		m_SizeSet = true;
		m_Size    = size;
	}

	ImGuiMainWindow::ImGuiMainWindow(
		const ImGuiMainWindowProps &props,
		const ImGuiWindowSize &size
		) : ImGuiMainWindow(props)
	{
		m_SizeSet = true;
		m_Size    = size;
	}

	void ImGuiMainWindow::Begin()
	{
		if(m_PosSet)
			SetPosition(m_Pos);
		if(m_SizeSet)
			SetSize(m_Size);
		m_Opened = ImGui::Begin(m_Props.Name.data(), &m_Props.Open, m_Props.Flags);
	}

	void ImGuiMainWindow::End()
	{
		ImGui::End();
	}

	void ImGuiMainWindow::SetPosition(const ImGuiWindowPosition &position)
	{
		ImGui::SetNextWindowPos(position.Position, position.Cond, position.Pivot);
	}

	void ImGuiMainWindow::SetSize(const ImGuiWindowSize &size)
	{
		ImGui::SetNextWindowSize(size.Size, size.Cond);
	}

	ImGuiChildWindow::ImGuiChildWindow(const ImGuiChildWindowProps &props) : m_Props(props) {}

	void ImGuiChildWindow::Begin()
	{
		ImGui::BeginChild(m_Props.Id.data(), m_Props.Size, m_Props.Border, m_Props.Flags);
	}

	void ImGuiChildWindow::End()
	{
		ImGui::EndChild();
	}

	ImGuiTable::ImGuiTable(const ImGuiTableProps &props) : m_Props(props) {}

	void ImGuiTable::Begin()
	{
		ImGui::BeginTable(
		                  m_Props.Id.data(),
		                  m_Props.ColumnsCount,
		                  m_Props.Flags,
		                  m_Props.OuterSize,
		                  m_Props.InnerWidth
		                 );
	}

	void ImGuiTable::End()
	{
		ImGui::EndTable();
	}

	void ImGuiGroup::Begin()
	{
		ImGui::BeginGroup();
	}

	void ImGuiGroup::End()
	{
		ImGui::EndGroup();
	}

	void ImGuiID::Begin()
	{
		if(std::holds_alternative<std::string>(Id))
		{
			ImGui::PushID(std::get<std::string>(Id).c_str());
			return;
		}

		if(std::holds_alternative<int>(Id))
		{
			ImGui::PushID(std::get<int>(Id));
			return;
		}

		if(std::holds_alternative<const void*>(Id))
		{
			ImGui::PushID(std::get<const void*>(Id));
		}
	}

	void ImGuiID::End()
	{
		ImGui::PopID();
	}
}
