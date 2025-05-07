#include "pch.h"
#include "StatisticGraphGroup.h"

#include <imgui.h>

namespace Engine {
	void StatisticGraphGroup::AddGraph(const std::string& id, Ref<StatisticGraph> graph) {
		if (!m_GraphLookup.contains(id))
			m_Graphs.emplace_back(id, graph);
		m_GraphLookup[id] = graph;
	}

	void StatisticGraphGroup::DrawAll() {
		for (const auto& [id, graph] : m_Graphs) {
			graph->Draw();
			ImGui::Separator();
		}
	}

	Ref<StatisticGraph> StatisticGraphGroup::Get(const std::string& id) const {
		auto it = m_GraphLookup.find(id);
		return (it != m_GraphLookup.end()) ? it->second : nullptr;
	}

	bool StatisticGraphGroup::Remove(const std::string& id) {
		auto it = m_GraphLookup.find(id);
		if (it == m_GraphLookup.end()) return false;

		m_GraphLookup.erase(it);
		m_Graphs.erase(std::ranges::remove_if(m_Graphs, [&](const auto& pair) { return pair.first == id; }).begin());
		return true;
	}

	bool StatisticGraphGroup::Has(const std::string& id) const {
		return m_GraphLookup.contains(id);
	}
}
