
#include <Engine.h>
#include <scriptindex.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "core.h"

using namespace Techless;

namespace Carnelian {

	void Core::OnCreated()
	{
        ActiveScene = CreatePtr<Scene>();

        auto& SceneCamera = ActiveScene->CreateEntity();
        SceneCamera.AddComponent<TransformComponent>();
        SceneCamera.AddComponent<CameraComponent>();

        auto& Script = SceneCamera.AddComponent<ScriptComponent>();
        Script.Bind<Camera>(SceneCamera);

        ActiveScene->SetActiveCamera(SceneCamera);

        Renderer::SetClearColour(glm::vec4(0.1f, 0.1f, 0.1f, 1.f));
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
}