#include "pch.h"
#include "Engine/Layers/LayerStack.h"

namespace Engine {
	LayerStack::~LayerStack() {
		Clear();
	}

	void LayerStack::PushLayer(LayerPtr layer) {
		m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
		++m_LayerInsertIndex;
		layer->m_IsOverlay = false;
	}

	void LayerStack::PushOverlay(LayerPtr overlay) {
		m_Layers.emplace_back(overlay);
		overlay->m_IsOverlay = true;
	}

	void LayerStack::PopLayer(LayerPtr layer) {
		if (const auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer); it != m_Layers.begin() + m_LayerInsertIndex) {
			layer->OnDetach();
			m_Layers.erase(it);
			--m_LayerInsertIndex;
		}
	}

	void LayerStack::PopOverlay(LayerPtr overlay) {
		if (const auto it = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), overlay); it != m_Layers.end()) {
			overlay->OnDetach();
			m_Layers.erase(it);
		}
	}

	bool LayerStack::Contains(const LayerPtr& layer) const {
		return std::ranges::find(m_Layers, layer) != m_Layers.end();
	}

	void LayerStack::Clear() {
		for (auto& layer : m_Layers) {
			layer->OnDetach();
		}

		m_Layers.clear();
		m_LayerInsertIndex = 0;
	}
}
