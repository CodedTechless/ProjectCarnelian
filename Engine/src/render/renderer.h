#pragma once

#include <engineincl.h>

#include <engine/maths/colour.hpp>
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
		glm::mat4 Transform;
		glm::vec4 Colour;
	};

	class Renderer {
	public:
		static void Init();
		static void InitOpenGL();

		static void DrawQuad(const glm::vec3& Position, const glm::vec2& Scale, float Orientation = 1.f, const Colour& colour = {});
		static void DrawSprite(Ptr<Sprite> sprite, const glm::mat4& Transform, const Colour& colour = {});
		static void DrawTexturedQuad(Ptr<Texture> Tex, const glm::vec2 TexCoords[4], const glm::mat4& Transform, const glm::vec4& Colour = {});

		static void DrawQuadArray(Ptr<Texture> Tex, const glm::vec2 TexCoords[4], Quad* QuadArray, unsigned int Count);

	public:
		static void Begin(glm::mat4 Projection, glm::mat4 Transform, Ptr<Shader> shader = nullptr, Ptr<FrameBuffer> frameBuffer = nullptr);
		static void End();

		static void BeginBatch();
		static void ResetBatch();

		static void Flush();

		static void SetClearColour(glm::vec4 Colour);
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