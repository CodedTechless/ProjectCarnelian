#pragma once

#include <engineincl.h>
#include <engine/sprite/sprite_atlas.h>
#include <engine/entity/scriptable_entity.h>
#include <engine/maths/colour.hpp>

#include <json/json.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

using JSON = nlohmann::json;

namespace Techless
{
	
	//////////////////////
	// Basic components //
	//////////////////////

	struct TagComponent
	{
		std::string Name = "Tag";

	public:
		NLOHMANN_DEFINE_TYPE_INTRUSIVE(TagComponent, Name);
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
				Child->SetParent(Parent ? Parent : nullptr);
			}

			if (Parent)
				Parent->RemoveChild(this);
		}

		inline glm::vec3 GetLocalPosition() const { return LocalPosition; };
		inline glm::vec2 GetLocalScale() const { return LocalScale; };
		inline float GetLocalOrientation() const { return LocalOrientation; };

		inline glm::vec3 GetGlobalPosition() { RecalculateTransform(); return GlobalPosition; };
		inline glm::vec2 GetGlobalScale() { RecalculateTransform(); return GlobalScale; };
		inline float GetGlobalOrientation() { RecalculateTransform(); return GlobalOrientation; };
		
		/*
			to-do: find a nicer way to do this please, this is disgusting.
		*/

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

		static bool IsParentOfSelf(TransformComponent* OriginalTransform, TransformComponent* Transform)
		{
			if (Transform == nullptr)
				return false;

			if (OriginalTransform == Transform)
				return true;

			return IsParentOfSelf(OriginalTransform, Transform->Parent);
		}

		bool SetParent(TransformComponent* Transform)
		{
			if (Transform && IsParentOfSelf(this, Transform))
				return false;

			if (Parent != nullptr)
				Parent->RemoveChild(this);

			Parent = Transform;

			if (Transform != nullptr)
				Transform->AddChild(this);

			MarkAsDirty(this);

			return true;
		}

		inline TransformComponent* GetParent() const { return Parent; };
		inline std::vector<TransformComponent*>& GetChildren() { return Children; };

	public:
		/*
			This is mainly used by the renderer, as scripts don't really have a need for messing directly
			with transformations (as of right now but that might change)

			Recalculates the transform based on parents positions
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
		// using a dirty flag to signify when a global position/scale/orientation does not match up with its parents position or with its own local position.

		static void MarkAsDirty(TransformComponent* Component)
		{
			if (Component->FLAG_TransformDirty)
				return;

			Component->FLAG_TransformDirty = true;

			for (auto* Transform : Component->GetChildren())
			{
				MarkAsDirty(Transform);
			}
		}

		bool FLAG_TransformDirty = true;

		friend class Scene;

	public:

		// json serialisation

		inline friend void to_json(JSON& json, const TransformComponent& component)
		{
			// we have no access to entities here, therefore the Parent can only be serialised by a separate part of the program

			json = JSON{
				{"LocalPosition", {
					{"x", component.LocalPosition.x},
					{"y", component.LocalPosition.y},
					{"z", component.LocalPosition.z}
				}},
				{"LocalScale", {
					{"x", component.LocalScale.x},
					{"y", component.LocalScale.y}
				}},
				{"LocalOrientation", component.LocalOrientation},
				{"Parent", nullptr}
			};
		}

		inline friend void from_json(const JSON& json, TransformComponent& component)
		{
			auto& LocalPosition = json.at("LocalPosition");
			component.LocalPosition = glm::vec3(LocalPosition["x"].get<float>(), LocalPosition["y"].get<float>(), LocalPosition["z"].get<float>());

			auto& LocalScale = json.at("LocalScale");
			component.LocalScale = glm::vec2(LocalScale["x"].get<float>(), LocalScale["y"].get<float>());

			json.at("LocalOrientation").get_to(component.LocalOrientation);

			MarkAsDirty(&component);
		}
	};

	struct RigidBodyComponent
	{
		float Velocity = 0.f;
		float Friction = 100.f;

	public:

		// json serialisation

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(RigidBodyComponent, Velocity, Friction);
	};

	/*
	struct CollisionMeshComponent
	{

	};
	*/

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

	public:

		// json serialisation

		inline friend void to_json(JSON& json, const SpriteComponent& component)
		{
			json = JSON{
				{"SpriteColour", component.SpriteColour}, // warning: if you get an error related to json it's probably this
				{"SpriteName", component.aSprite->GetName()}
			};
		}

		inline friend void from_json(const JSON& json, SpriteComponent& component)
		{
			json.at("SpriteColour").get_to(component.SpriteColour);
			
			auto SpriteName = json.at("SpriteName").get<std::string>();
			component.SetSprite(SpriteName);
		}
	};

	struct AnimatorComponent
	{
		/*
	public:

		inline friend void to_json(JSON& json, const AnimatorComponent& component)
		{

		}

		inline friend void from_json(const JSON& json, AnimatorComponent& component)
		{

		}*/
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

	public:

		// json serialisation

		inline friend void to_json(JSON& json, const CameraComponent& component)
		{
			json = JSON{
				{"ViewportResolution", {
					{"x", component.ViewportResolution.x},
					{"y", component.ViewportResolution.y},
				}},
				{"zPlane", {
					{"Near", component.Near},
					{"Far", component.Far}
				}}
			};
		}

		inline friend void from_json(const JSON& json, CameraComponent& component)
		{
			auto& ViewportResolution = json.at("ViewportResolution");
			auto j_ViewportResolution = glm::vec2(ViewportResolution.at("x").get<float>(), ViewportResolution.at("y").get<float>());

			auto& zPlane = json.at("zPlane");
			auto j_Near = zPlane.at("Near").get<float>();
			auto j_Far = zPlane.at("Far").get<float>();

			component.SetProjection(j_ViewportResolution, j_Near, j_Far);
		}
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

	public:

		/*
		inline friend void to_json(JSON& json, const ScriptComponent& component)
		{

		}

		inline friend void from_json(const JSON& json, ScriptComponent& component)
		{

		}*/
	};
}