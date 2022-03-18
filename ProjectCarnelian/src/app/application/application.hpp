#pragma once


#include <Engine.h>

#include <app/layers/core.h>

using namespace Techless;

namespace Carnelian
{

	class Carnelian : public Application
	{
	public:
		Carnelian()
		{
			SetApplicationTitle("Project Carnelian");

			Init("carnelian.ini");

			Core* Main = new Core();
			Layers.PushLayer(Main);

			Run();
		}
	};

}
