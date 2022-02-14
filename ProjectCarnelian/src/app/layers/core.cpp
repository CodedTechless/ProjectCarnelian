

#include <Engine.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "core.h"

using namespace Techless;

namespace Carnelian 
{

    void Core::BuildEnvironment()
    {
        if (ScriptEnvironment::Has(LayerName))
        {
            LayerScript = ScriptEnvironment::CreateGlobal(LayerName);
            LayerScript->set("Scene", ScriptEnvironment::GetSceneBinding(ActiveScene->GetScriptEnvID()));

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
        SceneExplorer.Refresh();

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

        UpdateRate = Delta;
    }

    void Core::OnUpdateEnd(const float Delta)
    {
        GetFunction("OnUpdateEnd");
        SceneExplorer.RenderImGuiElements();

        Renderer::ShowRuntimeStatsWindow();
    }

    Input::Filter Core::OnInputEvent(const InputEvent& inputEvent, bool Processed)
    {
        Input::Filter res = GetFunction("OnInputEvent")(inputEvent, Processed);

        if (res == Input::Filter::Stop)
            return res;
        else if (res == Input::Filter::Continue)
            Processed = true;

        return ActiveScene->OnInputEvent(inputEvent, Processed);
    }

    void Core::OnWindowEvent(const WindowEvent& windowEvent)
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