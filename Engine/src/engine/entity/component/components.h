#pragma once

#include <engineincl.h>

#include <engine/sprite/sprite_atlas.h>
#include <engine/lua/script_environment.h>

#include <engine/entity/scriptable_entity.h>
#include <engine/maths/colour.hpp>

#include <json/json.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

using JSON = nlohmann::json;

namespace Techless
{
	
	/*
	namespace Serialiser
	{

		template <typename Type>
		bool CompareTypeForName(const std::string& Name)
		{
			return Name == type;
		}

		template <typename Component>
		std::string GetTypeEntryName()
		{
			std::string Name = typeid(Component).name();
			
			if (CompareTypeForName<TagComponent>(Name)) return "Tag";
			if (CompareTypeForName<TransformComponent>(Name)) return "Transform";
			if (CompareTypeForName<RigidBodyComponent>(Name)) return "RigidBody";
			if (CompareTypeForName<SpriteComponent>(Name)) return "Sprite";
			if (CompareTypeForName<CameraComponent>(Name)) return "Camera";

			assert(false);
		}
	}
	
	// i never finished this because i wasn't able to make it work so...
	*/

	struct BaseComponent
	{
		BaseComponent() = default;
		virtual ~BaseComponent() = default;

		inline Entity* GetLinkedEntity() const { return LinkedEntity; };

	protected:
		Entity* LinkedEntity = nullptr;

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


	/////////////////////////
	// Physical Components //
	/////////////////////////

	// to-do: clean this up

	struct TransformComponent : public BaseComponent
	{
	public:
		TransformComponent() = default;
		TransformComponent(const TransformComponent& component) = default;

		inline glm::vec3 GetLocalPosition() const { return LocalPosition; };
		inline glm::vec2 GetLocalScale() const { return LocalScale; };
		inline float GetLocalOrientation() const { return LocalOrientation; };

		inline glm::vec3 GetGlobalPosition() { RecalculateTransform(); return GlobalPosition; };
		inline glm::vec2 GetGlobalScale() { RecalculateTransform(); return GlobalScale; };
		inline float GetGlobalOrientation() { RecalculateTransform(); return GlobalOrientation; };
		
		inline void SetLocalPosition(glm::vec3 Position) { LocalPosition = Position; MarkAsDirty(); };
		inline void SetLocalScale(glm::vec2 Scale) { LocalScale = Scale; MarkAsDirty(); };
		inline void SetLocalOrientation(float Rotation) { LocalOrientation = Rotation; MarkAsDirty(); };

		/*
			to-do: find a nicer way to do this please, this is disgusting.
		*/

		inline glm::mat4 GetGlobalTransform() { RecalculateTransform(); return GlobalTransform; }

	private:

		/*
			This is mainly used by the renderer, as scripts don't really have a need for messing directly
			with transformations (as of right now but that might change)

			Recalculates the transform based on parents positions. Stores it so that it doesn't need to be 
			recalculated every time its requested.
		*/
		void RecalculateTransform()
		{
			if (FLAG_TransformDirty)
			{
				Entity* Parent = LinkedEntity->GetParent();

				if (Parent)
				{
					TransformComponent& ParentTransform = Parent->GetComponent<TransformComponent>();
					ParentTransform.RecalculateTransform();

					GlobalScale = LocalScale * ParentTransform.GlobalScale;
					GlobalOrientation = LocalOrientation + ParentTransform.GlobalOrientation;
					
					auto rad = ParentTransform.GlobalOrientation * M_PI / 180;
					auto c = cos(rad);
					auto s = sin(rad);
					
					auto ParentGlobalPosition = ParentTransform.GlobalPosition;
					auto ScaledPosition = LocalPosition * glm::vec3(ParentTransform.GlobalScale, 0.f);

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

		// using a dirty flag to signify when a global position/scale/orientation does not match up with its parents position or with its own local position.
		void MarkAsDirty()
		{
			if (FLAG_TransformDirty)
				return;

			FLAG_TransformDirty = true;

			for (Entity* LinkedChild : LinkedEntity->GetChildren())
			{
				LinkedChild->GetComponent<TransformComponent>().MarkAsDirty();
			}
		}

		glm::vec3 LocalPosition{ 0.f, 0.f, 0.f };
		glm::vec2 LocalScale{ 1.f, 1.f };
		float LocalOrientation = 0.f;

		glm::vec3 GlobalPosition{ 0.f, 0.f, 0.f };
		glm::vec2 GlobalScale{ 1.f, 1.f };
		float GlobalOrientation = 0.f;

		glm::mat4 GlobalTransform{ 1.f };

		bool FLAG_TransformDirty = true;

		friend class Entity;
		friend class Scene;
		friend class ScriptAtlas;

	public: // json serialisation
		
		inline friend void to_json(JSON& json, const TransformComponent& component)
		{
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
				{"LocalOrientation", component.LocalOrientation}
			};
		}

		inline friend void from_json(const JSON& json, TransformComponent& component)
		{
			auto& LocalPosition = json.at("LocalPosition");
			component.LocalPosition = glm::vec3(LocalPosition["x"].get<float>(), LocalPosition["y"].get<float>(), LocalPosition["z"].get<float>());

			auto& LocalScale = json.at("LocalScale");
			component.LocalScale = glm::vec2(LocalScale["x"].get<float>(), LocalScale["y"].get<float>());

			json.at("LocalOrientation").get_to(component.LocalOrientation);

			component.MarkAsDirty();
		}
	};

	struct RigidBodyComponent : public BaseComponent
	{
	public:
		RigidBodyComponent() = default;
		RigidBodyComponent(const RigidBodyComponent& component) = default;

		float Velocity = 0.f;
		float Friction = 100.f;

	public: // json serialisation

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

	struct SpriteComponent : public BaseComponent
	{
	public:
		SpriteComponent() = default;
		SpriteComponent(const SpriteComponent& component) = default;

		Colour SpriteColour{ 1.f, 1.f, 1.f };

		inline void SetSprite(const std::string& spriteName) { aSprite = SpriteAtlas::Get(spriteName); };
		inline Ptr<Sprite> GetSprite() const { return aSprite; };
		inline std::string GetSpriteName() const { return aSprite->GetName(); };
		
	private:
		Ptr<Sprite> aSprite = nullptr;

	public: // json serialisation

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

	struct AnimatorComponent : public BaseComponent
	{
	public:
		AnimatorComponent() = default;
		AnimatorComponent(const AnimatorComponent& component) = default;
		
		/*
	public: // json serialisation

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

	struct CameraComponent : public BaseComponent
	{
	public:
		CameraComponent() = default;
		CameraComponent(const CameraComponent& component) = default;

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
		
		inline glm::vec2 GetViewportResolution() const { return ViewportResolution; };

	private:

		glm::vec2 ViewportResolution = { 1280.f, 720.f };

		float Near = -100.f;
		float Far = 100.f;

		glm::mat4 Projection = glm::ortho(0.f, 1280.f, 720.f, 0.f, -100.f, 100.f);

	public: // json serialisation

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

	struct ScriptComponent : public BaseComponent
	{
	public:
		// script component is entirely native so there's no way to serialise it without
		// extreme difficulty! therefore, i don't provide support for it at all :)

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

	struct LuaScriptComponent : public BaseComponent
	{
	public:
		LuaScriptComponent() = default;
		LuaScriptComponent(const LuaScriptComponent& component) = default;

		void Bind(const std::string& Name)
		{
			Instance = ScriptEnvironment::Create(Name, LinkedEntity);
		}

	private:
		Ptr<sol::environment> Instance;

	};
}