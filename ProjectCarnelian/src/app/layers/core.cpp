

#include <Engine.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "core.h"

using namespace Techless;

namespace Carnelian 
{

    //static float clock = 0.f;

    void Core::BuildEnvironment()
    {
        if (ScriptEnvironment::Has(LayerName))
        {
            LayerScript = ScriptEnvironment::RegisterGenericScript(LayerName);
            LayerScript->set("Scene", ScriptEnvironment::GetSceneBinding(ActiveScene->GetLuaID()));

            GetFunction("OnCreated")();
        }
        else
        {
            Debug::Log("Couldn't find a Lua script associated with layer with name " + LayerName, LayerName);
        }
    }

	void Core::OnCreated()
	{
        ActiveScene = Scene::Create();

        SceneExplorer.SetSceneContext(ActiveScene);
        BuildEnvironment();

        Renderer::SetClearColour(glm::vec4(0.1f, 0.1f, 0.1f, 1.f));
	}

    void Core::OnUpdateFixed(const float Delta)
    {
        GetFunction("OnFixedUpdate")(Delta);
        ActiveScene->FixedUpdate(Delta);

        FixedUpdateRate = Delta;
    }

    void Core::OnUpdate(const float Delta)
    {
        GetFunction("OnUpdate")(Delta);
        ActiveScene->Update(Delta);


        //Debug::Log(std::to_string(LayerScript.use_count()));

        /*
        if (clock > 1.f)
        {
            clock--;

            auto tags = ActiveScene->GetInstances<TagComponent>();

            for (auto& transform : *ActiveScene->GetInstances<TransformComponent>())
            {
                auto& e = transform.GetLinkedEntity()->GetComponent<TagComponent>();

                Debug::Log(e.Name + " " + std::to_string(transform.InterpolatedFrames));

                transform.InterpolatedFrames = 0;
            }
        }

        clock += Delta;*/

        UpdateRate = Delta;
    }

    void Core::OnUpdateEnd(const float Delta)
    {
        GetFunction("OnUpdateEnd");

        SceneExplorer.RenderImGuiElements();
        SceneConsole.RenderImGuiElements();

        /*ImGui::Begin("test");

        SimRatio = Application::GetActiveApplication().GetSimulationRatio();

        ImGui::Text(std::to_string(SimRatio).c_str()); 

        ImGui::End();*/

        Renderer::ShowRuntimeStatsWindow();
    }

    Input::Filter Core::OnInputEvent(InputEvent inputEvent, bool Processed)
    {
        Input::Filter res = GetFunction("OnInputEvent")(inputEvent, Processed);

        if (res == Input::Filter::Stop)
            return res;
        else if (res == Input::Filter::Continue)
            Processed = true;

        return ActiveScene->OnInputEvent(inputEvent, Processed);
    }

    void Core::OnWindowEvent(WindowEvent windowEvent)
    {
        GetFunction("OnWindowEvent")(windowEvent);

        ActiveScene->OnWindowEvent(windowEvent);
    }

/*  Entity& Core::CreatePlayer()
    {
        auto& PlayerPrefab = PrefabAtlas::Get("assets/prefabs/Player.prefab");
        auto& PlayerEntity = ActiveScene->Instantiate(PlayerPrefab);

        Debug::Log(PlayerEntity.GetID() + " " + PlayerEntity.GetComponent<TagComponent>().Name + " " + PlayerEntity.GetComponent<SpriteComponent>().GetSprite()->GetName());
        
        auto& Script = PlayerEntity.AddComponent<LuaScriptComponent>();
        Script.Bind("PlayerController");

        return PlayerEntity;
    }*/
}