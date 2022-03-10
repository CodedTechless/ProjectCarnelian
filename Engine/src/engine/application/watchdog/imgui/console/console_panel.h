#pragma once

#include <engine/application/watchdog/watchdog.h>

namespace Techless
{

	class ConsolePanel
	{
	public:
		ConsolePanel() = default;

		void RenderImGuiElements();
	
	private:
		bool AutoScroll = true;

	};

}