#pragma once

#include <engineincl.h>
#include <engine/application/application.h>

#include "components_base.h"

namespace Techless
{
	struct TransformState
	{
		Vector3 Position = { 0.f, 0.f, 0.f };
		Vector2 Scale = { 1.f, 1.f };
		float Orientation = 0.f;

		glm::mat4 Transform { 1.f };

		inline bool operator==(const TransformState& t) { return t.Position == Position && t.Scale == Scale && t.Orientation == Orientation; };
		inline bool operator!=(const TransformState& t) { return t.Position != Position || t.Scale != Scale || t.Orientation != Orientation; };
	};

	struct TransformComponent : public BaseComponent
	{
	public:
		TransformComponent() = default;
		TransformComponent(const TransformComponent& component) = default;

		inline Vector3 GetLocalPosition() const { return Position; };
		inline Vector2 GetLocalScale() const { return Scale; };
		inline float GetLocalOrientation() const { return Orientation; };

		Vector3 GetGlobalPosition() { RecalculateBase(); if (FLAG_DoInterpolation) return InterpState.Position; else return CurrentState.Position; };
		Vector2 GetGlobalScale() { RecalculateBase(); if (FLAG_DoInterpolation) return InterpState.Scale; else return CurrentState.Scale; };
		float GetGlobalOrientation() { RecalculateBase(); if (FLAG_DoInterpolation) return InterpState.Orientation; else return CurrentState.Orientation; };
		Mat4x4 GetGlobalTransform() { RecalculateBase(); if (FLAG_DoInterpolation) return InterpState.Transform; else return CurrentState.Transform; };

		void SetLocalPosition(Vector3 position) { if (position == Position) { return; }; Position = position; MarkTransformationDirty(); };
		void SetLocalScale(Vector2 scale) { if (scale == Scale) { return; } Scale = scale; MarkTransformationDirty(); };
		void SetLocalOrientation(float orientation) { if (orientation == Orientation) { return; } Orientation = orientation; MarkTransformationDirty(); };

		TransformState GetLocalTransformState()
		{
			return { Position, Scale, Orientation};
		}

		TransformState GetPreviousState() { return PreviousState; }
		TransformState GetCurrentState() { return CurrentState; }

		void SetEngineInterpolation(bool Mode)
		{
			if (FLAG_DoInterpolation == Mode)
				return;

			FLAG_DoInterpolation = Mode;

			for (Ptr<Entity> LinkedChild : LinkedEntity->GetChildren())
			{
				LinkedChild->GetComponent<TransformComponent>().SetEngineInterpolation(Mode);
			}
		};

		inline bool IsEngineInterpolationEnabled() const { return FLAG_DoInterpolation; };

		void ForceInterpolationUpdate()
		{
			if (FLAG_DoInterpolation)
				PreviousState = CurrentState;
		}

		void MarkInterpolationDirty()
		{
			if (!FLAG_DoInterpolation || FLAG_InterpolationDirty)
				return;

			FLAG_InterpolationDirty = true;

			for (Ptr<Entity> LinkedChild : LinkedEntity->GetChildren())
			{
				LinkedChild->GetComponent<TransformComponent>().MarkInterpolationDirty();
			}
		}

	private:

		void RecalculateBase()
		{
			/*
				to-do: optimise this system [UPDATE 31/03/22: optimised a lot more but needs review!]
			*/

			if (FLAG_TransformDirty)
			{
				Ptr<Entity> Parent = LinkedEntity->GetParent();

				if (Parent)
				{
					TransformComponent& ParentTransform = Parent->GetComponent<TransformComponent>();
					ParentTransform.RecalculateBase();

					BuildTransformState(CurrentState, GetLocalTransformState(), ParentTransform.CurrentState);
				}
				else
				{
					CurrentState = GetLocalTransformState();
				}

				BuildTransform(CurrentState);

				FLAG_TransformDirty = false;
			}

			if (FLAG_DoInterpolation && FLAG_InterpolationDirty && CurrentState != PreviousState)
			{
				Ptr<Entity> Parent = LinkedEntity->GetParent();

				if (Parent)
				{
					TransformComponent& ParentTransform = Parent->GetComponent<TransformComponent>();

					// if the parent transform is an interpolation transform, build my interpolation state based of its interpolation state instead of the global state
					if (ParentTransform.FLAG_DoInterpolation)
					{
						if (ParentTransform.FLAG_TransformDirty || ParentTransform.FLAG_InterpolationDirty)
							ParentTransform.RecalculateBase();

						BuildTransformState(InterpState, GetLocalTransformState(), ParentTransform.InterpState);
					}
					// if the parent transform is not interpolated (or there is no parent), then use my previous state and current global state to build an interpolated value
					else
					{
						Interpolate(InterpState, CurrentState, PreviousState);
					}
				}
				else
				{
					Interpolate(InterpState, CurrentState, PreviousState);
				}

				BuildTransform(InterpState);

				FLAG_InterpolationDirty = false;
			}
		}

		// builds a state where LocalState is relative to ParentState's coordinate space. does not build the state for you.
		static void BuildTransformState(TransformState& State, const TransformState& LocalState, const TransformState& ParentState)
		{
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
		static void Interpolate(TransformState& State, const TransformState& Next, const TransformState& Last)
		{
			float Ratio = Application::GetActiveApplication().GetSimulationRatio();

			State = Last;
			if (Last.Position != Next.Position)
			{
				State.Position = glm::mix(Last.Position, Next.Position, Ratio);
			}
			else
			{
				State.Position = Last.Position;
			}

			if (Last.Scale != Next.Scale)
			{
				State.Scale = glm::mix(Last.Scale, Next.Scale, Ratio);
			}
			else
			{
				State.Scale = Last.Scale;
			}

			if (Last.Orientation != Next.Orientation)
			{
				State.Orientation = Last.Orientation + Ratio * std::fmodf(Next.Orientation - Last.Orientation, 2.f * M_PI);
			}
			else
			{
				State.Orientation = Last.Orientation;
			}
		}


		// using a dirty flag to signify when a global position/scale/orientation does not match up with its parents position or with its own local position.
		void MarkTransformationDirty()
		{
			if (FLAG_TransformDirty)
				return;

			FLAG_TransformDirty = true;

			for (Ptr<Entity> LinkedChild : LinkedEntity->GetChildren())
			{
				LinkedChild->GetComponent<TransformComponent>().MarkTransformationDirty();
			}
		}



	private:

		Vector3 Position = { 0.f, 0.f, 0.f };	// Local position
		Vector2 Scale = { 1.f, 1.f };			// Local scale
		float Orientation = 0.f;				// Local orientation

		TransformState CurrentState = {};		// Current GLOBAL state of the transformation (in world coordinates)
		TransformState PreviousState = {};		// Previous GLOBAL state of the transformation, on the previous simulation tick
		TransformState InterpState = {};		// Current interpolated state between PreviousState and CurrentState

		bool FLAG_TransformDirty = true;		// Signifies whether or not the transformation is dirty. If FLAG_DoInterpolation is TRUE, this isn't used and is instead replaced by Last == Next
		
		bool FLAG_DoInterpolation = false;		// Signifies whether or not to use state interplotation.
		bool FLAG_InterpolationDirty = false;	// Signifies whether or not the cached interpolation transformation is dirty.

		friend class Entity;
		friend class Scene;

	public: // json serialisation

		inline friend void to_json(JSON& json, const TransformComponent& component)
		{
			json = JSON{
				{"LocalPosition", JSONUtil::Vec3ToJSON(component.Position) },
				{"LocalScale", JSONUtil::Vec2ToJSON(component.Scale) },
				{"LocalOrientation", component.Orientation }
			};
		}

		inline friend void from_json(const JSON& json, TransformComponent& component)
		{
			component.Position = JSONUtil::JSONToVec3(json.at("LocalPosition"));
			component.Scale = JSONUtil::JSONToVec2(json.at("LocalScale"));
			json.at("LocalOrientation").get_to(component.Orientation);

			component.MarkTransformationDirty();
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

	struct Rectangle
	{
		Vector2 Position;
		Vector2 Size;
	};

	struct BoxColliderComponent : public BaseComponent
	{
	public:
		BoxColliderComponent() = default;
		BoxColliderComponent(const BoxColliderComponent& component) = default;

		Vector2 Bounds = { 100.f, 100.f };

		bool CollideSimple(Entity& B)
		{
			auto& TransformA = LinkedEntity->GetComponent<TransformComponent>();
			
			auto& BoxColliderB = B.GetComponent<BoxColliderComponent>();
			auto& TransformB = B.GetComponent<TransformComponent>();

			Rectangle RectA = {
				TransformA.GetGlobalPosition(),
				Bounds * TransformA.GetGlobalScale()
			};

			Rectangle RectB = {
				TransformB.GetGlobalPosition(),
				Bounds * TransformB.GetGlobalScale()
			};

			return (
				RectA.Position.x < RectB.Position.x + RectB.Size.x &&
				RectA.Position.y < RectB.Position.y + RectB.Size.y &&
				RectA.Position.x + RectA.Size.x > RectB.Position.x &&
				RectA.Size.x + RectA.Position.y > RectB.Position.y
			);
		}

	public:
		inline friend void to_json(JSON& json, const BoxColliderComponent& component)
		{
			json = JSON{
				{"Bounds", JSONUtil::Vec2ToJSON(component.Bounds) }
			};
		}

		inline friend void from_json(const JSON& json, BoxColliderComponent& component)
		{
			component.Bounds = JSONUtil::JSONToVec2(json.at("Bounds"));
		}

	};
}