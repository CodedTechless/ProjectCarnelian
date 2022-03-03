#include "serialiser.h"

#include <engine/entity/components.h>

namespace Techless
{

	struct EntitySerialiser
	{
		EntitySerialiser(Entity& a_Entity, JSON& a_Components)
			: p_Entity(&a_Entity), p_Components(&a_Components) {};

		template <typename Component>
		void AssignComponent(const std::string& EntryName)
		{
			if (!p_Entity->HasComponent<Component>())
				return;

			p_Components->at(EntryName) = p_Entity->GetComponent<Component>();
		}

	private:
		Entity* p_Entity;
		JSON* p_Components;

	};

	Serialiser::Serialiser(Entity& a_RootEntity)
	{
		Serialise(a_RootEntity);
	}

	void Serialiser::Serialise(Entity& a_RootEntity)
	{
		RootEntity = &a_RootEntity;
		RootScene = RootEntity->GetScene();

		p_JSON = SerialiseEntity(a_RootEntity);
	}

	JSON Serialiser::SerialiseEntity(Entity& a_Entity)
	{
		if (a_Entity.Archivable == false)
			return;

		// create the element array
		JSON EntityElement = {
			{"Components", JSON::object()},
			{"Children", JSON::array()}
		};

		// serialise components
		{
			EntitySerialiser s = { a_Entity, EntityElement };

			s.AssignComponent <TagComponent>("Tag");
			s.AssignComponent <TransformComponent>("Transform");
			s.AssignComponent <RigidBodyComponent>("RigidBody");
			s.AssignComponent <SpriteComponent>("Sprite");
			s.AssignComponent <CameraComponent>("Camera");
			s.AssignComponent <LuaScriptComponent>("LuaScript");
		}

		// serialise all children

		for (Entity* c_Entity : a_Entity.GetChildren())
		{
			EntityElement["Children"] += SerialiseEntity(*c_Entity);
		}
	}
	
	void Serialiser::SaveToFile(const std::string& FilePath)
	{
		std::ofstream o(FilePath);
		o << p_JSON << std::endl;
	}


	// Deserialisation

	struct EntityDeserialiser
	{
		EntityDeserialiser(uint16_t id, Prefab& a_Prefab, JSON& a_Components)
			: ID(id), p_Prefab(&a_Prefab), p_Components(&a_Components) {};

		template <typename Component>
		void AssignComponent(const std::string& EntryName)
		{
			if (p_Components->find(EntryName) == p_Components->end())
				return;

			auto PrefabComponents = p_Prefab->GetComponents<Component>();
			PrefabComponents->Add(ID, p_Components->at(EntryName).get<Component>());
		}

	private:
		uint16_t ID;

		Prefab* p_Prefab;
		JSON* p_Components;

	};

	Deserialiser::Deserialiser(const std::string& FilePath)
	{
		LoadFromFile(FilePath);
	}

	void Deserialiser::LoadFromFile(const std::string& FilePath)
	{
		std::ifstream i(FilePath);
		i >> p_JSON;
	}

	Prefab Deserialiser::Deserialise()
	{
		DeserialiseEntity(p_JSON);
		p_Prefab.Entities = EntityIndex;

		return p_Prefab;
	}

	void Deserialiser::DeserialiseEntity(JSON& SerialisedEntity, int ParentID)
	{
		JSON& Components = SerialisedEntity["Components"];
		JSON& Children = SerialisedEntity["Children"];

		p_Prefab.ParentalIndex.push_back(ParentID);
		int ThisID = EntityIndex++;

		{
			EntityDeserialiser s = { ThisID, p_Prefab, Components };

			s.AssignComponent <TagComponent>("Tag");
			s.AssignComponent <TransformComponent>("Transform");
			s.AssignComponent <RigidBodyComponent>("RigidBody");
			s.AssignComponent <SpriteComponent>("Sprite");
			s.AssignComponent <CameraComponent>("Camera");
			s.AssignComponent <LuaScriptComponent>("LuaScript");
		}

		for (JSON& element : Children)
		{
			DeserialiseEntity(element, ThisID);
		}
	}
}