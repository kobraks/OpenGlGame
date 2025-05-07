#pragma once

#include "Engine/ImGui/StatisticGraph.h"

#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>

namespace Engine {
	class StatisticGraphGroup {
	public:
		void AddGraph(const std::string& id, Ref<StatisticGraph> graph);
		void DrawAll();

		Ref<StatisticGraph> Get(const std::string& id) const;
		bool Remove(const std::string& id);
		bool Has(const std::string& id) const;
	private:
		std::vector <std::pair<std::string, Ref<StatisticGraph>>> m_Graphs;
		std::unordered_map<std::string, Ref<StatisticGraph>> m_GraphLookup;
	};
}