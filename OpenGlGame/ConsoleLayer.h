#pragma once
#include <string>
#include <utility>
#include <vector>

#include "Color.h"
#include "Layer.h"
#include "LuaRegister.h"

namespace Game
{
	class Application;
	
	class ConsoleLayer : public Layer, public LuaRegister
	{
		bool m_Show = true;
		bool m_ReclaimFocus = false;
		bool m_ScrollToBottom = true;
		
		sol::state* m_Lua = nullptr;

		int32_t m_HistoryPos = -1;
		
		std::string m_Command;

		std::vector<std::pair<std::string, Color>> m_Messages;
		std::vector<std::string> m_History;

		Application* m_App;
	public:
		ConsoleLayer();

		void OnAttach() override;
		void OnImGuiRender() override;
		void OnEvent(Event &event) override;
		void OnDetach() override;

		void Clear();
		void ClearHistory();

		template<typename ...Args>
		void PrintMessage(const Color& color, const std::string_view& format, Args&&... args)
		{
			std::invoke(PrintMessage, fmt::format(format, std::forward<Args>(args)...), color);
		}
		
		void PrintMessage(const std::string &message, const Color& color = Color(1.f, 1.f, 1.f, 1.f));

		const auto& GetMessages() const { return m_Messages; }

	protected:
		void Register(sol::state &state) override;

		int TextCallback(void *data);
	};
}
