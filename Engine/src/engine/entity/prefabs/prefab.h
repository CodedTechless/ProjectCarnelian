#pragma once

#include <engine/entity/scene.h>

#include <engineincl.h>

namespace Techless
{

	namespace PrefabUtil
	{

		template <typename Component>
		class TypedPrefabRegistry : public RegistrySet
		{

		};
	
	}

	class Prefab
	{
	public:
		Prefab() = default;
		Prefab(const JSON& json);

		Deserialise(const JSON& json);
	
	private:

		template <typename Component>
		void PullSerialisedComponents(JSON& j_Components, const std::string& EntryName)
		{
			const char* Type = typeid(Component).name();
			if (Components.find(Type) == Components.end())
				Components[Type] = {};


		}

	private:
		std::string FilePath = "";
		
		std::unordered_map<std::string, Entity> Entities{};
		std::unordered_map<const char*, std::vector<RegistrySet>> Components{};

		friend class Scene;
	};

}