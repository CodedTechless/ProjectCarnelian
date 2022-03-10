#include "resource_loader.h"



namespace Techless
{

	void ResourceLoader::ReadFiles(std::vector<fs::directory_entry>& Entries, const std::string& Extension, const std::string& Path)
	{
		for (const fs::directory_entry& File : fs::directory_iterator(Path))
		{
			fs::path FsPath = File.path();
			std::string CurrentPath = FsPath.generic_string();

			if (File.is_directory())
			{
				ReadFiles(Entries, Extension, CurrentPath);
			}
			else if (File.is_regular_file() && FsPath.extension().generic_string() == Extension)
			{
				Entries.push_back(File);
			}
		}
	}

	std::string ResourceLoader::GetExtension(const Resource& ResourceType)
	{
		switch (ResourceType)
		{
			case Resource::Texture: return ".png";
			case Resource::LuaScript: return ".lua";
			case Resource::SpriteAnimation: return ".spranim";
			case Resource::Prefab: return ".prefab";
		}

		return "";
	}
}