#pragma once

#include <engine/watchdog/watchdog.h>

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