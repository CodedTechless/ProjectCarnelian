#pragma once

#include <engineincl.h>
#include <engine/sprite/sprite_atlas.h>
#include <engine/entity/scriptable_entity.h>
#include <engine/maths/colour.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace Techless
{
	
	//////////////////////
	// Basic components //
	//////////////////////

	struct TagComponent
	{
		std::string Name = "Tag";
	};


	/////////////////////////
	// Physical Components //
	/////////////////////////

	struct TransformComponent
	{
	public:
		~TransformComponent()
		{
			for (auto* Child : Children)
			{
				Child->SetParent(nullptr);
			}

			if (Parent)
				Parent->RemoveChild(this);
		}

		inline glm::vec3 GetLocalPosition() const { return LocalPosition; };
		inline glm::vec2 GetLocalScale() const { return LocalScale; };
		inline float GetLocalOrientation() const { return LocalOrientation; };

		inline glm::vec3 GetGlobalPosition() const { return GlobalPosition; };
		inline glm::vec2 GetGlobalScale() const { return GlobalScale; };
		inline float GetGlobalOrientation() const { return GlobalOrientation; };

		inline void operator+= (glm::vec3 Position) { LocalPosition += Position; MarkAsDirty(this); };
		inline void operator-= (glm::vec3 Position) { LocalPosition -= Position; MarkAsDirty(this); };
		inline void operator*= (glm::vec3 Position) { LocalPosition *= Position; MarkAsDirty(this); };
		inline void operator/= (glm::vec3 Position) { LocalPosition /= Position; MarkAsDirty(this); };
		inline void operator=  (glm::vec3 Position) { LocalPosition = Position; MarkAsDirty(this); };

		inline void operator+= (glm::vec2 Scale) { LocalScale += Scale; MarkAsDirty(this); };
		inline void operator-= (glm::vec2 Scale) { LocalScale -= Scale; MarkAsDirty(this); };
		inline void operator*= (glm::vec2 Scale) { LocalScale *= Scale; MarkAsDirty(this); };
		inline void operator/= (glm::vec2 Scale) { LocalScale /= Scale; MarkAsDirty(this); };
		inline void operator=  (glm::vec2 Scale) { LocalScale = Scale; MarkAsDirty(this); };

		inline void operator+= (float Orientation) { LocalOrientation += Orientation; MarkAsDirty(this); };
		inline void operator-= (float Orientation) { LocalOrientation -= Orientation; MarkAsDirty(this); };
		inline void operator*= (float Orientation) { LocalOrientation *= Orientation; MarkAsDirty(this); };
		inline void operator/= (float Orientation) { LocalOrientation /= Orientation; MarkAsDirty(this); };
		inline void operator=  (float Orientation) { LocalOrientation = Orientation; MarkAsDirty(this); };

	public:
		void SetParent(TransformComponent* Transform)
		{
			if (Transform == Parent) return;

			if (Parent != nullptr)
				Parent->RemoveChild(this);

			Parent = Transform;

			if (Transform != nullptr)
				Transform->AddChild(this);

			MarkAsDirty(this);
		}

		inline TransformComponent* GetParent() const { return Parent; };
		inline std::vector<TransformComponent*>& GetChildren() { return Children; };

	public:
		/*
			This is mainly used by the renderer, as scripts don't really have a need for messing directly
			with transformations (as of right now but that might change)
		*/

		void RecalculateTransform()
		{
			if (FLAG_TransformDirty)
			{
				if (Parent)
				{
					if (Parent->FLAG_TransformDirty)
					{
						Parent->RecalculateTransform();
					}

					GlobalScale = LocalScale * Parent->GlobalScale;
					GlobalOrientation = LocalOrientation + Parent->GlobalOrientation;
					
					auto rad = GlobalOrientation * M_PI / 180;
					auto c = cos(rad);
					auto s = sin(rad);
					
					auto ParentGlobalPosition = Parent->GlobalPosition;
					auto ScaledPosition = LocalPosition * glm::vec3(Parent->GlobalScale, 0.f);

					GlobalPosition = ParentGlobalPosition + glm::vec3(ScaledPosition.x * c - ScaledPosition.y * s , ScaledPosition.x * s + ScaledPosition.y * c, LocalPosition.z);
				}
				else
				{
					GlobalScale = LocalScale;
					GlobalOrientation = LocalOrientation;
					GlobalPosition = LocalPosition;
				}

				GlobalTransform = glm::translate(glm::mat4(1.f), GlobalPosition)
					* glm::rotate(glm::mat4(1.f), glm::radians(GlobalOrientation), glm::vec3(0.f, 0.f, 1.f))
					* glm::scale(glm::mat4(1.f), glm::vec3(GlobalScale, 1.f));

				FLAG_TransformDirty = false;
			}
		}

		inline glm::mat4 GetGlobalTransform(const glm::vec2& QuadSize = glm::vec2(1.f, 1.f)) { RecalculateTransform(); return GlobalTransform; }

	protected:
		glm::vec3 LocalPosition{ 0.f, 0.f, 0.f };
		glm::vec2 LocalScale{ 1.f, 1.f };
		float LocalOrientation = 0.f;

		glm::vec3 GlobalPosition{ 0.f, 0.f, 0.f };
		glm::vec2 GlobalScale{ 1.f, 1.f };
		float GlobalOrientation = 0.f;

		glm::mat4 GlobalTransform{ 1.f };

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
		
	private:
		static void MarkAsDirty(TransformComponent* Component)
		{
			Component->FLAG_TransformComponentsDirty = true;
			Component->FLAG_TransformDirty = true;

			for (auto* Transform : Component->GetChildren())
			{
				MarkAsDirty(Transform);
			}
		}

		bool FLAG_TransformComponentsDirty = true;
		bool FLAG_TransformDirty = true;

		friend class Scene;
	};

	struct RigidBodyComponent
	{
		float Velocity = 0.f;

		float Friction = 100.f;
	};

	struct CollisionMeshComponent
	{

	};


	////////////////////
	// Sprite-related //
	////////////////////

	struct SpriteComponent
	{
		Colour SpriteColour{ 1.f, 1.f, 1.f };

		inline void SetSprite(const std::string& NewSprite) { aSprite = SpriteAtlas::GetSprite(NewSprite); };
		inline Ptr<Sprite> GetSprite() const { return aSprite; };

		void SetRGBColour(const Colour& colour) { SpriteColour = colour; };
		
	private:
		Ptr<Sprite> aSprite;
	};

	struct AnimationComponent
	{

	};

	//////////////////////
	// Viewport related //
	//////////////////////

	struct CameraComponent
	{
		// Builds orthographic matrix for the rendering API to use.
		void SetProjection(glm::vec2 Size, float pNear, float pFar)
		{
			Projection = glm::ortho(0.f, Size.x, Size.y, 0.f, pNear, pFar);

			ViewportResolution = Size;
			Near = pNear;
			Far = pFar;
		}

		inline glm::mat4 GetTransform(const glm::vec3& Position) const
		{
			return glm::translate(glm::mat4(1.f), Position - (glm::vec3(GetViewportResolution(), 0.f) / 2.f));
		}

		inline glm::mat4 GetProjection() const { return Projection; };
		inline std::pair<float, float> GetZPlane() const{ return { Near, Far }; }; // First is Near, second is Far
		
		inline glm::vec2 GetViewportSize() const { return ViewportSize; };
		inline glm::vec2 GetViewportResolution() const { return ViewportResolution; };

	private:

		glm::vec2 ViewportSize = { 0.f, 0.f }; // [currently unused] to-do: add support for multiple viewports??
		glm::vec2 ViewportResolution = { 1280.f, 720.f };

		float Near = -100.f;
		float Far = 100.f;

		glm::mat4 Projection = glm::ortho(0.f, 1280.f, 720.f, 0.f, -100.f, 100.f);
	};


	////////////////////
	// Script-related //
	////////////////////

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