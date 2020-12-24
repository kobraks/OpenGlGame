#include "pch.h"
#include "LayerStack.h"

namespace Game
{
	LayerStack::~LayerStack()
	{
		for (auto& layer : m_Layers)
			layer->OnDetach();
	}
	
	void LayerStack::PushLayer(Pointer<Layer> layer)
	{
		m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
		m_LayerInsertIndex++;
	}
	
	void LayerStack::PushOverlay(Pointer<Layer> overlay)
	{
		m_Layers.emplace_back(overlay);
	}
	
	void LayerStack::PopLayer(Pointer<Layer> layer)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);
		if (it != m_Layers.begin()  + m_LayerInsertIndex)
		{
			layer->OnDetach();
			m_Layers.erase(it);
			m_LayerInsertIndex--;
		}
	}
	
	void LayerStack::PopOverlay(Pointer<Layer> overlay)
	{
		auto it = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), overlay);
		if (it != m_Layers.end())
		{
			overlay->OnDetach();
			m_Layers.erase(it);
		}
	}
}
