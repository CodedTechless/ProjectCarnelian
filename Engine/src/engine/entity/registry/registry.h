#pragma once

#include <engineincl.h>
#include <engine/watchdog/watchdog.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace Techless
{

	class RegistrySet
	{
	public:
		virtual ~RegistrySet() = default;
		virtual void Clear(const std::string& ID) = 0;
	};

	template<typename Type>
	class TypedRegistrySet : public RegistrySet
	{
	public:

		template<typename... Args>
		Type& Insert(const std::string& ID, Args&&... args)
		{
			//Debug::Log("Inserted " + entity + " into component type set " + typeid(Component).name() + " at index " + std::to_string(Index));

			Instances[Index] = { std::forward<Args>(args)... };

			IDIndex[ID] = Index;
			IndexID[Index] = ID;

			Index++;

			return Instances[IDIndex[ID]];
		}

		Type& Get(const std::string& ID)
		{
			return Instances[IDIndex[ID]];
		}

		bool Has(const std::string& ID)
		{
			return IDIndex.contains(ID);
		}

		void Remove(const std::string& ID)
		{
			
			auto RemovedIndex = IDIndex[ID];
			auto LastIndex = Index - 1;

			if (RemovedIndex != LastIndex)
			{
				Instances[RemovedIndex] = Instances[LastIndex];

				auto LastEntity = IndexID[LastIndex];
				IDIndex[LastEntity] = RemovedIndex;
				IndexID[RemovedIndex] = LastEntity;

				IDIndex.erase(ID);
				IndexID.erase(LastIndex);
			}

			Index--;
		}

		void Clear(const std::string& ID) override
		{
			if (IDIndex.find(ID) != IDIndex.end())
			{
				Remove(ID);
			}
		}

		inline std::string GetIDAtIndex(size_t ID) { return IndexID[ID]; };
		inline std::array<Type, MaxEntities>& GetInstances() { return Instances; };
		inline size_t GetSize() { return Index; };

		std::array<Type, MaxEntities>::iterator begin() { return Instances.begin(); };
		std::array<Type, MaxEntities>::iterator end() { return Instances.begin() + Index; };
		
	private:
		std::array<Type, MaxEntities> Instances{};

		std::unordered_map<std::string, size_t> IDIndex{};
		std::unordered_map<size_t, std::string> IndexID{};

		size_t Index = 0;
	};


	class Registry
	{
	public:

		template<typename Type>
		void Verify()
		{
			const char* TypeID = typeid(Type).name();
			if (InstanceSets.find(TypeID) == InstanceSets.end())
			{
				InstanceSets[TypeID] = std::make_shared<TypedRegistrySet<Type>>();
			}
		}

		template<typename Type, typename... Args>
		Type& Add(const std::string& ID, Args&&... args)
		{
			Verify<Type>();

			return GetRegistrySet<Type>()->Insert(ID, std::forward<Args>(args)... );
		}

		template <typename Type>
		Type& Get(const std::string& ID)
		{
			return GetRegistrySet<Type>()->Get(ID);
		}

		template <typename Type>
		bool Has(const std::string& ID)
		{
			return GetRegistrySet<Type>()->Has(ID);
		}

		template <typename Type>
		void Remove(const std::string& ID)
		{
			GetRegistrySet<Type>()->Remove(ID);
		}

		template <typename Type>
		Ptr<TypedRegistrySet<Type>> GetRegistrySet()
		{
			Verify<Type>();

			const char* InstanceName = typeid(Type).name();
			return std::static_pointer_cast<TypedRegistrySet<Type>>(InstanceSets[InstanceName]);
		}

		void Clear(const std::string& ID);

	private:
		std::unordered_map<const char*, Ptr<RegistrySet>> InstanceSets{};

		friend class Scene;
	};


}