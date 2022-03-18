#pragma once

#include <engineincl.h>

namespace Techless
{

	using DirectoryEntries = std::vector<fs::directory_entry>;

	enum class Resource
	{
		None,
		Texture,			// png
		Prefab,				// prefab
		LuaScript,			// lua
		SpriteAnimation		// spranim

	};

	class ResourceLoader
	{
	public:

//		static std::vector<Resource> Load(Resource ResourceType);
		template <typename Func>
		static void GetFiles(const Resource& ResourceType, Func&& ReadFunc, const std::string& Directory = "assets")
		{
			std::vector<fs::directory_entry> Entries = {};
			ReadFiles(Entries, GetExtension(ResourceType), Directory);

			for (auto& File : Entries)
				ReadFunc(File);
		}

	private:
		static std::string GetExtension(const Resource& ResourceType);

		static void ReadFiles(DirectoryEntries& Entries, const std::string& Extension, const std::string& Path = "assets");

	};

}