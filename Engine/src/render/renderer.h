#pragma once

#include <engineincl.h>

#include "buffer/index.h"
#include "buffer/vertex.h"
#include "array/vertex_array.h"
#include "shader/shader.h"
#include "texture/texture.h"

#include "engine/sprite/sprite_atlas.h"

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
		static void Stop();

		static void DrawSprite(const std::shared_ptr<Sprite> sprite, const glm::vec2& Position, const glm::vec2& Size = glm::vec2(1.f, 1.f), float Depth = 0, float Angle = 0, const glm::vec3& Colour = glm::vec3(1.f, 1.f, 1.f), float Alpha = 1.f);

		static void DrawBlankQuad(const glm::vec3& Position, const glm::vec2& Size = { 1.f, 1.f }, const glm::vec4& Colour = { 1.f, 1.f, 1.f, 1.f }, float Angle = 0);
		static void DrawQuad(const std::shared_ptr<Texture> Tex, const glm::vec2 TexCoords[4], const glm::vec3& Position, const glm::vec2& Size = glm::vec2(1.f, 1.f), const glm::vec4& Colour = glm::vec4(1.f, 1.f, 1.f, 1.f), float Angle = 0);
		static void DrawQuad(const std::shared_ptr<Texture> Tex, const glm::vec2 TexCoords[4], const glm::mat4& Transform, const glm::vec4& Colour);

		static void DrawQuadArray(const std::shared_ptr<Texture> Tex, const glm::vec2 TexCoords[4], Quad* QuadArray, unsigned int Count);

	public:
		static void Begin(glm::mat4 Projection, glm::mat4 Transform);
		static void End();

		static void Flush();
		static void ForceReset();

		static void SetClearColour(glm::vec4 Colour);
		static void Clear();

		static void RenderImGuiElements();

		static RendererDebug& GetDebugInfo() { return DebugInfo; };
		static std::shared_ptr<Texture> GetDefaultTexture() { return DefaultTexture; };
		static int GetMaxTextureSize() { return MaxTextureSize; };

	private:
		static std::shared_ptr<Texture> DefaultTexture;

		static RendererDebug DebugInfo;

	private:
		static int MaxTextureSize;
	};
}