

#include <Engine.h>
#include <app/native_scripts/scriptindex.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <engine/maths/vector.hpp>

#include <sol/sol.hpp>

#include "core.h"

using namespace Techless;

namespace Carnelian 
{



	void Core::OnCreated()
	{
        sol::state lua;
        lua.open_libraries(sol::lib::base);

        auto usertype = lua.new_usertype<Vector2>("Vector2",
            sol::constructors<Vector2(), Vector2(float, float)>(),
            "X", &Vector2::X,
            "Y", &Vector2::Y);

        sol::protected_function res = lua.load("print(amogus)");
        
        sol::environment my_env(lua, sol::create, lua.globals());
        my_env["amogus"] = 5;

        sol::set_environment(my_env, res);
        res();
        

        sol::environment my_env_2(lua, sol::create, lua.globals());
        my_env_2["amogus"] = 10;

        sol::set_environment(my_env_2, res);
        res();


        lua.script("print(amogus)", my_env);
        lua.script("local hello = { fart = 1 }; local michael = hello; print(hello, michael);", my_env_2);



        ActiveScene = CreatePtr<Scene>();

        auto& SceneCamera = ActiveScene->CreateEntity();
        SceneCamera.AddComponent<TransformComponent>();
        SceneCamera.AddComponent<CameraComponent>();

        auto& Script = SceneCamera.AddComponent<ScriptComponent>();
        Script.Bind<Camera>(SceneCamera);

        ActiveScene->SetActiveCamera(SceneCamera);

        Renderer::SetClearColour(glm::vec4(0.1f, 0.1f, 0.1f, 1.f));

        CreatePlayer();
	}

    void Core::OnUpdateFixed(const float& Delta)
    {
        ActiveScene->FixedUpdate(Delta);

        FixedUpdateRate = Delta;
    }

    void Core::OnUpdate(const float& Delta)
    {
        ActiveScene->Update(Delta);

        UpdateRate = Delta;
    }

    Input::Filter Core::OnInputEvent(const InputEvent& inputEvent, bool Processed)
    {
        return ActiveScene->OnInputEvent(inputEvent, Processed);
    }

    void Core::OnWindowEvent(const WindowEvent& windowEvent)
    {
        ActiveScene->OnWindowEvent(windowEvent);
    }

    Entity& Core::CreatePlayer()
    {
        auto& PlayerPrefab = PrefabAtlas::Get("assets/prefabs/Player.prefab");
        auto& PlayerEntity = ActiveScene->Instantiate(PlayerPrefab);
        
        //auto& Script = PlayerEntity.AddComponent<ScriptComponent>();
        //Script.Bind<Player>(PlayerEntity);

        return PlayerEntity;
    }
}