#include "pch.h"
#include "StatisticGraphGroup.h"

#include <imgui.h>

namespace Engine {
	void StatisticGraphGroup::AddGraph(const std::string& id, Ref<StatisticGraph> graph) {
		m_Graphs[id] = std::move(graph);
	}

	void StatisticGraphGroup::DrawAll(){
		for (auto& [id, graph] : m_Graphs) {
			graph->Draw();
			ImGui::Separator();
		}
	}

	Ref<StatisticGraph> StatisticGraphGroup::Get(const std::string& id) {
		auto it = m_Graphs.find(id);
		return (it != m_Graphs.end()) ? it->second : nullptr;
	}
}
