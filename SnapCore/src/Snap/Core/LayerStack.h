#pragma once

#include "Layer.h"

namespace SnapEngine
{

	class LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverLayer(Layer* overlayer);
		void PopLayer(Layer* layer);
		void PopOverLayer(Layer* overlayer);

		// To be used in range based for loop
		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }

	private:
		std::vector<Layer*> m_Layers;
		unsigned int m_LayersIteratorIndex = 0;
	};
}