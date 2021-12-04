
#include <Engine.h>
#include <scriptindex.h>

#include <iostream>

#include <imgui/imgui.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "test_layer.h"

using namespace Techless;

namespace Sandbox {

    Ptr<Sprite> newSprite = nullptr;

	void MainLayer::OnCreated()
	{
        Debug::Log("Starting MainLayer...","MainLayer");

        ActiveScene = CreatePtr<Scene>();
        
        // test entity
        auto& SceneCamera = ActiveScene->CreateEntity();
        SceneCamera.AddComponent<TransformComponent>();
        SceneCamera.AddComponent<CameraComponent>();
        
        auto& Script = SceneCamera.AddComponent<ScriptComponent>();
        Script.Bind<Camera>(SceneCamera);
        
        ActiveScene->SetActiveCamera(SceneCamera);

        /*
        auto& TextureSheetEnt = ActiveScene->CreateEntity();
        auto& TexScript = TextureSheetEnt.AddComponent<ScriptComponent>();
        TexScript.Bind<TextureSheetDrawer>(TextureSheetEnt);
        */
        
        auto& NewEntity = ActiveScene->CreateEntity();

        auto& Spr = NewEntity.AddComponent<SpriteComponent>();
        Spr.aSprite = SpriteAtlas::GetSprite("logo");

        auto& Transform = NewEntity.AddComponent<TransformComponent>();
        Transform.Position = { 50.f, 50.f };
        Transform.Scale = { 100.f, 100.f };

        //Debug::Log("Done creating camera");

        /*
        auto& NewEntity = ActiveScene->CreateEntity();

        auto& Spr = NewEntity.AddComponent<SpriteComponent>();
        Spr.aSprite = SpriteAtlas::GetSprite("Logo");

        auto& Transform = NewEntity.AddComponent<TransformComponent>();
        Transform.Position = { 50.f, 50.f };
        Transform.Scale = { 100.f, 100.f };
        */

        /*
        for (unsigned int iX = 0; iX < 5; ++iX)
        {
            for (unsigned int iY = 0; iY < 5; ++iY)
            {
                auto& Ent = ActiveScene->CreateEntity();

                auto& NewEntTransform = Ent.AddComponent<TransformComponent>();
                NewEntTransform.Position = { 100.f + iX * 30.f, 100.f + iY * 30.f };
                NewEntTransform.Scale = { 20.f, 20.f};

                auto& NewEntSpr = Ent.AddComponent<SpriteComponent>();
                NewEntSpr.aSprite = SpriteAtlas::GetSprite("logo");

                auto& NewTag = Ent.AddComponent<TagComponent>();
                NewTag.Name = "Spinny";

                /*
                auto& NewScript = Ent.AddComponent<ScriptComponent>();
                NewScript.Bind<Spin>(Ent);
            }
        }*/

        //TestSprite = SpriteAtlas::GetSprite("Logo");

        Renderer::SetClearColour(glm::vec4(0.1f, 0.1f, 0.1f, 1.f));

        Debug::Log("Done!", "MainLayer");
	}

    void MainLayer::OnUpdateFixed(const float& Delta)
    {
        ActiveScene->FixedUpdate(Delta);

        FixedUpdateRate = Delta;
    }

    float Pos = 0;

    void MainLayer::OnUpdate(const float& Delta)
    {
        /*auto TagComponents = ActiveScene->GetInstances<TagComponent>();
        auto TransformComponents = ActiveScene->GetInstances<TransformComponent>();
        auto SpriteComponents = ActiveScene->GetInstances<SpriteComponent>();

        unsigned int i = 0;
        for (auto Tag : *TagComponents)
        {
            if (Tag.Name == "Spinny")
            {
                auto EntityID = TagComponents->GetIDAtIndex(i);
                
                auto& Transform = TransformComponents->Get(EntityID);
                Transform.Angle = Transform.Angle + 1.f * Delta;
            
                auto& Sprite = SpriteComponents->Get(EntityID);
                Sprite.Colour = glm::vec3(std::sin(Pos) + 1, std::sin(Pos + 2) + 1, std::sin(Pos + 3) + 1);
            }

            i++;
        }*/

        //Pos += 0.01f * Delta;


        ActiveScene->Update(Delta);

        UpdateRate = Delta;
    }

	void MainLayer::OnUpdateEnd(const float& Delta)
	{

	}
}