


#include "layer_set.h"

namespace Techless {

	LayerSet::~LayerSet() {
		for (auto& Layer : Layers) {
			Layer->OnRemoved();
			delete Layer;
		}
	}

	void LayerSet::PushLayer(Layer* Layer) {
		Layers.emplace(Layers.begin() + LayerInsertIndex, Layer);
		LayerInsertIndex++;

		Layer->OnCreated();
	}

	void LayerSet::PopLayer(Layer* Layer) {
		auto Loc = Layers.begin() + LayerInsertIndex;

		auto Iterator = std::find(Layers.begin(), Loc, Layer);
		if (Iterator != Loc) {
			Layer->OnRemoved();
			Layers.erase(Iterator);

			LayerInsertIndex--;
		}
	}

	void LayerSet::PushOverlay(Layer* Overlay) {
		Layers.emplace_back(Overlay);
	}

	void LayerSet::PopOverlay(Layer* Overlay) {
		auto Loc = Layers.begin() + LayerInsertIndex;

		auto Iterator = std::find(Loc, Layers.end(), Overlay);
		if (Iterator != Layers.end()) {
			Overlay->OnRemoved();
			Layers.erase(Iterator);
		}
	}
}