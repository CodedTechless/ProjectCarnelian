
#include <Engine.h>

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>

#include <imgui/imgui.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "main_layer.h"

using namespace Techless;

namespace TextureEditor {
    std::shared_ptr<Texture> LoadedTexture;
    std::unordered_map<std::string, std::shared_ptr<Texture>> CachedTextures;

    JSON CurrentAtlas;
    bool ProjectActive = false;
    
    bool NeedsSaving = false;

    char buf[250];
    std::string SelectedSprite = "";
    std::string ProjectPath = "";

    glm::vec2 TexTopLeft = { 0.f,0.f };
    glm::vec2 TexBottomRight = { 0.f,0.f };

    glm::vec2 CameraPos = { 0.f, 0.f };
    float Zoom = 1.f;

    constexpr char InfoFileName[] = "editorinfo.dat";
    
    Window* ActiveWindow;

    // sprite create window

    char SpriteName[30];

    int ValuesTopLeft[] = { TexTopLeft.x, TexTopLeft.y };
    int ValuesBottomRight[] = { TexBottomRight.x, TexBottomRight.y };
    int Origin[] = { 0.f, 0.f };

    std::string NewTextureName = "";

    std::shared_ptr<Texture> CreateNewTexture(const std::string& ID, const std::string& Path)
    {
        auto NewTexture = std::make_shared<Texture>(Path);
        CachedTextures[ID] = NewTexture;

        return NewTexture;
    }

    void OpenProject(const std::string& Path)
    {
        std::string TexDictLocation = Path + "\\assets\\texdict.json";

        std::ifstream SpriteDataFile(TexDictLocation.c_str());
        SpriteDataFile >> CurrentAtlas;

        ProjectActive = true;
        ProjectPath = Path;
    }

    void SelectSprite(const std::string& k)
    {
        auto SpriteData = CurrentAtlas["sprites"][k];

        auto TexSheetName = SpriteData["texture_sheet_name"].get<std::string>();
        auto TexSheetPath = CurrentAtlas["texture_sheets"][TexSheetName].get<std::string>();

        auto TopLeftData = SpriteData["top_left"];
        auto BottomRightData = SpriteData["bottom_right"];

        TexTopLeft = { TopLeftData["x"].get<unsigned int>() , TopLeftData["y"].get<unsigned int>() };
        TexBottomRight = { BottomRightData["x"].get<unsigned int>() , BottomRightData["y"].get<unsigned int>() };

        ValuesTopLeft[0] = TexTopLeft.x;
        ValuesTopLeft[1] = TexTopLeft.y;

        ValuesBottomRight[0] = TexBottomRight.x;
        ValuesBottomRight[1] = TexBottomRight.y;

        if (CachedTextures.find(TexSheetName) == CachedTextures.end())
        {
            if (!LoadedTexture || LoadedTexture->GetFilePath() != ProjectPath + "\\" + TexSheetPath)
            {
                auto NewTexture = CreateNewTexture(TexSheetName, ProjectPath + "\\" + TexSheetPath);

                LoadedTexture = NewTexture;
                Debug::Log("Loaded " + LoadedTexture->GetFilePath());
            }
        }
        else
        {
            LoadedTexture = CachedTextures[TexSheetName];
        }

        auto WindowSize = (glm::vec2)ActiveWindow->Size;
        glm::vec2 size = LoadedTexture->GetDimensions();
        CameraPos = (glm::vec2(640, 360) - (size / 2.f)) + ((TexTopLeft + TexBottomRight) / 2.f) - ((WindowSize * Zoom) / 2.f);

        memset(SpriteName, 0, sizeof SpriteName);
        strcpy_s(SpriteName, k.c_str());

        SelectedSprite = k;
        NewTextureName = TexSheetName;
    }

    bool OpenAlreadyActiveTexDict()
    {
        std::fstream EditorInfo;
        EditorInfo.open(InfoFileName, std::ofstream::in);
        if (EditorInfo)
        {
            if (EditorInfo.is_open())
            {
                std::string Line;
                if (getline(EditorInfo, Line))
                {
                    if (Line != "null")
                    {
                        OpenProject(Line);

                        EditorInfo.close();
                        return true;
                    }
                }
            }
        }
        EditorInfo.close();

        return false;
    }

	void MainLayer::OnCreated()
	{
        Debug::Log("Started MainLayer","MainLayer");

        Renderer::SetClearColour(glm::vec4(0.2f, 0.2f, 0.2f, 1.f));

        ProjectActive = OpenAlreadyActiveTexDict();

        ActiveWindow = Application::GetActiveApplication().GetActiveWindow();
	}

    bool MainLayer::OnInput(const InputObject& InputEvent)
    {
        auto io = ImGui::GetIO();

        if (io.WantCaptureMouse)
        {
            return false;
        }

        if (InputEvent.ScrollWheelPosition != 0)
        {
            auto WindowSize = (glm::vec2)ActiveWindow->Size;
            auto NewZoom = std::max(Zoom + InputEvent.ScrollWheelPosition * -1.f * (0.1 * Zoom), 0.1);
            CameraPos = CameraPos + (((WindowSize * Zoom) - (WindowSize * (float)NewZoom)) / 2.f);

            Zoom = NewZoom;
        }

        //std::cout << Zoom << std::endl;

        return false;
    }

    void MainLayer::OnUpdate(const float& Delta)
    {
        auto io = ImGui::GetIO();

        if (io.WantCaptureKeyboard == false)
        {
            if (Input::KeyDown(Keyboard::KeyCodes::A))
                CameraPos.x -= 10 * Delta;
            if (Input::KeyDown(Keyboard::KeyCodes::D))
                CameraPos.x += 10 * Delta;
            if (Input::KeyDown(Keyboard::KeyCodes::W))
                CameraPos.y -= 10 * Delta;
            if (Input::KeyDown(Keyboard::KeyCodes::S))
                CameraPos.y += 10 * Delta;
        }

        auto WindowSize = (glm::vec2)ActiveWindow->Size;

        glm::mat4 Proj = glm::ortho(0.f, WindowSize.x * Zoom, WindowSize.y * Zoom, 0.f, -100.f, 100.f);
        glm::mat4 Transform = glm::translate(glm::mat4(1.f), glm::vec3(CameraPos, 0.f));

        Renderer::Begin(Proj, Transform);

        if (LoadedTexture != nullptr)
        {
            glm::vec3 pos = { 640, 360, 0 };
            glm::vec2 size = LoadedTexture->GetDimensions();
            const glm::vec2 TexCoords[] = { {0.f, 0.f}, {1.f, 0.f}, {1.f, 1.f}, {0.f, 1.f} };

            Renderer::DrawQuad(LoadedTexture, TexCoords, pos, size);

            glm::vec2 Middle = (TexTopLeft + TexBottomRight) / 2.f;
            glm::vec2 Size = (TexBottomRight - TexTopLeft);

            Renderer::DrawBlankQuad({ (glm::vec2(pos) - (size / 2.f)) + Middle, 10.f }, Size, { 1, 0, 0, 0.25 });
        }

        Renderer::End();
    }

	void MainLayer::OnUpdateEnd(const float& Delta)
	{
        {
            ImGui::Begin("File", nullptr, ImGuiWindowFlags_NoResize);
            

            if (ProjectActive)
            {

                // Button for saving when changes have been made.
                if (NeedsSaving)
                    ImGui::Text("Unsaved Changes");

                if (ImGui::Button("Save") && NeedsSaving)
                {
                    NeedsSaving = false;

                    std::fstream ActiveFile(ProjectPath + "\\assets\\texdict.json", std::ofstream::out | std::ofstream::trunc);
                    ActiveFile << CurrentAtlas;
                    ActiveFile.close();
                }

                ImGui::SameLine();


                // Closes the project and writes a null to the active project file.
                if (ImGui::Button("Close Project"))
                {
                    LoadedTexture = nullptr;
                    CachedTextures.clear();
                    
                    ProjectActive = false;
                    NeedsSaving = false;

                    std::fstream ActiveProject("editorinfo.dat", std::ofstream::out | std::ofstream::trunc);
                    ActiveProject << "null";
                    ActiveProject.close();
                }
            } 
            else
            {
                // Input boxes for opening a new project.

                ImGui::SetWindowSize({ 350, 0 });

                ImGui::InputText("Path", buf, 250);

                if (ImGui::Button("Open Project"))
                {
                    ProjectPath = std::string(buf);
                    
                    OpenProject(ProjectPath);
                    
                    std::fstream EditorInfo;
                    EditorInfo.open(InfoFileName, std::ofstream::out | std::ofstream::trunc);
                    if (EditorInfo.is_open())
                    {
                        EditorInfo << ProjectPath;
                        EditorInfo.close();
                    }
                }
            }

            ImGui::End();
        }

        if (ProjectActive)
        {
            ImGui::Begin("Sprite Editor");

            {
                ImGui::BeginChild("Scrolling", { 0, 250 }, true, ImGuiWindowFlags_None);

                if (ImGui::BeginTable("spr", 4)) {
                    ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 20);
                    ImGui::TableSetupColumn("Sprite Name");
                    ImGui::TableSetupColumn("Texture Sheet");
                    ImGui::TableSetupColumn("Path");
                    ImGui::TableHeadersRow();

                    unsigned int n = 0;
                    for (auto Element : CurrentAtlas["sprites"].items())
                    {
                        auto k = Element.key();
                        auto v = Element.value();

                        auto TexSheetName = v["texture_sheet_name"].get<std::string>();
                        auto TexSheetPath = CurrentAtlas["texture_sheets"][TexSheetName].get<std::string>();

                        ImGui::TableNextRow();
                        std::string Num = std::to_string(n);

                        ImGui::TableNextColumn();
                        if (ImGui::Selectable(Num.c_str(),SelectedSprite == k, ImGuiSelectableFlags_SpanAllColumns))
                        {
                            SelectSprite(k);
                        }

                        ImGui::TableNextColumn();
                        ImGui::Text(k.c_str());

                        ImGui::TableNextColumn();
                        ImGui::Text(TexSheetName.c_str());

                        ImGui::TableNextColumn();
                        ImGui::Text(TexSheetPath.c_str());

                        n++;
                    }
                    ImGui::EndTable();
                        
                }

                ImGui::EndChild();
            }

            {
                ImGui::BeginChild("Actions", { 0,35 }, true);

                if (ImGui::Button("New Sprite"))
                {
                    std::string Name = "NewSprite";
                    auto n = 1;
                    while (CurrentAtlas["sprites"].contains(Name))
                    {
                        Name = "NewSprite" + std::to_string(n);
                        n++;
                    }
                        
                    JSON NewSprite = {
                        {"texture_sheet_name", "Test"},

                        {"top_left", {
                            {"x", 0},
                            {"y", 0}
                        }},

                        {"bottom_right", {
                            {"x", 32},
                            {"y", 32}
                        }}
                    };

                    CurrentAtlas["sprites"][Name] = NewSprite;

                    SelectSprite(Name);

                    NeedsSaving = true;
                }

                ImGui::SameLine();

                if (ImGui::Button("Apply"))
                {
                    auto NewSpriteName = std::string(SpriteName);
                    if (NewSpriteName != SelectedSprite)
                    {
                        CurrentAtlas["sprites"][NewSpriteName] = CurrentAtlas["sprites"][SelectedSprite];
                        CurrentAtlas["sprites"].erase(SelectedSprite);
                    }

                    CurrentAtlas["sprites"][NewSpriteName]["texture_sheet_name"] = NewTextureName;

                    CurrentAtlas["sprites"][NewSpriteName]["top_left"]["x"] = TexTopLeft.x;
                    CurrentAtlas["sprites"][NewSpriteName]["top_left"]["y"] = TexTopLeft.y;

                    CurrentAtlas["sprites"][NewSpriteName]["bottom_right"]["x"] = TexBottomRight.x;
                    CurrentAtlas["sprites"][NewSpriteName]["bottom_right"]["y"] = TexBottomRight.y;

                    SelectSprite(NewSpriteName);

                    NeedsSaving = true;
                }

                ImGui::EndChild();
            }

            ImGui::Text("Properties");
            
            ImGui::BeginChild("Properties", { 0,220 }, true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
            if (SelectedSprite != "")
            {
                //

                ImGui::BeginChild("Attributes", { 0, 65 }, true, ImGuiWindowFlags_NoScrollbar);
                ImGui::InputText("Name", SpriteName, 30);

                auto Sprite = CurrentAtlas["sprites"][SelectedSprite];
                //auto CurrentSheet = Sprite["texture_sheet_name"].get<std::string>();

                if (ImGui::BeginCombo("Sprite Sheet", NewTextureName.c_str()))
                {
                    for (auto Element : CurrentAtlas["texture_sheets"].items())
                    {
                        auto Sheet = Element.key();

                        if (ImGui::Selectable(Sheet.c_str(), Sheet == NewTextureName))
                        {
                            Debug::Log("Set " + SelectedSprite + "'s sprite sheet to " + Sheet);
                            NewTextureName = Sheet;

                            //NeedsSaving = true;
                        }

                        if (Sheet == NewTextureName)
                            ImGui::SetItemDefaultFocus();
                    }

                    ImGui::EndCombo();
                }

                ImGui::EndChild();
                
                //

                ImGui::BeginChild("Appearance", { 0,120 }, true, ImGuiWindowFlags_NoScrollbar);

                ImGui::Text("Sprite Coordinates");
                if (ImGui::InputInt2("Top Left", ValuesTopLeft))
                    TexTopLeft = { ValuesTopLeft[0], ValuesTopLeft[1] };
                if (ImGui::InputInt2("Bottom Right", ValuesBottomRight))
                    TexBottomRight = { ValuesBottomRight[0], ValuesBottomRight[1] };

                ImGui::Separator();

                ImGui::Text("Sprite Origin");
                ImGui::InputInt2("", Origin);

                ImGui::EndChild();
            }

            ImGui::EndChild();
            ImGui::End();
        }


        auto Runtime = Application::GetRuntimeData();
        auto DebugInfo = Renderer::GetDebugInfo();
        
		{
			ImGui::Begin("Render Debug");

            if (ImGui::CollapsingHeader("Performance"))
            {
                ImGui::Columns(2, "performance_table");

                std::string PerformanceLabels = "FPS\nUpdate Rate\nLast update took\nLast frame took";
                ImGui::Text(PerformanceLabels.c_str());

                ImGui::NextColumn();

                std::string PerformanceData = std::to_string(Runtime.Framerate) + "\n" + std::to_string(Runtime.UpdateRate) + "\n" + std::to_string(UpdateRate) + "ms\n" + std::to_string(Delta) + "ms";
                ImGui::Text(PerformanceData.c_str());

                ImGui::Columns();
            }

            if (ImGui::CollapsingHeader("Renderer Information"))
            {
                
                ImGui::Columns(2, "renderer_info_table");

                std::string RendererLabels = "Draw calls last frame\nVertex count last frame";
                ImGui::Text(RendererLabels.c_str());

                ImGui::NextColumn();

                std::string RendererData = std::to_string(DebugInfo.DrawCalls) + " calls\n" + std::to_string(DebugInfo.VertexCount) + " verticies";
                ImGui::Text(RendererData.c_str());

                ImGui::Columns();
            }

			ImGui::End();

		}
	}
}