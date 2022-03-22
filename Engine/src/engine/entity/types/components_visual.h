#pragma once

#include <engineincl.h>
#include <engine/sprite/sprite_atlas.h>
#include <engine/sprite/animation/animation_atlas.h>
#include <render/renderer.h>

#include "components_base.h"
#include "components_phys.h"


namespace Techless
{
	struct YSortComponent : public BaseComponent
	{
	public:
		YSortComponent() = default;
		YSortComponent(const YSortComponent& component) = default;

	public:
		inline friend void to_json(JSON& json, const YSortComponent& component)
		{
			json = JSON{};
		}

		inline friend void from_json(const JSON& json, YSortComponent& component)
		{

		}

	};

	struct SpriteComponent : public BaseComponent
	{
	public:
		SpriteComponent() = default;
		SpriteComponent(const SpriteComponent& component) = default;

		bool Visible = true;
		Colour SpriteColour = { 1.f, 1.f, 1.f, 1.f };

		void SetSprite(Ptr<Sprite> sprite) { m_Sprite = sprite; };
		Ptr<Sprite> GetSprite() { return m_Sprite; };

//		void SetShader(Ptr<Shader> shader) { m_Shader = shader; };
//		Ptr<Shader> GetShader(Ptr<Shader> shader) { return m_Shader; };

	private:
		Ptr<Sprite> m_Sprite = nullptr;
//		Ptr<Shader> m_Shader = nullptr;

	public: // json serialisation

		inline friend void to_json(JSON& json, const SpriteComponent& component)
		{
			json = JSON{
				{"SpriteColour", JSONUtil::ColourToJSON(component.SpriteColour)}, // warning: if you get an error related to json it's probably this
				{"SpriteName", component.m_Sprite ? component.m_Sprite->GetName() : ""}
			};
		}

		inline friend void from_json(const JSON& json, SpriteComponent& component)
		{
			component.SpriteColour = JSONUtil::JSONToColour(json.at("SpriteColour"));

			Ptr<Sprite> Sprite = SpriteAtlas::Get(json.at("SpriteName").get<std::string>());
			component.SetSprite(Sprite);
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

		Vector3 ScreenToViewportCoordinates(Vector3 ScreenCoords)
		{
			return ScreenCoords - Vector3(m_ViewportPosition, 0.f);
		}

		Vector3 ViewportToWorldCoordinates(Vector3 ViewportCoords)
		{
			auto& Transform = LinkedEntity->GetComponent<TransformComponent>();

			auto o = Vector3(m_OrthoSize, 0);
			auto v = Vector3(m_ViewportSize, 0);

			return (Transform.GetGlobalPosition() - (o / 2.f)) + ViewportCoords * (o / v);
		}

		Mat4x4 GetTransform(const Vector3& Position) const
		{
			return glm::translate(Mat4x4(1.f), Position - (Vector3(m_OrthoSize, 0.f) / 2.f));
		}

		Ptr<FrameBuffer> GetFrameBuffer() { return m_FrameBuffer; };

		inline Viewport GetViewport() const { return { m_ViewportPosition, m_ViewportSize }; };
		inline Mat4x4 GetProjection() const { return Projection; };
		inline ZPlane GetOrthoZPlane() const { return m_OrthoZPlane; }; // First is Near, second is Far

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
}