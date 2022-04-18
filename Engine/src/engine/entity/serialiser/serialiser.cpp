#include "serialiser.h"

#include <engine/entity/components.h>

namespace Techless
{

	struct EntitySerialiser
	{
		EntitySerialiser(Ptr<Entity> a_Entity, JSON& a_Components)
			: p_Entity(a_Entity), p_Components(&a_Components) {};

		template <typename Component>
		void AssignComponent(const std::string& EntryName)
		{
			if (!p_Entity->HasComponent<Component>())
				return;

			p_Components->emplace(EntryName, p_Entity->GetComponent<Component>());
		}

	private:
		Ptr<Entity> p_Entity;
		JSON* p_Components;

	};

	Serialiser::Serialiser(Ptr<Entity> a_RootEntity)
	{
		Serialise(a_RootEntity);
	}

	void Serialiser::Serialise(Ptr<Entity> a_RootEntity)
	{
		RootEntity = a_RootEntity;
		RootScene = RootEntity->GetScene();

		p_JSON = SerialiseEntity(a_RootEntity);
	}

	JSON Serialiser::SerialiseEntity(Ptr<Entity> a_Entity)
	{
		// create the element array
		JSON EntityElement = {
			{"Components", JSON::object()},
			{"Children", JSON::array()}
		};

		// serialise components
		{
			EntitySerialiser s = { a_Entity, EntityElement["Components"] };

			// [COMPONENT ASSIGNMENT]

			s.AssignComponent <TagComponent>			("Tag");
			s.AssignComponent <TransformComponent>		("Transform");
			s.AssignComponent <BoxColliderComponent>	("BoxCollider");
			s.AssignComponent <YSortComponent>			("YSort");
//			s.AssignComponent <RigidBodyComponent>		("RigidBody");

			s.AssignComponent <SpriteComponent>			("Sprite");
			s.AssignComponent <SpriteAnimatorComponent>	("SpriteAnimator");
			s.AssignComponent <CameraComponent>			("Camera");

			s.AssignComponent <LuaScriptComponent>		("LuaScript");
		}

		// serialise all children

		for (Ptr<Entity> c_Entity : a_Entity->GetChildren())
		{
			if (c_Entity->Archivable)
				EntityElement["Children"] += SerialiseEntity(c_Entity);
		}

		return EntityElement;
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

	Deserialiser::Deserialiser(const std::string& FilePath, const std::string& PrefabName)
	{
		p_Prefab = { PrefabName };
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
		uint16_t ThisID = EntityIndex++;

		{
			
			// [COMPONENT ASSIGNMENT]

			EntityDeserialiser s = { ThisID, p_Prefab, Components };

			s.AssignComponent <TagComponent>("Tag");
			s.AssignComponent <TransformComponent>("Transform");
			s.AssignComponent <BoxColliderComponent>("BoxCollider");
			s.AssignComponent <YSortComponent>("YSort");
//			s.AssignComponent <RigidBodyComponent>		("RigidBody");

			s.AssignComponent <SpriteComponent>("Sprite");
			s.AssignComponent <SpriteAnimatorComponent>("SpriteAnimator");
			s.AssignComponent <CameraComponent>("Camera");

			s.AssignComponent <LuaScriptComponent>("LuaScript");
		}

		for (JSON& element : Children)
		{
			DeserialiseEntity(element, ThisID);
		}
	}
}