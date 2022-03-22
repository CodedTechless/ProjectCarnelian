

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui/imgui.h>

#include <engine/application/application.h>
#include <engine/application/watchdog/watchdog.h>

#include "renderer.h"
// {0.f, 0.f}, { 1.f, 0.f }, { 1.f, 1.f }, { 0.f, 1.f }

namespace Techless 
{

	struct QuadFormat 
	{
		Vector3 Position;
		Vector4 Colour;
		Vector2 TexCoords;
		float TexIndex;
	};

	struct RendererDataSet 
	{
		static const uint32_t MaxQuads = 20000;
		static const uint32_t MaxVerts = MaxQuads * 4;
		static const uint32_t MaxInds = MaxQuads * 6;
		
		QuadFormat* QuadArray = nullptr;
		QuadFormat* QuadArrayPointer = nullptr;

		uint CurrentVertexIndex = 0;

		Ptr<VertexArray> QuadVertexArray;

		Ptr<VertexBuffer> BatchVertexBuffer;
		Ptr<IndexBuffer> BatchIndBuffer;

		Ptr<Shader> DefaultTextureShader;

		Ptr<Shader> ActiveShader;
		Ptr<FrameBuffer> ActiveFrameBuffer;

		uint NextTextureSlot = 1;
		static const int ActiveTextureSlots = 16;
		std::array<Ptr<Texture>, 16> ActiveTextures;

		Vector4 VertexDefault[4] = {};
		Colour ClearColour = { 0.1f, 0.1f, 0.1f, 1.f };
	};

	static RendererDataSet RendererData;
	RendererDebug Renderer::DebugInfo = {};

	int Renderer::MaxTextureSize = 1024;
	Ptr<Texture> Renderer::DefaultTexture = nullptr;

	void OpenGLDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		Debug::OpenGLMessage(std::string(message),source, type, id, severity);
	}

	void Renderer::InitOpenGL()
	{

		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLDebugMessage, nullptr);

		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);

		// Enable depth testing (for Z-axis based depth) and blending (for alpha support)
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST); //we don't need depth for a 2d game ya doofus!

		GLint glMaxTextureSize;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glMaxTextureSize);
		MaxTextureSize = std::min((int)glMaxTextureSize, 2048);

//		Debug::Log("MaxTextureSize: " + std::to_string(MaxTextureSize) + " (" + std::to_string(glMaxTextureSize) + ")", "Renderer");
	}

	void Renderer::Init()
	{
		Debug::Log("Initialising Renderer (Profile: OpenGL)", "Renderer");
		InitOpenGL();

		// Create the vertex array which will hold attribute information for our batch vertex buffer and index buffer.
		RendererData.QuadVertexArray = std::make_shared<VertexArray>();
		RendererData.QuadVertexArray->Bind();

		// Create a new array of quads to be pushed into a vertex buffer. Then, new verticies can be pushed in real time.
		RendererData.QuadArray = new QuadFormat[RendererDataSet::MaxVerts];
		
		// Set up an empty unsigned int array to hold our quad indicies in the correct format.
		auto IndexBufferTemplate = new uint[RendererDataSet::MaxInds];

		uint QuadOffset = 0;
		for (uint i = 0; i < RendererDataSet::MaxInds; i += 6) {
			IndexBufferTemplate[i + 0] = 0 + QuadOffset;
			IndexBufferTemplate[i + 1] = 1 + QuadOffset;
			IndexBufferTemplate[i + 2] = 2 + QuadOffset;

			IndexBufferTemplate[i + 3] = 0 + QuadOffset;
			IndexBufferTemplate[i + 4] = 2 + QuadOffset;
			IndexBufferTemplate[i + 5] = 3 + QuadOffset;
			
			QuadOffset += 4; 
		}
		RendererData.BatchIndBuffer = std::make_shared<IndexBuffer>(IndexBufferTemplate, RendererDataSet::MaxInds);
		delete[] IndexBufferTemplate;
		
		// Create the vertex buffer and set its layout.
		RendererData.BatchVertexBuffer = std::make_shared<VertexBuffer>(RendererDataSet::MaxVerts * sizeof(QuadFormat));
		RendererData.BatchVertexBuffer->SetBufferLayout({ // Create a layout for data that is held in the vertex buffer for drawing.
			{ LayoutType::Float, 3 }, // Position
			{ LayoutType::Float, 4 }, // Colour
			{ LayoutType::Float, 2 }, // Texture coordinates
			{ LayoutType::Float, 1 }  // Texture index
		});

		// Add the vertex and index buffer to the vertex array.
		RendererData.QuadVertexArray->PushVertexBuffer(RendererData.BatchVertexBuffer);
		RendererData.QuadVertexArray->SetIndexBuffer(RendererData.BatchIndBuffer);

		// Initialise the texture shader and import the basic shader.
		RendererData.DefaultTextureShader = std::make_shared<Shader>();
		RendererData.DefaultTextureShader->ImportShader("srcassets/shaders/basic.shader");
		RendererData.DefaultTextureShader->Bind();

		RendererData.ActiveShader = RendererData.DefaultTextureShader;

		// Assign texture slot 1 to the white "default" texture.
		RendererData.ActiveTextures[0] = std::make_shared<Texture>("srcassets/textures/default.png");
		RendererData.ActiveTextures[0]->Bind();
		DefaultTexture = RendererData.ActiveTextures[0];

		// Fill the samplers uniform with the numbers for each active texture slot
		auto Samplers = new int[RendererDataSet::ActiveTextureSlots];
		for (auto i = 0; i < RendererDataSet::ActiveTextureSlots; i++)
		{
			Samplers[i] = i;
		}

		RendererData.DefaultTextureShader->SetUniform1iv("Samplers", Samplers, RendererDataSet::ActiveTextureSlots);

		// Set the default vertex positions (verticies are centred by default.)
		RendererData.VertexDefault[0] = { -0.5f,  0.5f, 0.f, 1.f };
		RendererData.VertexDefault[1] = {  0.5f,  0.5f, 0.f, 1.f };
		RendererData.VertexDefault[2] = {  0.5f, -0.5f, 0.f, 1.f };
		RendererData.VertexDefault[3] = { -0.5f, -0.5f, 0.f, 1.f };

		auto e = glGetString(GL_VERSION);
		Debug::Log("Started renderer. (OpenGL " + std::string((const char*)e) + ")", "Renderer");
	}

	////////////////////
	// Quad Rendering //
	////////////////////

	void Renderer::DrawQuad(const Vector3& Position, const Vector2& Scale, float Orientation, const Colour& colour)
	{
		Mat4x4 Transform = 
			glm::translate(Mat4x4(1.0f), Position)
			* glm::rotate(Mat4x4(1.0f), glm::radians(Orientation), Vector3(0.f, 0.f, 1.f))
			* glm::scale(Mat4x4(1.0f), Vector3(Scale.x, Scale.y, 1.f));

		Vector2 TexCoords[] = { { 0.f, 0.f }, { 1.f, 0.f }, { 1.f, 1.f }, { 0.f, 1.f } };
		DrawTexturedQuad(RendererData.ActiveTextures[0], TexCoords, Transform, colour);
	}

	void Renderer::DrawSprite(Ptr<Sprite> sprite, const Vector3& Position, const Vector2& Scale, float Orientation, const Colour& colour)
	{
		Mat4x4 Transform =
			glm::translate(Mat4x4(1.0f), Position)
			* glm::rotate(Mat4x4(1.0f), glm::radians(Orientation), Vector3(0.f, 0.f, 1.f))
			* glm::scale(Mat4x4(1.0f), Vector3(Scale.x, Scale.y, 1.f));

		DrawSpriteExt(sprite, Transform, colour);
	}

	void Renderer::DrawSpriteExt(Ptr<Sprite> sprite, const Mat4x4& Transform, const Colour& colour)
	{
		auto texture = sprite->GetTexture();
		auto SpriteBounds = sprite->GetAbsoluteBounds();

		Vector2 TopLeft = SpriteBounds.TopLeft;
		Vector2 BottomRight = SpriteBounds.BottomRight;

		Vector2 TexCoords[4] = { { TopLeft.x, TopLeft.y }, { BottomRight.x, TopLeft.y }, { BottomRight.x, BottomRight.y }, {TopLeft.x, BottomRight.y} };

		DrawTexturedQuad(texture, TexCoords, Transform * glm::scale(Mat4x4(1.f), Vector3(sprite->GetSize(), 1.f)), colour);
	}

	// Draw a quad using matricies
	void Renderer::DrawTexturedQuad(Ptr<Texture> Tex, const Vector2 TexCoords[4], const Mat4x4& Transform, const Colour& Colour)
	{
		if (RendererData.CurrentVertexIndex >= RendererDataSet::MaxInds)
		{
			ResetBatch();
		}

		float TexIndex = 0.f;
		for (auto i = 0; i < RendererDataSet::ActiveTextureSlots; i++)
		{
			if (RendererData.ActiveTextures[i] == Tex)
			{
				TexIndex = i;
			}
		}

		if (TexIndex == 0.f)
		{
			if (RendererData.NextTextureSlot > RendererDataSet::ActiveTextureSlots)
			{
				ResetBatch();
			}

			RendererData.ActiveTextures[RendererData.NextTextureSlot] = Tex;
			RendererData.NextTextureSlot++;
		}

		for (size_t i = 0; i < 4; i++)
		{
			RendererData.QuadArrayPointer->Position = Transform * RendererData.VertexDefault[i];
			RendererData.QuadArrayPointer->Colour = Colour;
			RendererData.QuadArrayPointer->TexCoords = TexCoords[i];
			RendererData.QuadArrayPointer->TexIndex = TexIndex;

			//std::cout << "X: " << RendererData.QuadArrayPointer->Position.x << " Y: " << RendererData.QuadArrayPointer->Position.y << " Z: " << RendererData.QuadArrayPointer->Position.z << " TexCoords: " << TexCoords[i].x << " " << TexCoords[i].y << std::endl;

			RendererData.QuadArrayPointer++;
			DebugInfo.VertexCount++;
		}

		RendererData.CurrentVertexIndex += 6;
	}

	void Renderer::DrawQuadArray(Ptr<Texture> Tex, const Vector2 TexCoords[4], Quad* QuadArray, unsigned int Count)
	{
		if (RendererData.CurrentVertexIndex >= RendererDataSet::MaxInds)
		{
			ResetBatch();
		}

		float TexIndex = 0.f;
		for (auto i = 0; i < RendererDataSet::ActiveTextureSlots; i++)
		{
			if (RendererData.ActiveTextures[i] == Tex)
			{
				TexIndex = i;
			}
		}

		if (TexIndex == 0.f)
		{
			if (RendererData.NextTextureSlot > RendererDataSet::ActiveTextureSlots)
			{
				ResetBatch();
			}

			RendererData.ActiveTextures[RendererData.NextTextureSlot] = Tex;
			RendererData.NextTextureSlot++;
		}

		for (size_t iQ = 0; iQ < Count; iQ++)
		{
			auto& NextQuad = QuadArray[iQ];

			for (size_t i = 0; i < 4; i++)
			{
				RendererData.QuadArrayPointer->Position = NextQuad.Transform * RendererData.VertexDefault[i];
				RendererData.QuadArrayPointer->Colour = NextQuad.m_Colour;
				RendererData.QuadArrayPointer->TexCoords = TexCoords[i];
				RendererData.QuadArrayPointer->TexIndex = TexIndex;

				//std::cout << "X: " << RendererData.QuadArrayPointer->Position.x << " Y: " << RendererData.QuadArrayPointer->Position.y << " Z: " << RendererData.QuadArrayPointer->Position.z << " TexCoords: " << TexCoords[i].x << " " << TexCoords[i].y << std::endl;

				RendererData.QuadArrayPointer++;
				DebugInfo.VertexCount++;
			}

			RendererData.CurrentVertexIndex += 6;
		}

		delete[] QuadArray;
	}


	///////////
	// Scene //
	///////////
	void Renderer::Begin(Mat4x4 Projection, Mat4x4 Transform, Ptr<Shader> shader)
	{	
		Mat4x4 Proj = Projection * glm::inverse(Transform);

		RendererData.ActiveShader->SetUniformMat4f("CameraProjection", Proj);

		DebugInfo.DrawCalls = 0;
		DebugInfo.VertexCount = 0;

		BeginBatch();
	}

	void Renderer::End()
	{
		Flush();
	}

	void Renderer::BeginBatch()
	{
		RendererData.NextTextureSlot = 1;
		RendererData.CurrentVertexIndex = 0;
		RendererData.QuadArrayPointer = RendererData.QuadArray;
	}

	void Renderer::ResetBatch()
	{
		Flush();
		BeginBatch();
	}

	void Renderer::Flush()
	{
		if (RendererData.CurrentVertexIndex == 0)
			return;

		{
			auto Size = (uint32_t)((uint8_t*)RendererData.QuadArrayPointer - (uint8_t*)RendererData.QuadArray);
			RendererData.BatchVertexBuffer->Set(RendererData.QuadArray, Size);

			for (unsigned int i = 0; i < RendererData.NextTextureSlot; i++)
			{
				RendererData.ActiveTextures[i]->Bind(i);
			}
		}

		RendererData.ActiveShader->Bind();
		glDrawElements(GL_TRIANGLES, RendererData.CurrentVertexIndex, GL_UNSIGNED_INT, nullptr); // draws the array of buffer data that is currently binded

		DebugInfo.DrawCalls++;
	}

	void Renderer::ResetClearColour()
	{
		auto colour = RendererData.ClearColour;
		glClearColor(colour.r, colour.g, colour.b, colour.a);
	}

	void Renderer::SetShader(Ptr<Shader> shader)
	{
		ResetBatch();
		RendererData.ActiveShader = shader;
	}

	void Renderer::ResetShader()
	{
		ResetBatch();
		RendererData.ActiveShader = RendererData.DefaultTextureShader;
	}

	void Renderer::SetViewport(Viewport viewport)
	{
		glViewport(viewport.Position.x, -viewport.Position.y, viewport.Size.x, viewport.Size.y);
	}

	void Renderer::SetClearColour(Colour colour)
	{
		RendererData.ClearColour = colour;
		glClearColor(colour.r, colour.g, colour.b, colour.a);
	}

	void Renderer::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Renderer::ShowRuntimeStatsWindow()
	{
		const auto& Runtime = Application::GetRuntimeData();
		const auto& DebugInfo = Renderer::GetDebugInfo();

		{
			ImGui::Begin("Render Debug");

			if (ImGui::CollapsingHeader("Performance"))
			{
				if (ImGui::BeginTable("performance_table", 2))
				{
					ImGui::TableNextRow();
					ImGui::TableNextColumn();

					ImGui::Text("FPS");
					ImGui::TableNextColumn();
					ImGui::Text(std::to_string(Runtime.Framerate).c_str());
					ImGui::TableNextColumn();

					ImGui::Text("Simulation Rate");
					ImGui::TableNextColumn();
					ImGui::Text(std::to_string(Runtime.SimulationRate).c_str());
					ImGui::TableNextColumn();

					ImGui::Text("Frame Delta");
					ImGui::TableNextColumn();
					ImGui::Text((std::to_string(Runtime.FrameDelta) + "ms").c_str());
					ImGui::TableNextColumn();

					ImGui::Text("Simulation Delta");
					ImGui::TableNextColumn();
					ImGui::Text((std::to_string(Runtime.SimulationDelta) + "ms").c_str());
					ImGui::TableNextColumn();

					ImGui::EndTable();
				}
			}

			if (ImGui::CollapsingHeader("Renderer Information"))
			{

				if (ImGui::BeginTable("renderer_info_table", 2))
				{
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					
					ImGui::Text("Draw calls");
					ImGui::TableNextColumn();
					ImGui::Text(std::to_string(DebugInfo.DrawCalls).c_str());
					ImGui::TableNextColumn();
				
					ImGui::Text("Vertex count");
					ImGui::TableNextColumn();
					ImGui::Text(std::to_string(DebugInfo.VertexCount).c_str());

					ImGui::EndTable();
				}
			}

			if (ImGui::CollapsingHeader("Script Runtime"))
			{
				if (ImGui::BeginTable("lua_info_table", 2))
				{
					ImGui::TableNextRow();
					ImGui::TableNextColumn();

					ImGui::Text("Lua VM memory usage");
					ImGui::TableNextColumn();
					ImGui::Text((std::to_string(std::floor(Runtime.LuaMemoryUsage / 10000.0) / 100.0) + "MB").c_str());
					ImGui::TableNextColumn();

					ImGui::EndTable();
				}
			}

			ImGui::End();
		}
	}
}