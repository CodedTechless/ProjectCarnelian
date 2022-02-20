#pragma once

#include <engineincl.h>

#include <engine/application/application.h>

#include <engine/sprite/sprite_atlas.h>
#include <engine/lua/script_environment.h>

#include <engine/entity/scriptable_entity.h>
#include <engine/maths/colour.hpp>

#include <json/json.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/matrix_interpolation.hpp>
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

	struct TransformState
	{
		glm::vec3 Position{ 0.f };
		glm::vec2 Scale{ 0.f };
		float Orientation = 0.f;

		glm::mat4 Transform{ 1.f };
	};

	struct TransformRecalculateResult
	{
		TransformState GlobalState{};
		TransformState InterpState{};
	};


	struct TransformComponent : public BaseComponent
	{
	public:
		TransformComponent() = default;
		TransformComponent(const TransformComponent& component) = default;

		inline glm::vec3 GetLocalPosition() const { return LocalPosition; };
		inline glm::vec2 GetLocalScale() const { return LocalScale; };
		inline float GetLocalOrientation() const { return LocalOrientation; };

		inline glm::vec3 GetGlobalPosition() { return RecalculateBase().GlobalState.Position; };
		inline glm::vec2 GetGlobalScale() { return RecalculateBase().GlobalState.Scale; };
		inline float GetGlobalOrientation() { return RecalculateBase().GlobalState.Orientation; };
		
		inline glm::mat4 GetGlobalTransform() 
		{ 
			auto Base = RecalculateBase();
			
			if (FLAG_DoInterpolation)
				return Base.InterpState.Transform;
			else
				return Base.GlobalState.Transform;
		};

		inline void SetLocalPosition(glm::vec3 Position) { if (LocalPosition == Position) { return; }; LocalPosition = Position; MarkAsDirty(); };
		inline void SetLocalScale(glm::vec2 Scale) { if (LocalScale == Scale) { return; } LocalScale = Scale; MarkAsDirty(); };
		inline void SetLocalOrientation(float Orientation) { if (LocalOrientation == Orientation) { return; } LocalOrientation = Orientation; MarkAsDirty(); };

		inline TransformState GetLocalTransformState()
		{
			return { LocalPosition, LocalScale, LocalOrientation };
		}

		inline TransformState GetGlobalTransformState() 
		{ 
			return { GlobalPosition, GlobalScale, GlobalOrientation, GlobalTransform };
		}

		inline void SetEngineInterpolation(bool Mode) 
		{ 
			if (FLAG_DoInterpolation == Mode)
				return;
			
			FLAG_DoInterpolation = Mode; 
			
			for (Entity* LinkedChild : LinkedEntity->GetChildren())
			{
				LinkedChild->GetComponent<TransformComponent>().SetEngineInterpolation(Mode);
			}
		};

		inline bool IsEngineInterpolationEnabled() const { return FLAG_DoInterpolation; };

		inline void ForceReloadPreviousState()
		{
			PreviousState = GetGlobalTransformState();
		}

	private:

		TransformRecalculateResult RecalculateBase()
		{
			/*

				to-do: optimise this system [UPDATE 19/02/22: optimised a bit but could probably be better!]
			*/

			TransformRecalculateResult Result { GetGlobalTransformState() };
			TransformRecalculateResult ParentTransformState{};
			bool FetchedBase = false;
			

			if (FLAG_TransformDirty)
			{
				Entity* Parent = LinkedEntity->GetParent();

				if (Parent)
				{
					TransformComponent& ParentTransform = Parent->GetComponent<TransformComponent>();
					ParentTransformState = ParentTransform.RecalculateBase();
					FetchedBase = true;

					Result.GlobalState = BuildTransformState(GetLocalTransformState(), ParentTransformState.GlobalState);
				}
				else
				{
					Result.GlobalState = GetLocalTransformState();
				}
				
				GlobalTransform = Result.GlobalState.Transform;
				GlobalPosition = Result.GlobalState.Position;
				GlobalScale = Result.GlobalState.Scale;
				GlobalOrientation = Result.GlobalState.Orientation;

				FLAG_TransformDirty = false;
			}

			if (FLAG_DoInterpolation && !MatchesState(this, PreviousState))
			{
				Entity* Parent = LinkedEntity->GetParent();

				if (Parent)
				{
					TransformComponent& ParentTransform = Parent->GetComponent<TransformComponent>();
					
					// if the parent transform is an interpolation transform, build my interpolation state based of its interpolation state instead of the global state
					if (ParentTransform.FLAG_DoInterpolation)
					{
						if (!FetchedBase)
							ParentTransformState = ParentTransform.RecalculateBase();
						
						Result.InterpState = BuildTransformState(GetLocalTransformState(), ParentTransformState.InterpState);
					}
					// if the parent transform is not interpolated (or there is no parent), then use my previous state and current global state to build an interpolated value!
					else
					{
						Result.InterpState = Interpolate(Result.GlobalState, PreviousState);
					}
				}
				else
				{
					Result.InterpState = Interpolate(Result.GlobalState, PreviousState);
				}
			}
			else
			{
				Result.InterpState = Result.GlobalState;
			}

			return Result;
		}

		static bool MatchesState(TransformComponent* Component, const TransformState& StateToMatch)
		{
			return Component->GlobalPosition == StateToMatch.Position && Component->GlobalScale == StateToMatch.Scale && Component->GlobalOrientation == StateToMatch.Orientation;
		}

		// builds a state where LocalState is relative to ParentState's coordinate space. does not build the state for you.
		static TransformState BuildTransformState(const TransformState& LocalState, const TransformState& ParentState)
		{
			TransformState State{};
			
			State.Scale = LocalState.Scale * ParentState.Scale;
			State.Orientation = LocalState.Orientation + ParentState.Orientation;

			float S = 0, C = 1;
			if (ParentState.Orientation != 0)
			{
				C = cos(ParentState.Orientation);
				S = sin(ParentState.Orientation);
			}

			auto ParentPosition = ParentState.Position;
			auto ScaledPosition = LocalState.Position * glm::vec3(ParentState.Scale, 0.f);

			State.Position = ParentPosition + glm::vec3(ScaledPosition.x * C - ScaledPosition.y * S, ScaledPosition.x * S + ScaledPosition.y * C, LocalState.Position.z);

			BuildTransform(State);

			return State;
		}

		// builds the transform of any State passed to it
		static void BuildTransform(TransformState& State)
		{
			State.Transform = glm::translate(glm::mat4(1.f), State.Position);

			if (State.Orientation != 0.f)
			{
				State.Transform *= glm::rotate(glm::mat4(1.f), State.Orientation, glm::vec3(0.f, 0.f, 1.f));
			}

			if (State.Scale != glm::vec2(1.f, 1.f))
			{
				State.Transform *= glm::scale(glm::mat4(1.f), glm::vec3(State.Scale, 1.f));
			}
		}

		// interpolates Last towards Next and returns the result as a TransformState. builds the state for you, too!
		static TransformState Interpolate(const TransformState& Next, const TransformState& Last)
		{
			float Ratio = Application::GetActiveApplication().GetSimulationRatio();

			TransformState State = Last;
			if (Last.Position != Next.Position) State.Position = glm::mix(Last.Position, Next.Position, Ratio); else State.Position = Last.Position;
			if (Last.Scale != Next.Scale) State.Scale = glm::mix(Last.Scale, Next.Scale, Ratio); else State.Scale = Last.Scale;
			if (Last.Orientation != Next.Orientation) State.Orientation = Last.Orientation + Ratio * std::fmodf(Next.Orientation - Last.Orientation, 2.f * M_PI); else State.Orientation = Last.Orientation;

			BuildTransform(State);

			return State;
		}

		/*
			This is mainly used by the renderer, as scripts don't really have a need for messing directly
			with transformations (as of right now but that might change)

			Recalculates the transform based on parents positions. Stores it so that it doesn't need to be 
			recalculated every time its requested.
		*/


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

		// The final calculated global position, scale and orientation.
		glm::vec3 GlobalPosition { 0.f, 0.f, 0.f };
		glm::vec2 GlobalScale { 1.f, 1.f };
		float GlobalOrientation = 0.f;

		// A TransformState storing the previous state of the transform. This will be the value before it was last updated.
		TransformState PreviousState{};

		// The final global transform to be used by the renderer.
		glm::mat4 GlobalTransform{ 1.f };
		
		bool FLAG_TransformDirty = true; // Signifies whether or not the transformation is dirty. If FLAG_DoInterpolation is TRUE, this isn't used and is instead replaced by Last == Next
		bool FLAG_DoInterpolation = false; // Signifies whether or not to use state interplotation.

		friend class Entity;
		friend class Scene;

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

		inline void SetSprite(Ptr<Sprite> sprite) { aSprite = sprite; };
		inline Ptr<Sprite> GetSprite() const { return aSprite; };
		
	private:
		Ptr<Sprite> aSprite = nullptr;

	public: // json serialisation

		inline friend void to_json(JSON& json, const SpriteComponent& component)
		{

			json = JSON{
				{"SpriteColour", component.SpriteColour}, // warning: if you get an error related to json it's probably this
				{"SpriteName", component.aSprite ? component.aSprite->GetName() : ""}
			};
		}

		inline friend void from_json(const JSON& json, SpriteComponent& component)
		{
			json.at("SpriteColour").get_to(component.SpriteColour);
			
			std::string Name = json.at("SpriteName").get<std::string>();
			
			if (SpriteAtlas::Has(Name))
			{
				Ptr<Sprite> Sprite = SpriteAtlas::Get(Name);
				component.SetSprite(Sprite);
			}
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

		void Unbind()
		{
			ScriptEnvironment::ResetEntity(LinkedEntity->GetScene()->GetLuaID(), LinkedEntity);
		}

		void Bind(const std::string& name)
		{
			if (Loaded)
				Unbind();

			if (ScriptEnvironment::Has(name))
			{
				ScriptEnvironment::RegisterEntityScript(name, LinkedEntity);
				Name = name;

				Debug::Log("Loaded script " + name + " into " + LinkedEntity->GetID(), "LuaScriptBinding");

				Loaded = true;
			}
			else
			{
				Debug::Error("Script with name " + name + " does not exist.", "LuaScriptBinding");
			}
		}

		inline bool IsLoaded() const { return Loaded; };
		inline std::string GetScriptName() const { return Name; };

	private:
		std::string Name = "";
		bool Loaded = false;

		friend class Scene;

	public:

		inline friend void to_json(JSON& json, const LuaScriptComponent& component)
		{
			json = JSON{
				{"ScriptName", component.Name}
			};
		}

		inline friend void from_json(const JSON& json, LuaScriptComponent& component)
		{
			std::string Name = json.at("ScriptName").get<std::string>();
			if (ScriptEnvironment::Has(Name))
				component.Bind(Name);
		}
	};
}