#pragma once

#include <engineincl.h>

#include <engine/sprite/sprite_atlas.h>

#include "buffer/index.h"
#include "buffer/vertex.h"
#include "buffer/frame.h"
#include "array/vertex_array.h"
#include "shader/shader.h"
#include "texture/texture.h"


namespace Techless {

	struct RendererDebug
	{
		unsigned int DrawCalls = 0;
		unsigned int UsedTextureSlots = 0;
		unsigned int VertexCount = 0;
	};

	struct Quad
	{
		Mat4x4 Transform;
		Colour m_Colour;
	};

	class Renderer {
	public:
		static void Init();
		static void InitOpenGL();

		static void DrawQuad(const Vector3& Position, const Vector2& Scale, float Orientation = 0.f, const Colour& colour = { 1.f, 1.f, 1.f, 1.f });
		static void DrawSprite(Ptr<Sprite> sprite, const Vector3& Transform, const Vector2& Scale = { 1.f , 1.f }, float Orientation = 0.f, const Colour& colour = { 1.f, 1.f, 1.f, 1.f });
		static void DrawSpriteExt(Ptr<Sprite> sprite, const Mat4x4& Transform, const Colour& colour = { 1.f, 1.f, 1.f, 1.f });
		static void DrawTexturedQuad(Ptr<Texture> Tex, const Vector2 TexCoords[4], const Mat4x4& Transform, const Colour& colour = { 1.f, 1.f, 1.f, 1.f });

		static void DrawQuadArray(Ptr<Texture> Tex, const Vector2 TexCoords[4], Quad* QuadArray, unsigned int Count);

	public:
		static void Begin(Mat4x4 Projection, Mat4x4 Transform, Ptr<Shader> shader = nullptr);
		static void End();

		static void BeginBatch();
		static void ResetBatch();

		static void Flush();

	public:
		static void SetViewport(Viewport viewport);
		static void SetClearColour(Colour colour);	

		void SetShader(Ptr<Shader> shader);
		void ResetShader();

		static void ResetClearColour();
		static void Clear();

	public:
		static void ShowRuntimeStatsWindow();

		static RendererDebug& GetDebugInfo() { return DebugInfo; };
		static int GetMaxTextureSize() { return MaxTextureSize; };

	private:
		static std::shared_ptr<Texture> DefaultTexture;
		static RendererDebug DebugInfo;
		static int MaxTextureSize;
	};
}