#include "serialiser.h"



namespace Techless
{

	Serialiser::Serialiser(Entity& a_RootEntity)
	{
		Serialise(a_RootEntity);
	}

	void Serialiser::Serialise(Entity& a_RootEntity)
	{
		RootEntity = &a_RootEntity;
		RootScene = RootEntity->GetScene();

		p_JSON = {
		   {"Entity", JSON::array()},
		   {"Component", JSON::object()}
		};

		SerialiseChildren(a_RootEntity);
	}

	void Serialiser::SerialiseChildren(Entity& a_Entity, size_t a_ParentIndex)
	{
		if (a_Entity.Archivable == false)
		{
			return;
		}

		size_t MyIndex = p_JSON["Entity"].size();

		RelationID[MyIndex] = a_Entity.GetID(); // store the index of this entity
		p_JSON["Entity"] += {
			{"Parent", a_ParentIndex}
		}; // add the entity to the JSON object

		for (Entity* Child : a_Entity.GetChildren())
		{
			SerialiseChildren(*Child, MyIndex); // call this function again for all this entity's children
		}
	}
	
	void Serialiser::SaveToFile(const std::string& FilePath)
	{
		std::ofstream o(FilePath);
		o << p_JSON << std::endl;
	}


	// Deserialisation

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

	}

}