#pragma once

#include <Engine.h>
#include <scriptindex.h>

namespace PrefabEditor
{
	struct EditorScene
	{
		EditorScene() = default;
		EditorScene(const std::string& sceneName, Prefab* LoadWithPrefab = nullptr);

		void Save();


	public:
		std::string SceneName = "New Prefab";

		bool UnsavedChanges = true;

		Ptr<Scene> LinkedScene = nullptr;
		Ptr<NativeScript::Core::Camera> ActiveCameraScript = nullptr;

		Ptr<Entity> SceneRoot = nullptr;
		Ptr<Entity> SelectedEntity = nullptr;
	};
}
