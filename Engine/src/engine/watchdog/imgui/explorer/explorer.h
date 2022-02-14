#pragma once

#include <imgui/imgui.h>

#include <engine/entity/scene.h>
#include <engine/entity/entity.h>

namespace Techless
{
	typedef std::unordered_map<Entity*, std::vector<Entity*>> ExplorerIndex;

	class ExplorerPanel
	{
	public:
		ExplorerPanel() = default;
		
		void SetSceneContext(Ptr<Scene> sceneContext);
		void SetSelectedEntity(Entity* entity);

		inline Ptr<Scene> GetSceneContext() const { return SceneContext; };

		void RenderImGuiElements();
		void RenderProperties();

		void Refresh();

	private:
		template<class Component>
		void CreateMenuEntry(const char* Text)
		{
			if (ImGui::MenuItem(Text))
			{
				if (!SelectedEntity->HasComponent<Component>())
				{
					SelectedEntity->AddComponent<Component>();
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
		Ptr<Scene> SceneContext = nullptr;
		Entity* SelectedEntity = nullptr;

		std::vector<Entity*> TopLevelEntities = {};
		ExplorerIndex ParentEntities = {};
	};

}