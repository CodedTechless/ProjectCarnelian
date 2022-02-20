#pragma once

#include <engine/entity/entity.h>
#include <engine/entity/components.h>

namespace Techless
{

	class Serialiser
	{
	public: // Serialiser
		Serialiser() = default;
		Serialiser(Entity& RootEntity);

		void Serialise(Entity& RootEntity);

		void SaveToFile(const std::string& FilePath);

		template <typename Component>
		void AssignComponent(const std::string& EntryName)
		{
			auto Set = RootScene->GetInstances<Component>();
			if (Set->GetSize() == 0)
				return;

			p_JSON["Component"][EntryName] = JSON::array();
			JSON& Entry = p_JSON["Component"][EntryName];

			for (const auto& EntityID : RelationID)
			{
				if (Set->Has(EntityID))
					Entry += Set->Get(EntityID);
			}
		}

	private:
		void SerialiseChildren(Entity& a_Entity, size_t a_ParentIndex = 0);

	private:
		JSON p_JSON;

		Entity* RootEntity = nullptr;
		Scene* RootScene = nullptr;

		std::vector<std::string> RelationID{};
	};

	class Deserialiser
	{
	public:
		Deserialiser(const std::string& FilePath);

		Prefab Deserialise();

		void LoadFromFile(const std::string& FilePath);

		template <typename Component>
		void PullComponents(const std::string& EntryName)
		{
			if (p_JSON["Components"].find(EntryName) == p_JSON["Components"].end())
				return;

			auto PrefabComponents = p_Prefab.GetComponents<Component>();
			JSON& Components = p_JSON["Components"][EntryName];

			for (JSON::iterator it = Components.begin(); it != Components.end(); ++it)
			{
				PrefabComponents->Add(it.key(), it.value().get<Component>());
			}
	}

	private:
		Prefab p_Prefab;
		JSON p_JSON;


	};

}