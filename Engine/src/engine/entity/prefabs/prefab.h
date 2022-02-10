#pragma once

#include <engine/entity/registry/registry.h>
#include <engineincl.h>

#include <json/json.hpp>

using JSON = nlohmann::json;

namespace Techless
{

	namespace PrefabUtil
	{

		/*
			this is a very simple child of RegistrySet specifically used to store Prefab contents.
			Uses an std::vector instead of a std::array because it doesn't need to worry about
			resizing its contents.
		*/

		template <typename Component>
		class TypedPrefabRegistry : public RegistrySet
		{
		public:
			void Add(const std::string& EntryName, Component component)
			{
				Components[EntryName] = component;
			}

			Component& Get(const std::string& EntryName)
			{
				if (Components.find(EntryName) == Components.end())
					return NULL;

				return Components[EntryName];
			}

			void Clear(std::string ID)
			{
				if (Components.find(ID) != Components.end())
				{
					Components.erase(ID);
				}
			}

			std::unordered_map<std::string, Component>::iterator begin() { return Components.begin(); };
			std::unordered_map<std::string, Component>::iterator end() { return Components.end(); };

		private:
			std::unordered_map<std::string, Component> Components{};

		};
	
	}

	struct PrefabEntity
	{
		std::string EntityID = "";
		std::string ParentEntityID = "";

		bool IsRoot = false;

	public:

		inline friend void from_json(const JSON& json, PrefabEntity& entity)
		{
			json.at("EntityID").get_to(entity.EntityID);
			if (json.at("ParentEntityID").type_name() != "null")
			{
				json.at("ParentEntityID").get_to(entity.ParentEntityID);
			}
		}
	};

	class Prefab
	{
	public:
		Prefab() = default;
		Prefab(JSON& json, const std::string& filePath);

		void Deserialise(JSON& json);
	
		template <typename Type>
		Ptr<PrefabUtil::TypedPrefabRegistry<Type>> GetComponents()
		{
			const char* TypeName = typeid(Type).name();
			if (Components.find(TypeName) == Components.end())
				Components[TypeName] = CreatePtr<PrefabUtil::TypedPrefabRegistry<Type>>();

			return std::static_pointer_cast<PrefabUtil::TypedPrefabRegistry<Type>>(Components[TypeName]);
		}

	private:

		/*
			Pulls all serialised components of type Component from a JSON array (j_Components) containing said
			serialised components in JSON
		*/
		template <typename Component>
		void PullSerialisedComponents(JSON& j_Components, const std::string& EntryName)
		{
			if (j_Components.find(EntryName) == j_Components.end())
				return;

			auto PrefabComponents = GetComponents<Component>();
			JSON& j_TypeComponent = j_Components[EntryName];

			for (JSON::iterator it = j_TypeComponent.begin(); it != j_TypeComponent.end(); ++it)
			{
				PrefabComponents->Add(it.key(), it.value().get<Component>());
			}
		}

	private:
		std::string FilePath = "";
		
		std::vector<PrefabEntity> Entities{};
		std::unordered_map<const char*, Ptr<RegistrySet>> Components{};

		friend class Scene;
	};

}