#pragma once


#include <Engine.h>

#include <app/layers/editor.h>

using namespace Techless;

namespace PrefabEditor
{

	class PrefabEditor : public Application
	{
	public:
		PrefabEditor()
		{
			SetApplicationTitle("Prefab Editor");

			Init();

			auto Main = new Editor();
			Layers.PushLayer(Main);

			Run();
		}
	};

}
