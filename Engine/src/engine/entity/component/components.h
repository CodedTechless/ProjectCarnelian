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

		inline glm::mat4 GetGlobalTransform() { RecalculateTransforms(); return GlobalTransform; };

		inline void operator+= (glm::vec3 Position) { LocalPosition += Position; MarkAsDirty(this); };
		inline void operator-= (glm::vec3 Position) { LocalPosition -= Position; MarkAsDirty(this); };
		inline void operator*= (glm::vec3 Position) { LocalPosition *= Position; MarkAsDirty(this); };
		inline void operator/= (glm::vec3 Position) { LocalPosition /= Position; MarkAsDirty(this); };
		inline void operator=  (glm::vec3 Position) { LocalPosition = Position; MarkAsDirty(this); };

		inline void operator+= (glm::vec2 Scale) { LocalScale /= Scale; MarkAsDirty(this); };
		inline void operator-= (glm::vec2 Scale) { LocalScale /= Scale; MarkAsDirty(this); };
		inline void operator*= (glm::vec2 Scale) { LocalScale /= Scale; MarkAsDirty(this); };
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

	protected:
		glm::vec3 LocalPosition{ 0.f, 0.f, 0.f };
		glm::vec2 LocalScale{ 1.f, 1.f };
		float LocalOrientation = 0.f;

		glm::vec3 GlobalPosition{ 0.f, 0.f, 0.f };
		glm::vec2 GlobalScale{ 1.f, 1.f };
		float GlobalOrientation = 0.f;

		glm::mat4 GlobalTransform{ 1.f };

	private:
		void RecalculateTransforms()
		{
			if (FLAG_TransformDirty)
			{
				if (Parent)
				{
					GlobalScale = LocalScale * Parent->GlobalScale;
					GlobalOrientation = LocalOrientation + Parent->GlobalOrientation;
					
					auto GlobalScaledPosition = LocalPosition * glm::vec3(Parent->GlobalScale, 1.f);
					GlobalPosition = glm::vec3(std::cos(GlobalOrientation * M_PI) * GlobalScaledPosition.x, std::sin(GlobalOrientation * M_PI) * GlobalScaledPosition.y, GlobalPosition.z);
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
			}
		}

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
			Component->FLAG_TransformDirty = true;

			for (auto* Transform : Component->GetChildren())
			{
				MarkAsDirty(Transform);
			}
		}

		bool FLAG_TransformDirty = false;

		friend class Scene;
	};

	struct RigidBodyComponent
	{

	};


	////////////////////
	// Sprite-related //
	////////////////////

	struct SpriteComponent
	{
		Ptr<Sprite> aSprite;

		Colour SpriteColour{ 1.f, 1.f, 1.f };
		glm::vec3 Offset{ 0.f, 0.f, 0.f };

		inline void SetSprite(const std::string& NewSprite) { aSprite = SpriteAtlas::GetSprite(NewSprite); };

		void SetRGBColour(const Colour& colour) { SpriteColour = colour; };
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