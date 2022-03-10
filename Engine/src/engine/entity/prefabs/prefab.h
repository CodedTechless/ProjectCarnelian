#pragma once

#include <engineincl.h>

namespace Techless
{

	namespace PrefabUtil
	{

		class RegistrySet
		{
		public:
			virtual ~RegistrySet() = default;
		};

		/*
			this is a very simple child of RegistrySet specifically used to store Prefab contents.
			Uses an std::vector instead of a std::array because it doesn't need to worry about
			resizing its contents.
		*/

		template <typename Component>
		class TypedPrefabRegistry : public PrefabUtil::RegistrySet
		{
		public:
			void Add(uint16_t RegistryID, Component component)
			{
				Components[RegistryID] = component;
			}

			Component& Get(uint16_t RegistryID)
			{
				if (Components.find(RegistryID) == Components.end())
					return NULL;

				return Components[RegistryID];
			}

			std::unordered_map<uint16_t, Component>::iterator begin() { return Components.begin(); };
			std::unordered_map<uint16_t, Component>::iterator end() { return Components.end(); };

		private:
			std::unordered_map<uint16_t, Component> Components{};

		};
	
	}

	class Prefab
	{
	public:
		Prefab() = default;
//		Prefab(JSON& json, const std::string& name);
		Prefab(const std::string& name);

		inline std::string GetName() const { return Name; };

		template <typename Type>
		Ptr<PrefabUtil::TypedPrefabRegistry<Type>> GetComponents()
		{
			const char* TypeName = typeid(Type).name();
			if (Components.find(TypeName) == Components.end())
				Components[TypeName] = CreatePtr<PrefabUtil::TypedPrefabRegistry<Type>>();

			return std::static_pointer_cast<PrefabUtil::TypedPrefabRegistry<Type>>(Components[TypeName]);
		}

		int Entities = 0;

		std::vector<int> ParentalIndex{}; // where "int" is parent index
		std::unordered_map<const char*, Ptr<PrefabUtil::RegistrySet>> Components{};

	private:
		std::string Name = "";
		bool Loaded = false;

		friend class Scene;
	};

}


/*

saving this in case i need it at some other point :)

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
*/