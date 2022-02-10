#pragma once

#include <imgui/imgui.h>

#include <editor_scene/editor_scene.h>

#include <Engine.h>

using namespace Techless;

namespace PrefabEditor
{
	typedef std::unordered_map<Entity*, std::vector<Entity*>> ExplorerIndex;

	class ExplorerPanel
	{
	public:
		ExplorerPanel() = default;
		
		void SetSceneContext(Ptr<EditorScene> sceneContext);
		void SetSelectedEntity(Entity* entity);

		inline Ptr<EditorScene> GetSceneContext() const { return SceneContext; };

		void RenderImGuiElements();
		void RenderProperties();

		void Refresh();

	private:
		template<class Component>
		void CreateMenuEntry(const char* Text)
		{
			if (ImGui::MenuItem(Text))
			{
				if (!SceneContext->SelectedEntity->HasComponent<Component>())
				{
					SceneContext->SelectedEntity->AddComponent<Component>();
					Debug::Log("Added a " + std::string(typeid(Component).name()) + " to entity.", "PrefabEditor");
				}
				else
				{
					Debug::Log("Attempted to add a " + std::string(typeid(Component).name()) + " to entity, but entity already had that component.", "PrefabEditor");
				}

				ImGui::CloseCurrentPopup();
			}
		}

		bool RenderExplorerEntity(Entity* entity);
	
	private:
		Ptr<EditorScene> SceneContext;

		std::vector<Entity*> TopLevelEntities = {};
		ExplorerIndex ParentEntities = {};
	};

}