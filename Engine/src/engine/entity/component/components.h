#pragma once

#include <engineincl.h>
#include <engine/sprite/sprite_atlas.h>
#include <engine/entity/scriptable_entity.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace Techless
{
	
	struct TransformComponent
	{

		glm::vec2 Position{ 0.f, 0.f };
		glm::vec2 Scale{ 1.f, 1.f };
		float Angle = 0.f;

		float Depth = 0.f;

	public:
		~TransformComponent()
		{
			for (auto* Child : Children)
			{
				Child->Parent = nullptr;
			}

			if (Parent)
				Parent->RemoveChild(this);
		}

		inline void SetParent(TransformComponent* Transform)
		{
			if (Parent != nullptr)
				Parent->RemoveChild(this);

			Parent = Transform;
			
			if (Transform != nullptr)
				Transform->AddChild(this);
		}

		inline TransformComponent* GetParent() const { return Parent; };
		inline std::vector<TransformComponent*> GetChildren() const { return Children; }

	private:
		void AddChild(TransformComponent* Transform)
		{
			Children.push_back(Transform);
		}

		void RemoveChild(TransformComponent* Transform)
		{
			auto it = std::find(Children.begin(), Children.end(), Transform);
			if (it != Children.end())
			{
				Children.erase(it);
			}
		}

		TransformComponent* Parent = nullptr;
		std::vector<TransformComponent*> Children{};
	};

	struct SpriteComponent
	{
		Ptr<Sprite> aSprite;

		glm::vec3 Offset{ 0.f, 0.f, 0.f };
		glm::vec3 Colour{ 1.f, 1.f, 1.f };
		float Alpha = 1.f;

		void SetRGBColour(glm::vec3 Colour)
		{

		}

		glm::vec3 GetRGBColour() { 
			return Colour * 255.f;
		};
	};

	struct TagComponent
	{
		std::string Name = "Tag";
	};

	struct CameraComponent
	{
		void SetProjection(glm::vec2 Size, float Near, float Far)
		{
			Projection = glm::ortho(0.f, Size.x, Size.y, 0.f, Near, Far);
		}

		glm::mat4 GetProjection() { return Projection; };

	private:
		glm::mat4 Projection = glm::ortho(0.f, 1280.f, 720.f, 0.f, -100.f, 100.f);
	};

	struct ScriptComponent
	{
		Ptr<ScriptableEntity> Instance = nullptr;

		template<typename Script>
		Ptr<Script> Bind(Entity& Owner)
		{
			Ptr<Script> newScript = CreatePtr<Script>();

			Instance = std::static_pointer_cast<ScriptableEntity>(newScript);
			Instance->LinkedEntity = &Owner;
			Instance->OnCreate();

			return newScript;
		}

		template<typename Script>
		Ptr<Script> GetScript()
		{
			return std::static_pointer_cast<Script>(Instance);
		}

	private:
		bool Loaded = false;

		friend class Scene;
	};
}