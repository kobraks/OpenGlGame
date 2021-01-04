#pragma once
#include <string>
#include <utility>
#include <vector>

#include "Color.h"
#include "Layer.h"
#include "LuaRegister.h"

namespace Game
{
	class ConsoleLayer : public Layer, public LuaRegister
	{
		bool m_Show = true;
		bool m_Processed = false;
		bool m_ReclaimFocus = false;
		
		sol::state* m_Lua = nullptr;

		int32_t m_HistoryPos = -1;
		
		std::string m_Command;

		std::vector<std::pair<std::string, Color>> m_Messages;
		std::vector<std::string> m_History;

	public:
		ConsoleLayer();

		void OnAttach() override;
		void OnImGuiRender() override;
		void OnUpdate() override;
		void OnDetach() override;

		void Clear();
		void ClearHistory();
		
		void PrintMessage(const std::string &message, const Color& color = Color(1.f, 1.f, 1.f, 1.f));

		const auto& GetMessages() const { return m_Messages; }

	protected:
		void Register(sol::state &state) override;

		int TextCallback(void *data);
	};
}
