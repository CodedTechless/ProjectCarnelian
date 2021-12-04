#pragma once

#include <iostream>

#include <Engine.h>

#include "../layers/test_layer.h"

using namespace Techless;

namespace Sandbox
{

	class DesecratedDungeons : public Application
	{
	public:
		DesecratedDungeons() 
		{
			SetApplicationTitle("DesecratedDungeons");

			Init();

			auto Main = new MainLayer();
			Layers.PushLayer(Main);
		}
	};

}
