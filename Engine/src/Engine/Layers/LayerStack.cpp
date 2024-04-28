#include "pch.h"
#include "Engine/Layers/LayerStack.h"

namespace Engine {
	LayerStack::~LayerStack() {
		for (auto &layer : m_Layers) {
			layer->OnDetach();
		}
	}

	void LayerStack::PushLayer(LayerPtr layer) {
		m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
		m_LayerInsertIndex++;
	}

	void LayerStack::PushOverlay(LayerPtr overlay) {
		m_Layers.emplace_back(overlay);
	}

	void LayerStack::PopLayer(LayerPtr layer) {
		if (const auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer); it != m_Layers.begin() + m_LayerInsertIndex) {
			layer->OnDetach();
			m_Layers.erase(it);
			m_LayerInsertIndex--;
		}
	}

	void LayerStack::PopOverlay(LayerPtr overlay) {
		if (const auto it = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), overlay); it != m_Layers.end()) {
			overlay->OnDetach();
			m_Layers.erase(it);
		}
	}
}