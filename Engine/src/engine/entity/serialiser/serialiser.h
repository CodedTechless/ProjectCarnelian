#pragma once

#include <engine/entity/entity.h>

namespace Techless
{

	class Serialiser
	{
	public: // Serialiser
		Serialiser() = default;
		Serialiser(Ptr<Entity> RootEntity);

		void Serialise(Ptr<Entity> RootEntity);
		void SaveToFile(const std::string& FilePath);

	private:
		JSON SerialiseEntity(Ptr<Entity> a_Entity);

	private:
		JSON p_JSON;

		Ptr<Entity> RootEntity = nullptr;
		Ptr<Scene> RootScene = nullptr;

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