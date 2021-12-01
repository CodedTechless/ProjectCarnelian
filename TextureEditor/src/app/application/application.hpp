#pragma once

#include <iostream>

#include <Engine.h>

#include "../layers/main_layer.h"

using namespace Techless;

namespace TextureEditor
{

	class TextureEditor : public Application
	{
	public:
		TextureEditor()
		{
			Init();

			auto Main = new MainLayer();
			Layers.PushLayer(Main);
		}
	};

}
