#pragma once

#include <engineincl.h>

namespace fs = std::filesystem;

namespace PrefabEditor
{

	struct PrefabFileItem
	{
		std::string FilePath;
		std::string FileName;
	};

	class AssetManagerPanel
	{
	public:
		AssetManagerPanel();

		void RenderImGuiElements();

	private:
		fs::path CurrentDirectory;

	};

}