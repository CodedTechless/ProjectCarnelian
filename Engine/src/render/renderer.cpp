
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "engine/watchdog/watchdog.h"

#include "renderer.h"

// {0.f, 0.f}, { 1.f, 0.f }, { 1.f, 1.f }, { 0.f, 1.f }

namespace Techless {

	struct QuadFormat {
		glm::vec3 Position;
		glm::vec4 Colour;
		glm::vec2 TexCoords;
		float TexIndex;
	};

	struct RendererDataSet {
		static const uint32_t MaxQuads = 20000;
		static const uint32_t MaxVerts = MaxQuads * 4;
		static const uint32_t MaxInds = MaxQuads * 6;
		
		QuadFormat* QuadArray = nullptr;
		QuadFormat* QuadArrayPointer = nullptr;

		unsigned int CurrentVertexIndex = 0;

		std::shared_ptr<VertexArray> QuadVertexArray;

		std::shared_ptr<VertexBuffer> BatchVertexBuffer;
		std::shared_ptr<IndexBuffer> BatchIndBuffer;

		std::shared_ptr<Shader> TextureShader;

		unsigned int NextTextureSlot = 1;
		static const int ActiveTextureSlots = 16;
		std::array<std::shared_ptr<Texture>, 16> ActiveTextures;

		glm::vec4 VertexDefault[4] = {};
	};

	static RendererDataSet RendererData;
	RendererDebug Renderer::DebugInfo = {};

	int Renderer::MaxTextureSize = 1024;
	std::shared_ptr<Texture> Renderer::DefaultTexture = nullptr;

	void OpenGLDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		Debug::OpenGLMessage(std::string(message),source, type, id, severity);
	}

	void Renderer::Init()
	{
		Debug::Log("Initialising renderer...", "Renderer");

#if DEBUG
		Debug::Log("Debug mode is enabled in this build.", "Renderer");

		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLDebugMessage, nullptr);

		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif

		// Enable depth testing (for Z-axis based depth) and blending (for alpha support)
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		GLint glMaxTextureSize;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glMaxTextureSize);
		MaxTextureSize = std::min((int)glMaxTextureSize, 2048);

		/*
		GLint MaxTextureSlots;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &MaxTextureSlots);
		Renderer::MaxTextureUnits = MaxTextureSlots;
		RendererData.ActiveTextureSlots = MaxTextureSlots;*/

		Debug::Log("MaxTextureSize: " + std::to_string(MaxTextureSize) + " (" + std::to_string(glMaxTextureSize) + ")", "Renderer");
//		Debug::Log("GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS: " + std::to_string(MaxTextureSlots), "Renderer");

		// Create the vertex array which will hold attribute information for our batch vertex buffer and index buffer.
		RendererData.QuadVertexArray = std::make_shared<VertexArray>();
		RendererData.QuadVertexArray->Bind();

		// Create a new array of quads to be pushed into a vertex buffer. Then, new verticies can be pushed in real time.
		RendererData.QuadArray = new QuadFormat[RendererDataSet::MaxVerts];
		
		// Set up an empty unsigned int array to hold our quad indicies in the correct format.
		auto IndexBufferTemplate = new unsigned int[RendererDataSet::MaxInds];

		unsigned int QuadOffset = 0;
		for (auto i = 0; i < RendererDataSet::MaxInds; i += 6) {
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
		RendererData.TextureShader = std::make_shared<Shader>();
		RendererData.TextureShader->ImportShader("assets/shaders/basic.shader");
		RendererData.TextureShader->Bind();

		// Assign texture slot 1 to the white "default" texture.
		RendererData.ActiveTextures[0] = std::make_shared<Texture>("assets/default.png");
		RendererData.ActiveTextures[0]->Bind();
		DefaultTexture = RendererData.ActiveTextures[0];

		// Fill the samplers uniform with the numbers for each active texture slot
		auto Samplers = new int[RendererDataSet::ActiveTextureSlots];
		for (auto i = 0; i < RendererDataSet::ActiveTextureSlots; i++)
		{
			Samplers[i] = i;
		}

		RendererData.TextureShader->SetUniform1iv("Samplers", Samplers, RendererDataSet::ActiveTextureSlots);

		// Set the default vertex positions (verticies are centred by default.)
		RendererData.VertexDefault[0] = { -0.5f,  0.5f, 0.f, 1.f };
		RendererData.VertexDefault[1] = {  0.5f,  0.5f, 0.f, 1.f };
		RendererData.VertexDefault[2] = {  0.5f, -0.5f, 0.f, 1.f };
		RendererData.VertexDefault[3] = { -0.5f, -0.5f, 0.f, 1.f };
		
		// Set up ImGui for debug UI and shit like that
		IMGUI_CHECKVERSION();											// check the currently running version
		ImGui::CreateContext();											// create the ImGui context
		ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);	// bind it to the currently active GLFW window
		ImGui::StyleColorsDark();										// sets the window colour style to dark mode
		ImGui_ImplOpenGL3_Init((char*)glGetString(330));				// initialises it in OpenGL mode

		auto e = glGetString(GL_VERSION);
		Debug::Log("Started renderer. (OpenGL " + std::string((const char*)e) + ")", "Renderer");
	}


	/////////////
	// Sprites //
	/////////////

	// Draw a sprite with vector2 position and scale, colour, transparency, z-depth and an angle.
	void Renderer::DrawSprite(const std::shared_ptr<Sprite> sprite, const glm::vec2& Position, const glm::vec2& Size, float Depth, float Angle, const glm::vec3& Colour, float Alpha)
	{
		auto Texture = sprite->GetTexture();
		auto SpriteBounds = sprite->GetBounds();
		auto TextureSize = Texture->GetDimensions();

		auto TopLeft = SpriteBounds.TopLeft / TextureSize;
		auto BottomRight = SpriteBounds.BottomRight / TextureSize;

		glm::vec2 TexCoords[4] = { { TopLeft.x, TopLeft.y }, { BottomRight.x, TopLeft.y }, { BottomRight.x, BottomRight.y }, {TopLeft.x, BottomRight.y} };
		DrawQuad(Texture, TexCoords, glm::vec3(Position, Depth), Size, glm::vec4(Colour, Alpha), Angle);
	}

	////////////////////
	// Quad Rendering //
	////////////////////

	void Renderer::DrawBlankQuad(const glm::vec3& Position, const glm::vec2& Size, const glm::vec4& Colour, float Angle)
	{
		glm::mat4 Transform = glm::translate(glm::mat4(1.0f), Position);
		
		if (Angle != 0) Transform *= glm::rotate(glm::mat4(1.0f), glm::radians(Angle), glm::vec3(0, 0, 1));
		if (Size != glm::vec2(1.f, 1.f)) Transform *= glm::scale(glm::mat4(1.0f), glm::vec3(Size.x, Size.y, 1));

		glm::vec2 TexCoords[] = { { 0.f, 0.f }, { 1.f, 0.f }, { 1.f, 1.f }, { 0.f, 1.f } };
		DrawQuad(RendererData.ActiveTextures[0], TexCoords, Transform, Colour);
	}

	// Draw a quad using vector coordinates with a colour and an angle
	void Renderer::DrawQuad(const std::shared_ptr<Texture> Tex, const glm::vec2 TexCoords[4], const glm::vec3& Position, const glm::vec2& Size, const glm::vec4& Colour, float Angle)
	{
		glm::mat4 Transform =
			glm::translate(glm::mat4(1.0f), Position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(Angle), glm::vec3(0, 0, 1))
			* glm::scale(glm::mat4(1.0f), glm::vec3(Size.x, Size.y, 1));

		DrawQuad(Tex, TexCoords, Transform, Colour);
	}

	// Draw a quad using matricies
	void Renderer::DrawQuad(const std::shared_ptr<Texture> Tex, const glm::vec2 TexCoords[4], const glm::mat4& Transform, const glm::vec4& Colour)
	{
		if (RendererData.CurrentVertexIndex >= RendererDataSet::MaxInds)
		{
			ForceReset();
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
				ForceReset();
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

	void Renderer::DrawQuadArray(const std::shared_ptr<Texture> Tex, const glm::vec2 TexCoords[4], Quad* QuadArray, unsigned int Count)
	{
		if (RendererData.CurrentVertexIndex >= RendererDataSet::MaxInds)
		{
			ForceReset();
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
				ForceReset();
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
				RendererData.QuadArrayPointer->Colour = NextQuad.Colour;
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

	void Renderer::Begin(glm::mat4 Projection, glm::mat4 Transform)
	{	
		// begins a new scene
		RendererData.NextTextureSlot = 1;
		RendererData.CurrentVertexIndex = 0;
		RendererData.QuadArrayPointer = RendererData.QuadArray;

		glm::mat4 Proj = Projection * glm::inverse(Transform);
		RendererData.TextureShader->Bind();
		RendererData.TextureShader->SetUniformMat4f("u_Proj", Proj);

		DebugInfo.DrawCalls = 0;
		DebugInfo.VertexCount = 0;

	}

	void Renderer::End()
	{
		// ends the active scene
		auto Size = (uint32_t)((uint8_t*)RendererData.QuadArrayPointer - (uint8_t*)RendererData.QuadArray);
		
		RendererData.BatchVertexBuffer->Set(RendererData.QuadArray, Size);

		for (auto i = 0; i < RendererData.NextTextureSlot; i++)
		{
			RendererData.ActiveTextures[i]->Bind(i);
		}

		Flush();
	}

	void Renderer::Flush()
	{
		// flushes the pending renders
		if (RendererData.CurrentVertexIndex == 0) {
			return;
		}

		glDrawElements(GL_TRIANGLES, RendererData.CurrentVertexIndex, GL_UNSIGNED_INT, nullptr); // draws the array of buffer data that is currently binded

		DebugInfo.DrawCalls++;
	}

	void Renderer::ForceReset()
	{
		// used when max indicies/vertex/textures are taken. resets the pipeline and starts a new scene to take over (to avoid a crash)
		End();

		std::cout << "Reset forced" << std::endl;

		RendererData.NextTextureSlot = 1;
		RendererData.CurrentVertexIndex = 0;
		RendererData.QuadArrayPointer = RendererData.QuadArray;
	}

	void Renderer::SetClearColour(glm::vec4 Colour)
	{
		glClearColor(Colour.x, Colour.y, Colour.z, Colour.w);
	}

	void Renderer::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void Renderer::RenderImGuiElements()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void Renderer::Stop()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
}