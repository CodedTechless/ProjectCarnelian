
#include "editor_scene.h"

namespace PrefabEditor
{

	EditorScene::EditorScene(const std::string& sceneName, Prefab* LoadWithPrefab)
		: SceneName(sceneName)
	{
		LinkedScene = Scene::Create();
		LinkedScene->SetScriptExecutionEnabled(false);

		if (LoadWithPrefab)
		{
			SceneRoot = &LinkedScene->Instantiate(*LoadWithPrefab);
		}
		else
		{
			SceneRoot = &LinkedScene->CreateEntity("Root");
		}

		auto& SceneCamera = LinkedScene->CreateEntity("techless_EditorCamera");
		SceneCamera.AddComponent<CameraComponent>();
		SceneCamera.Archivable = false;

		auto& Script = SceneCamera.AddComponent<ScriptComponent>();
		ActiveCameraScript = Script.Bind<NativeScript::Core::Camera>(SceneCamera);

		LinkedScene->SetActiveCamera(SceneCamera);

	}

	void EditorScene::Save()
	{
		LinkedScene->Serialise("assets/prefabs/" + SceneName + ".prefab", *SceneRoot);
	}

}