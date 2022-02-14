
#include "layer.h"

#include <engine/watchdog/watchdog.h>

namespace Techless
{

	Layer::Layer(const std::string& layerName)
		: LayerName(layerName)
	{}

	/*
	void Layer::BuildEnvironment()
	{
		if (ScriptEnvironment::Has(LayerName))
		{
			LayerScript = ScriptEnvironment::CreateGlobal(LayerName);

			GetFunction("OnCreated")();
		}
		else
		{
			Debug::Log("Couldn't find a Lua script associated with layer with name " + LayerName, LayerName);
		}
	}

	void Layer::OnCreated()
	{
		BuildEnvironment();
	}

	void Layer::OnRemoved()
	{
		if (LayerScript)
			GetFunction("OnRemoved")();

	}

	void Layer::OnUpdate(const float Delta)
	{
		if (LayerScript)
			GetFunction("OnUpdate")(Delta);

	}

	void Layer::OnUpdateFixed(const float Delta)
	{
		if (LayerScript)
			GetFunction("OnUpdateFixed")(Delta);

	}

	void Layer::OnUpdateEnd(const float Delta)
	{
		if (LayerScript)
			GetFunction("OnUpdateEnd")(Delta);

	}

	void Layer::OnUpdateFixedEnd(const float Delta)
	{
		if (LayerScript)
			GetFunction("OnUpdateFixedEnd")(Delta);

	}

	Input::Filter Layer::OnInputEvent(const InputEvent& inputEvent, bool Processed)
	{
		if (LayerScript)
			return GetFunction("OnInputEvent")(inputEvent, Processed);

		return Input::Filter::Ignore;
	}

	void Layer::OnWindowEvent(const WindowEvent& windowEvent)
	{
		if (LayerScript)
			GetFunction("OnWindowEvent")(windowEvent);
	}

	*/
}