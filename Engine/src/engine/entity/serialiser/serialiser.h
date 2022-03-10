#pragma once

#include <engine/entity/entity.h>

namespace Techless
{

	class Serialiser
	{
	public: // Serialiser
		Serialiser() = default;
		Serialiser(Entity& RootEntity);

		void Serialise(Entity& RootEntity);
		void SaveToFile(const std::string& FilePath);

	private:
		JSON SerialiseEntity(Entity& a_Entity);

	private:
		JSON p_JSON;

		Entity* RootEntity = nullptr;
		Scene* RootScene = nullptr;

	};

	class Deserialiser
	{
	public:
		Deserialiser(const std::string& FilePath, const std::string& PrefabName);

		Prefab Deserialise();

		void LoadFromFile(const std::string& FilePath);

	private:
		void DeserialiseEntity(JSON& SerialisedEntity, int ParentID = -1);

	private:
		Prefab p_Prefab;
		JSON p_JSON;

		uint16_t EntityIndex = 0;


	};

}