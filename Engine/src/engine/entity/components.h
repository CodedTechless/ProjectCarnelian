#pragma once

#include <engineincl.h>

#include <engine/application/application.h>

#include <engine/sprite/sprite_atlas.h>
#include <engine/sprite/animation/animation_atlas.h>
#include <engine/lua/script_environment.h>

#include <engine/entity/scriptable_entity.h>

#include <render/buffer/frame.h>

#include <glm/gtx/matrix_interpolation.hpp>
#include <glm/gtx/transform.hpp>

namespace Techless
{



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
		Vector3 Position{ 0.f };
		Vector2 Scale{ 0.f };
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

		inline Vector3 GetLocalPosition() const { return LocalPosition; };
		inline Vector2 GetLocalScale() const { return LocalScale; };
		inline float GetLocalOrientation() const { return LocalOrientation; };

		inline Vector3 GetGlobalPosition() { if (FLAG_DoInterpolation) return RecalculateBase().InterpState.Position; else return RecalculateBase().GlobalState.Position; };
		inline Vector2 GetGlobalScale() { if (FLAG_DoInterpolation) return RecalculateBase().InterpState.Scale; else return RecalculateBase().GlobalState.Scale; };
		inline float GetGlobalOrientation() { if (FLAG_DoInterpolation) return RecalculateBase().InterpState.Orientation; else return RecalculateBase().GlobalState.Orientation; };
		
		glm::mat4 GetGlobalTransform() 
		{ 
			auto Base = RecalculateBase();
			
			if (FLAG_DoInterpolation)
				return Base.InterpState.Transform;
			else
				return Base.GlobalState.Transform;
		};

		void SetLocalPosition(Vector3 Position) { if (LocalPosition == Position) { return; }; LocalPosition = Position; MarkAsDirty(); };
		void SetLocalScale(Vector2 Scale) { if (LocalScale == Scale) { return; } LocalScale = Scale; MarkAsDirty(); };
		void SetLocalOrientation(float Orientation) { if (LocalOrientation == Orientation) { return; } LocalOrientation = Orientation; MarkAsDirty(); };

		TransformState GetLocalTransformState()
		{
			return { LocalPosition, LocalScale, LocalOrientation };
		}

		TransformState GetGlobalTransformState()
		{ 
			return { GlobalPosition, GlobalScale, GlobalOrientation, GlobalTransform };
		}

		TransformState GetPreviousState()
		{
			return PreviousState;
		}

		TransformState GetCurrentState() // grabs the latest calculated interp/global state WITHOUT forcing an update. mainly used for debug ops
		{
			return CurrentState;
		}

		void SetEngineInterpolation(bool Mode) 
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

		void ForceInterpolationUpdate()
		{
			if (FLAG_DoInterpolation)
			{
				PreviousState = CurrentState;
			}
		}

		//uint InterpolatedFrames = 0;


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

				BuildTransform(Result.GlobalState);
				
				GlobalTransform = Result.GlobalState.Transform;
				GlobalPosition = Result.GlobalState.Position;
				GlobalScale = Result.GlobalState.Scale;
				GlobalOrientation = Result.GlobalState.Orientation;

				FLAG_TransformDirty = false;

				CurrentState = Result.GlobalState;
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

				BuildTransform(Result.InterpState);

				//InterpolatedFrames++;
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
			auto ScaledPosition = LocalState.Position * Vector3(ParentState.Scale, 0.f);

			State.Position = ParentPosition + Vector3(ScaledPosition.x * C - ScaledPosition.y * S, ScaledPosition.x * S + ScaledPosition.y * C, LocalState.Position.z);

			BuildTransform(State);

			return State;
		}

		// builds the transform of any State passed to it
		static void BuildTransform(TransformState& State)
		{
			State.Transform = glm::translate(glm::mat4(1.f), State.Position);

			if (State.Orientation != 0.f)
			{
				State.Transform *= glm::rotate(glm::mat4(1.f), State.Orientation, Vector3(0.f, 0.f, 1.f));
			}

			if (State.Scale != Vector2(1.f, 1.f))
			{
				State.Transform *= glm::scale(glm::mat4(1.f), Vector3(State.Scale, 1.f));
			}
		}

		// interpolates Last towards Next and returns the result as a TransformState.
		static TransformState Interpolate(const TransformState& Next, const TransformState& Last)
		{
			float Ratio = Application::GetActiveApplication().GetSimulationRatio();

			TransformState State = Last;
			if (Last.Position != Next.Position) State.Position = glm::mix(Last.Position, Next.Position, Ratio); else State.Position = Last.Position;
			if (Last.Scale != Next.Scale) State.Scale = glm::mix(Last.Scale, Next.Scale, Ratio); else State.Scale = Last.Scale;
			if (Last.Orientation != Next.Orientation) State.Orientation = Last.Orientation + Ratio * std::fmodf(Next.Orientation - Last.Orientation, 2.f * M_PI); else State.Orientation = Last.Orientation;

			return State;
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


		Vector3 LocalPosition{ 0.f, 0.f, 0.f };
		Vector2 LocalScale{ 1.f, 1.f };
		float LocalOrientation = 0.f;

		// The final calculated global position, scale and orientation.
		Vector3 GlobalPosition { 0.f, 0.f, 0.f };
		Vector2 GlobalScale { 1.f, 1.f };
		float GlobalOrientation = 0.f;

		// A TransformState storing the previous state of the transform. This will be the value before it was last updated.
		TransformState CurrentState{};
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
			auto& p = component.LocalPosition;
			auto& s = component.LocalScale;

			json = JSON{
				{"LocalPosition", { {"X", p.x}, {"Y", p.y}, {"Z", p.z} }},
				{"LocalScale", { {"X", s.x}, {"Y", s.y} }},
				{"LocalOrientation", component.LocalOrientation}
			};
		}

		inline friend void from_json(const JSON& json, TransformComponent& component)
		{
			const JSON& Position = json.at("LocalPosition");
			component.LocalPosition = { Position.at("X").get<float>(), Position.at("Y").get<float>(), Position.at("Z").get<float>() };

			const JSON& Scale = json.at("LocalScale");
			component.LocalScale = { Scale.at("X").get<float>(), Scale.at("Y").get<float>() };

			json.at("LocalOrientation").get_to(component.LocalOrientation);

			component.MarkAsDirty();
		}
	};

	struct YSortComponent : public BaseComponent
	{
	public:
		YSortComponent() = default;
		YSortComponent(const YSortComponent& component) = default;


	};

	struct RigidBodyComponent : public BaseComponent
	{
	public:
		RigidBodyComponent() = default;
		RigidBodyComponent(const RigidBodyComponent& component) = default;

		Vector3 Velocity = { 0.f, 0.f, 0.f };
		
		float GroundFriction = 100.f;
		float AirFriction = 30.f;

	public: // json serialisation

		inline friend void to_json(JSON& json, const RigidBodyComponent& component)
		{
			json = JSON{
				{"Velocity", JSONUtil::Vec3ToJSON(component.Velocity) },
				{"GroundFriction", component.GroundFriction},
				{"AirFriction", component.AirFriction}
			};
		}

		inline friend void from_json(const JSON& json, RigidBodyComponent& component)
		{
			component.Velocity = JSONUtil::JSONToVec3(json.at("Velocity"));

			json.at("GroundFriction").get_to(component.GroundFriction);
			json.at("AirFriction").get_to(component.AirFriction);
		}
	};

	struct BoxColliderComponent : public BaseComponent
	{
	public:
		BoxColliderComponent() = default;
		BoxColliderComponent(const BoxColliderComponent& component) = default;

		Vector2 Bounds = { 100.f, 100.f };

	};

	////////////////////
	// Sprite-related //
	////////////////////

	struct SpriteComponent : public BaseComponent
	{
	public:
		SpriteComponent() = default;
		SpriteComponent(const SpriteComponent& component) = default;

		bool Visible = true;
		Colour SpriteColour{ 1.f, 1.f, 1.f, 1.f };

		inline void SetSprite(Ptr<Sprite> sprite) { aSprite = sprite; };
		inline Ptr<Sprite> GetSprite() const { return aSprite; };
		
	private:
		Ptr<Sprite> aSprite = nullptr;

	public: // json serialisation

		inline friend void to_json(JSON& json, const SpriteComponent& component)
		{
			const auto& c = component.SpriteColour;

			json = JSON{
				{"SpriteColour", { {"R", c.r}, {"G", c.g}, {"B", c.b}, {"A", c.a} }}, // warning: if you get an error related to json it's probably this
				{"SpriteName", component.aSprite ? component.aSprite->GetName() : ""}
			};
		}

		inline friend void from_json(const JSON& json, SpriteComponent& component)
		{
			const auto& Colour = json.at("SpriteColour");
			component.SpriteColour = { Colour.at("R").get<float>(), Colour.at("G").get<float>(), Colour.at("B").get<float>(), Colour.at("A").get<float>() };
			
			std::string Name = json.at("SpriteName").get<std::string>();
			
			if (SpriteAtlas::Has(Name))
			{
				Ptr<Sprite> Sprite = SpriteAtlas::Get(Name);
				component.SetSprite(Sprite);
			}
		}
	};

	struct SpriteAnimatorComponent : public BaseComponent
	{
	public:
		SpriteAnimatorComponent() = default;
		
		void Update(float Delta)
		{
			if (!CurrentAnimation)
				return;

			FrameTime += Delta;

			float AnimFrameTime = CurrentAnimation->GetFrameTime();
			if (FrameTime >= AnimFrameTime)
			{
				FrameTime -= AnimFrameTime;
				Frame++;
			}

			if (!Paused && Frame >= CurrentAnimation->GetLength())
			{
				Frame = 0;

				if (!CurrentAnimation->Looped)
					PlayDefault();
			}

			auto& c_Sprite = LinkedEntity->GetComponent<SpriteComponent>();
			c_Sprite.SetSprite(CurrentAnimation->GetFrameSprite(Frame));
		}

		void PlayDefault() { Play(m_AnimationSet->Default); };
		void Play(const std::string& Name)
		{
			FrameTime = 0;
			Frame = 0;

			CurrentAnimation = m_AnimationSet->Sequences[Name];
		}

		bool IsPlaying(const std::string& Name) const { return CurrentAnimation && CurrentAnimation->Name == Name; };

		void SetAnimationSet(Ptr<SpriteAnimationSet> AnimSet) { m_AnimationSet = AnimSet; PlayDefault(); };

		inline Ptr<SpriteAnimationSet> GetAnimationSet() const { return m_AnimationSet; };
		inline Ptr<SpriteAnimationSequence> GetCurrentAnimation() { return CurrentAnimation; };

		uint Frame = 0;
		bool Paused = false;

	private:
		Ptr<SpriteAnimationSet> m_AnimationSet = nullptr;
		Ptr<SpriteAnimationSequence> CurrentAnimation = nullptr;

		float FrameTime = 0;

	public: // json serialisation

		inline friend void to_json(JSON& json, const SpriteAnimatorComponent& component)
		{
			json = JSON{
				{"AnimationSetName", component.m_AnimationSet->Name},
				{"CurrentAnimation", component.CurrentAnimation->Name}
			};
		}

		inline friend void from_json(const JSON& json, SpriteAnimatorComponent& component)
		{
			const JSON& Name = json.at("AnimationSetName");
			
			if (Name.is_string())
				component.SetAnimationSet(AnimationAtlas::Get(Name.get<std::string>()));

			const JSON& AnimName = json.at("CurrentAnimation");

			if (AnimName.is_string())
				component.Play(AnimName);
		}
	};

	//////////////////////
	// Viewport related //
	//////////////////////



	struct CameraComponent : public BaseComponent
	{
	public:
		CameraComponent() = default;
		CameraComponent(const CameraComponent& component) = default;

		bool AutoViewportResizeToWindow = true;

		void SetOrthoSize(Vector2 Res)
		{
			m_OrthoSize = Res;
			RecalculateProjection();
		}

		void SetOrthoZPlane(float Near, float Far)
		{
			m_OrthoZPlane = { Near, Far };
			RecalculateProjection();
		}

		void SetViewportPosition(Vector2 Position)
		{
			m_ViewportPosition = Position;
		}
		
		void SetViewportSize(Vector2 Size)
		{
			m_ViewportSize = Size;

			if (m_FramebufferMode && m_FrameBuffer)
			{
				m_FrameBuffer->Resize(m_ViewportSize);
			}
		}

		void SetFramebufferEnabled(bool Mode)
		{
			if (Mode)
			{
				FrameBufferSpecification FSpec;
				FSpec.Attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::Depth };
				FSpec.Size = m_ViewportSize;

				m_FrameBuffer = CreatePtr<FrameBuffer>(FSpec);
			}
			else
			{
				m_FrameBuffer = nullptr;
			}

			m_FramebufferMode = Mode;
		}

		Vector3 ScreenToWorldCoordinates(Vector3 ScreenCoords)
		{
			auto& Transform = LinkedEntity->GetComponent<TransformComponent>();

			auto o = Vector3(m_OrthoSize, 0);
			auto v = Vector3(m_ViewportSize, 0);

			return (Transform.GetGlobalPosition() - (o / 2.f)) + ScreenCoords * (o / v);
		}

		Mat4x4 GetTransform(const Vector3& Position) const
		{
			return glm::translate(Mat4x4(1.f), Position - (Vector3(m_OrthoSize, 0.f) / 2.f));
		}

		Ptr<FrameBuffer> GetFrameBuffer() { return m_FrameBuffer; };

		inline Viewport GetViewport() const { return { m_ViewportPosition, m_ViewportSize }; };
		inline Mat4x4 GetProjection() const { return Projection; };
		inline ZPlane GetOrthoZPlane() const{ return m_OrthoZPlane; }; // First is Near, second is Far
		
		inline Vector2 GetOrthoSize() const { return m_OrthoSize; };
		inline bool IsFramebufferMode() const { return m_FramebufferMode; };

	private:

		void RecalculateProjection()
		{
			Projection = glm::ortho(0.f, m_OrthoSize.x, m_OrthoSize.y, 0.f, m_OrthoZPlane.Near, m_OrthoZPlane.Far);
		}

		Ptr<FrameBuffer> m_FrameBuffer = nullptr;
		bool m_FramebufferMode = false;

		Vector2 m_ViewportPosition = { 0.f, 0.f };
		Vector2 m_ViewportSize = { 1280.f, 720.f };

		Vector2 m_OrthoSize = { 1280.f, 720.f };
		ZPlane m_OrthoZPlane = {};

		Mat4x4 Projection = glm::ortho(0.f, 1280.f, 720.f, 0.f, -100.f, 100.f);

		friend class Scene;

	public: // json serialisation

		inline friend void to_json(JSON& json, const CameraComponent& component)
		{
			json = JSON{
				{"ViewportPosition", JSONUtil::Vec2ToJSON(component.m_ViewportPosition) },
				{"ViewportSize", JSONUtil::Vec2ToJSON(component.m_ViewportSize) },
				{"OrthoSize", JSONUtil::Vec2ToJSON(component.m_OrthoSize) },
				{"OrthoZPlane", {
					{"Near", component.m_OrthoZPlane.Near},
					{"Far", component.m_OrthoZPlane.Far}
				}}
			};
		}

		inline friend void from_json(const JSON& json, CameraComponent& component)
		{
			Vector2 CameraResolution = JSONUtil::JSONToVec2(json.at("CameraResolution"));

			const JSON& zPlane = json.at("zPlane");
			float j_Near = zPlane.at("Near").get<float>();
			float j_Far = zPlane.at("Far").get<float>();

			component.SetOrthoSize(CameraResolution);
			component.SetOrthoZPlane(j_Near, j_Far);

			Vector2 ViewportPos = JSONUtil::JSONToVec2(json.at("ViewportPosition"));
			Vector2 ViewportSize = JSONUtil::JSONToVec2(json.at("ViewportSize"));

			component.SetViewportPosition(ViewportPos);
			component.SetViewportSize(ViewportSize);
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

		void Bind(std::string name)
		{
			if (!LinkedEntity)
				return;

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
			std::string name = json.at("ScriptName").get<std::string>();
			component.Name = name;
		}
	};
}