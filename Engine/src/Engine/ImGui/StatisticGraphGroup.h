#pragma once

#include "Engine/ImGui/StatisticGraph.h"

#include <unordered_map>

namespace Engine {
	class StatisticGraphGroup {
	public:
		void AddGraph(const std::string& id, Ref<StatisticGraph> graph);
		void DrawAll();

		Ref<StatisticGraph> Get(const std::string& id);

	private:
		std::unordered_map<std::string, Ref<StatisticGraph>> m_Graphs;
	};
}