#include "pch.h"
#include "ImGuiUtils.h"

#include "Assert.h"

namespace Game
{
	struct InputTextCallbackData
	{
		std::string *String;
		CallbackFunction Callback;
		void *UserData;
	};

	static int InputTextCallback(ImGuiInputTextCallbackData *data)
	{
		auto userData = static_cast<InputTextCallbackData*>(data->UserData);
		if(data->EventFlag == ImGuiInputTextFlags_CallbackResize)
		{
			auto string = userData->String;
			ASSERT((data->Buf == string->c_str()), "");
			string->resize(data->BufTextLen);
			data->Buf = const_cast<char*>(string->c_str());
		}
		else if(userData->Callback)
		{
			data->UserData = userData->UserData;
			return userData->Callback(data);
		}

		return 0;
	}

	ImGuiMainWindow::ImGuiMainWindow(const ImGuiMainWindowProps &props) : m_Props(props) {}

	ImGuiMainWindow::ImGuiMainWindow(const ImGuiMainWindowProps &props, const ImGuiWindowPosition &position) : ImGuiMainWindow(props)
	{
		m_PosSet = true;
		m_Pos = position;
	}

	ImGuiMainWindow::ImGuiMainWindow(const ImGuiMainWindowProps &props, const ImGuiWindowPosition &position, const ImGuiWindowSize &size) : ImGuiMainWindow(props, position)
	{
		m_SizeSet = true;
		m_Size = size;
	}

	ImGuiMainWindow::ImGuiMainWindow(const ImGuiMainWindowProps &props, const ImGuiWindowSize &size) : ImGuiMainWindow(props)
	{
		m_SizeSet = true;
		m_Size = size;
	}

	void ImGuiMainWindow::Begin()
	{
		if (m_PosSet)
			SetPosition(m_Pos);
		if (m_SizeSet)
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

	bool InputText(const std::string_view &label, std::string &string, ImGuiInputTextFlags flags, CallbackFunction callback, void *userData)
	{
		ASSERT((ImGuiInputTextFlags_CallbackResize & flags) == 0, "");
		flags |= ImGuiInputTextFlags_CallbackResize;

		InputTextCallbackData data{&string, callback, userData};
		return ImGui::InputText(label.data(), string.data(), string.capacity(), flags, InputTextCallback, &data);
	}

	bool Combo(const std::string_view &label, int32_t &currentItem, const std::string_view &itemList, int32_t maxHeightInItems)
	{
		return ImGui::Combo(label.data(), &currentItem, itemList.data(), maxHeightInItems);
	}
}
