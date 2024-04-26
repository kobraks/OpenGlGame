#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Layers/Layer.h"

#include <vector>

namespace Engine {
	class LayerStack {
	public:
		using LayerPointer = Pointer<Layer>;
		using LayerList = std::vector<LayerPointer>;

		LayerStack() = default;
		~LayerStack();

		void PushLayer(LayerPointer layer);
		void PushOverlay(LayerPointer overlay);

		void PopLayer(LayerPointer layer);
		void PopOverlay(LayerPointer overlay);

		LayerList::iterator begin() { return m_Layers.begin(); }
		LayerList::iterator end() { return m_Layers.begin(); }

		LayerList::const_iterator begin() const { return m_Layers.begin(); }
		LayerList::const_iterator end() const { return m_Layers.end(); }

		LayerList::reverse_iterator rbegin() { return m_Layers.rbegin(); }
		LayerList::reverse_iterator rend() { return m_Layers.rend(); }

		LayerList::const_reverse_iterator rbegin() const { return m_Layers.rbegin(); }
		LayerList::const_reverse_iterator rend() const { return m_Layers.rend(); }

	private:
		LayerList m_Layers;
		size_t m_LayerInsertIndex = 0;
	};
}