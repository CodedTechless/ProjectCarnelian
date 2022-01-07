#pragma once


#include <Engine.h>

#include <app/layers/editor.h>

using namespace Techless;

namespace Carnelian
{

	class Carnelian : public Application
	{
	public:
		Carnelian()
		{
			SetApplicationTitle("Prefab Editor");

			Init();

			auto Main = new Editor();
			Layers.PushLayer(Main);

			Run();
		}
	};

}
