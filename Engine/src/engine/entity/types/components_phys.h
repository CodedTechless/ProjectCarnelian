#pragma once

#include <engineincl.h>
#include <engine/application/application.h>

#include "components_base.h"

namespace Techless
{
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

			TransformRecalculateResult Result{ GetGlobalTransformState() };
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
		Vector3 GlobalPosition{ 0.f, 0.f, 0.f };
		Vector2 GlobalScale{ 1.f, 1.f };
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

	/*
	struct RigidBodyComponent : public BaseComponent
	{
	public:
		RigidBodyComponent() = default;
		RigidBodyComponent(const RigidBodyComponent& component) = default;
		
		Vector3 Velocity = { 0.f, 0.f, 0.f };
		float MaxVelocity = 125.f;

		float BodyFriction = 1800.f;			// deceleration rate per second
		float FrictionOverSpeedEffect = 3.f;	// extra slowdown if the speed exceeds max speed

		void Step(float Delta)
		{
			float Angle = std::atan2(Velocity.y, Velocity.x);
			float IAngle = Angle + M_PI;

			Vector3 Friction = { std::cos(IAngle) * BodyFriction, std::sin(IAngle) * BodyFriction, 0.f };

			Velocity -= Friction * Delta * (glm::length(Velocity) ? FrictionOverSpeedEffect : 1);
			
			Velocity.x = std::max()

		}

	public: // json serialisation

		inline friend void to_json(JSON& json, const RigidBodyComponent& component)
		{
			json = JSON{
				{"Velocity", JSONUtil::Vec3ToJSON(component.Velocity) },
				{"MaxVelocity", component.MaxVelocity},
				{"BodyFriction", component.BodyFriction},
				{"FrictionOverSpeedEffect", component.FrictionOverSpeedEffect}
			};
		}

		inline friend void from_json(const JSON& json, RigidBodyComponent& component)
		{
			component.Velocity = JSONUtil::JSONToVec3(json.at("Velocity"));

			json.at("MaxVelocity").get_to(component.MaxVelocity);
			json.at("BodyFriction").get_to(component.BodyFriction);
			json.at("FrictionOverSpeedEffect").get_to(component.FrictionOverSpeedEffect);
		}
	};
	*/
	struct BoxColliderComponent : public BaseComponent
	{
	public:
		BoxColliderComponent() = default;
		BoxColliderComponent(const BoxColliderComponent& component) = default;

		Vector2 Bounds = { 100.f, 100.f };

	};
}