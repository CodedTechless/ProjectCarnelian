#pragma once

#include <engineincl.h>

#include <engine/entity/entity.h>

namespace Techless
{
	struct BaseComponent
	{
		BaseComponent() = default;
		virtual ~BaseComponent() = default;

		inline Ptr<Entity> GetLinkedEntity() const { return LinkedEntity; };

	protected:
		Ptr<Entity> LinkedEntity = nullptr;

		friend class Entity;
	};

	//////////////////////
	// Basic components //
	//////////////////////

	struct TagComponent : public BaseComponent
	{
	public:
		TagComponent() = default;
		TagComponent(const TagComponent& component) = default;

		std::string Name = "Tag";

	public: // json serialisation

		inline friend void to_json(JSON& json, const TagComponent& component)
		{
			json = JSON{
				{"Name", component.Name}
			};
		}

		inline friend void from_json(const JSON& json, TagComponent& component)
		{
			json.at("Name").get_to(component.Name);
		}
	};
}