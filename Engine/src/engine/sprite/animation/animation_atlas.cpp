#include "animation_atlas.h"

#include <engine/application/resource_loader.h>
#include <engine/application/watchdog/watchdog.h>

namespace Techless
{
	std::unordered_map<std::string, Ptr<SpriteAnimationSet>> AnimationAtlas::Animations = {};

	void AnimationAtlas::Init()
	{
		Debug::Log("Loading sprite animations...", "AnimationAtlas");

		ResourceLoader::GetFiles(Resource::SpriteAnimation,
			[](const fs::directory_entry& File)
			{
				auto FileStream = std::ifstream(File.path().generic_string());
				JSON json;
				FileStream >> json;

				auto AnimSet = CreatePtr<SpriteAnimationSet>(json.get<SpriteAnimationSet>());

				std::string Name = File.path().stem().generic_string();

				AnimSet->Name = Name;
				Animations[Name] = AnimSet;
			});
	}


	Ptr<SpriteAnimationSet> AnimationAtlas::Get(const std::string& Name)
	{
		if (Animations.find(Name) != Animations.end())
			return Animations[Name];

		return nullptr;
	}

	bool AnimationAtlas::Has(const std::string& Name)
	{
		return (Animations.find(Name) != Animations.end());
	}
}