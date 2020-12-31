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

	ImGuiUniqueGuard::ImGuiUniqueGuard(
		const ImVec2 &position,
		const std::string_view &name,
		bool &open,
		ImGuiWindowFlags flags
		)
	{
		ImGui::SetNextWindowPos(position);
		m_Opened = ImGui::Begin(name.data(), &open, flags);
	}

	ImGuiUniqueGuard::ImGuiUniqueGuard(const std::string_view &name, bool &open, ImGuiWindowFlags flags)
	{
		m_Opened = ImGui::Begin(name.data(), &open, flags);
	}

	ImGuiUniqueGuard::~ImGuiUniqueGuard()
	{
		ImGui::End();
	}

	ImGuiGuard::ImGuiGuard(const ImVec2 &position, const std::string_view &name, bool &open, ImGuiWindowFlags flags)
	{
		m_ReferenceCount = new size_t(1);

		ImGui::SetNextWindowPos(position);
		m_Opened = ImGui::Begin(name.data(), &open, flags);
	}

	ImGuiGuard::ImGuiGuard(const std::string_view &name, bool &open, ImGuiWindowFlags flags)
	{
		m_ReferenceCount = new size_t(1);
		m_Opened         = ImGui::Begin(name.data(), &open, flags);
	}

	ImGuiGuard::~ImGuiGuard()
	{
		if(*m_ReferenceCount == 1)
		{
			delete m_ReferenceCount;
			ImGui::End();
		}
		else
		{
			--(*m_ReferenceCount);
		}
	}

	ImGuiGuard::ImGuiGuard(const ImGuiGuard &guard)
	{
		*this = guard;
	}

	ImGuiGuard::ImGuiGuard(ImGuiGuard &&guard)
	{
		*this = std::move(guard);
	}

	ImGuiGuard& ImGuiGuard::operator=(const ImGuiGuard &guard)
	{
		m_ReferenceCount = guard.m_ReferenceCount;
		++(*m_ReferenceCount);
		m_Opened = guard.m_Opened;

		return *this;
	}

	ImGuiGuard& ImGuiGuard::operator=(ImGuiGuard &&guard)
	{
		m_ReferenceCount       = guard.m_ReferenceCount;
		guard.m_ReferenceCount = nullptr;
		m_Opened               = guard.m_Opened;

		return *this;
	}

	ImGuiChildUniqueGuard::ImGuiChildUniqueGuard(ImGuiID id, const ImVec2 &size, bool border, ImGuiWindowFlags flags)
	{
		m_Opened = ImGui::BeginChild(id, size, border, flags);
	}

	ImGuiChildUniqueGuard::ImGuiChildUniqueGuard(
		const std::string_view &id,
		const ImVec2 &size,
		bool border,
		ImGuiWindowFlags flags
		)
	{
		m_Opened = ImGui::BeginChild(id.data(), size, border, flags);
	}

	ImGuiChildUniqueGuard::~ImGuiChildUniqueGuard()
	{
		ImGui::EndChild();
	}

	ImGuiChildGuard::ImGuiChildGuard(ImGuiID id, const ImVec2 &size, bool border, ImGuiWindowFlags flags)
	{
		m_ReferenceCount = new size_t(1);

		m_Opened = ImGui::BeginChild(id, size, border, flags);
	}

	ImGuiChildGuard::ImGuiChildGuard(
		const std::string_view &id,
		const ImVec2 &size,
		bool border,
		ImGuiWindowFlags flags
		)
	{
		m_ReferenceCount = new size_t(1);

		m_Opened = ImGui::BeginChild(id.data(), size, border, flags);
	}

	ImGuiChildGuard::~ImGuiChildGuard()
	{
		if(*m_ReferenceCount == 1)
		{
			delete m_ReferenceCount;
			ImGui::EndChild();
		}
		else --(*m_ReferenceCount);
	}

	ImGuiChildGuard::ImGuiChildGuard(const ImGuiChildGuard &guard)
	{
		*this = guard;
	}

	ImGuiChildGuard::ImGuiChildGuard(ImGuiChildGuard &&guard)
	{
		*this = std::move(guard);
	}

	ImGuiChildGuard& ImGuiChildGuard::operator=(const ImGuiChildGuard &guard)
	{
		m_ReferenceCount = guard.m_ReferenceCount;
		++(*m_ReferenceCount);
		m_Opened = guard.m_Opened;

		return *this;
	}

	ImGuiChildGuard& ImGuiChildGuard::operator=(ImGuiChildGuard &&guard)
	{
		m_ReferenceCount       = guard.m_ReferenceCount;
		guard.m_ReferenceCount = nullptr;
		m_Opened               = guard.m_Opened;

		return *this;
	}

	bool InputText(
		const std::string_view &label,
		std::string &string,
		ImGuiInputTextFlags flags,
		CallbackFunction callback,
		void *userData
		)
	{
		ASSERT((ImGuiInputTextFlags_CallbackResize & flags) == 0, "");
		flags |= ImGuiInputTextFlags_CallbackResize;

		InputTextCallbackData data{&string, callback, userData};
		return ImGui::InputText(label.data(), string.data(), string.capacity(), flags, InputTextCallback, &data);
	}

	bool Combo(
		const std::string_view &label,
		int32_t &currentItem,
		const std::string_view &itemList,
		int32_t maxHeightInItems
		)
	{
		return ImGui::Combo(label.data(), &currentItem, itemList.data(), maxHeightInItems);
	}
}
