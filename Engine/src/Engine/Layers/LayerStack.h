#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Layers/Layer.h"

#include <vector>

namespace Game
{
	class LayerStack
	{
	public:
		using LayerList = std::vector<Layer*>;

	private:
		LayerList m_Layers;
		size_t m_LayerInsertIndex = 0;

	public:
		LayerStack() = default;
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		LayerList::iterator begin() { return m_Layers.begin(); }
		LayerList::iterator end() { return m_Layers.end(); }

		LayerList::reverse_iterator rbegin() { return m_Layers.rbegin(); }
		LayerList::reverse_iterator rend() { return m_Layers.rend(); }

		LayerList::const_iterator begin() const { return m_Layers.begin(); }
		LayerList::const_iterator end() const { return m_Layers.end(); }

		LayerList::const_reverse_iterator rbegin() const { return m_Layers.rbegin(); }
		LayerList::const_reverse_iterator rend() const { return m_Layers.rend(); }
	};
}