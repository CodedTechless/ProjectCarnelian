#pragma once

#include <engineincl.h>

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
		
		void RefreshSprites();
		void RefreshPrefabs(const std::string& Path = "assets/prefabs");
		
	private:
		std::vector<PrefabFileItem> PrefabFiles;
	};

}