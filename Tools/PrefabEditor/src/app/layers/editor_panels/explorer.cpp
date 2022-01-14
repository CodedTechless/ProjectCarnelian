
#include "explorer.h"

namespace PrefabEditor
{

    void ExplorerPanel::SetSceneContext(Ptr<Scene> sceneContext)
    {
        SceneContext = sceneContext;
        Refresh();
    }

    void ExplorerPanel::SetSelectedEntity(Entity& entity)
    {
        SelectedEntity = &entity;
        Refresh();
    }

    bool ExplorerPanel::RenderExplorerEntity(Entity* entity, const ExplorerIndex& explorerIndex)
    {
        if (!entity->Archivable)
            return false;

        bool RequiresRefresh = false;

        ImGuiTreeNodeFlags node_flags = (SelectedEntity && SelectedEntity->GetID() == entity->GetID() ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

        TransformComponent& Transform = entity->GetComponent<TransformComponent>();
        bool HasChildren = (explorerIndex.find(&Transform) != explorerIndex.end());

        if (!HasChildren)
        {
            node_flags |= ImGuiTreeNodeFlags_Leaf;
        }

        std::string EntityID = entity->GetID();

        std::string Tag = entity->GetComponent<TagComponent>().Name;
        bool Opened = ImGui::TreeNodeEx(entity->GetID().c_str(), node_flags, Tag.c_str());

        if (ImGui::BeginDragDropSource())
        {
            ImGui::Text(Tag.c_str());

            const char* CharEntityID = EntityID.c_str();

            ImGui::SetDragDropPayload("RE_PARENTING", (void*)CharEntityID, (EntityID.length() + 1) * sizeof(char) );
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget())
        {
            const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("RE_PARENTING");

            if (Payload)
            {
                std::string DeliveryEntityID = (const char*)Payload->Data;

                if (DeliveryEntityID != EntityID)
                {
                    auto Transforms = SceneContext->GetInstances<TransformComponent>();

                    if (Transforms->Has(DeliveryEntityID))
                    {
                        TransformComponent& otherTransform = Transforms->Get(DeliveryEntityID);
                        bool Success = otherTransform.SetParent(&Transform);

                        if (Success)
                            Debug::Log("Set parent of " + DeliveryEntityID + " to " + EntityID, "PrefabEditor");
                        else
                            Debug::Log("Couldn't set parent.", "PrefabEditor");

                        RequiresRefresh = true;
                    }
                }
            }

            ImGui::EndDragDropTarget();
        }

        if (ImGui::IsItemClicked())
        {
            SelectedEntity = entity;
        }

        if (Opened)
        {
            if (HasChildren)
            {
                for (Entity* f_Entity : explorerIndex.at(&Transform))
                {
                    auto NextRequiresRefresh = RenderExplorerEntity(f_Entity, explorerIndex);

                    if (NextRequiresRefresh)
                        RequiresRefresh = true;
                }
            }

            ImGui::TreePop();
        }

        return RequiresRefresh;
    }

	void ExplorerPanel::RenderImGuiElements()
	{

        ImGui::Begin("Explorer");

        if (SceneContext)
        {
            if (ImGui::BeginDragDropTarget())
            {
                const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("RE_PARENTING");

                Debug::Log("markiplier eat poopoo and caca");

                if (Payload)
                {
                    std::string DeliveryEntityID = (const char*)Payload->Data;

                    auto Transforms = SceneContext->GetInstances<TransformComponent>();
                    if (Transforms->Has(DeliveryEntityID))
                    {
                        TransformComponent& otherTransform = Transforms->Get(DeliveryEntityID);
                        otherTransform.SetParent(nullptr);

                        Debug::Log("Set parent to nullptr for " + DeliveryEntityID, "PrefabEditor");

                        Refresh();
                    }
                }

                ImGui::EndDragDropTarget();
            }

            bool FullRefresh = false;

            for (Entity* entity : TopLevelEntities)
            {
                bool RequiresRefresh = RenderExplorerEntity(entity, ParentEntities);

                if (RequiresRefresh)
                    FullRefresh = true;
            }

            if (FullRefresh)
                Refresh();
            
        }

        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem("Delete"))
            {
                SceneContext->DestroyEntity(SelectedEntity->GetID());
                
                SelectedEntity = nullptr;
                Refresh();
            }

            ImGui::EndPopup();
        }

        ImGui::End();

        ImGui::Begin("Properties");

        if (SelectedEntity)
            RenderProperties();


        ImGui::End();
	}

    void ExplorerPanel::Refresh()
    {
        TopLevelEntities.clear();
        ParentEntities.clear();

        auto Entities = SceneContext->GetInstances<Entity>();
        auto Transforms = SceneContext->GetInstances<TransformComponent>();

        int i = 0;
        for (auto& transform : *Transforms)
        {
            TransformComponent* Parent = transform.GetParent();

            Entity* Entity = &Entities->Get(Transforms->GetIDAtIndex(i++));
            if (Parent == nullptr)
                TopLevelEntities.emplace_back(Entity);
            else
                ParentEntities[Parent].emplace_back(Entity);
        }
    }

    template<typename Component, typename PropertyFunction>
    static void RenderComponentProperties(const char* Title, Entity& entity, PropertyFunction propertyFunction)
    {
        if (entity.HasComponent<Component>())
        {
            auto& component = entity.GetComponent<Component>();
            
            bool Open = ImGui::TreeNodeEx(Title, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding);

            if (Open)
            {
                propertyFunction(component);
                ImGui::TreePop();

                ImGui::Separator();
            }
        }
    }

    static void DrawVec2(const char* Title, glm::vec2& Base, float Width = 120.f)
    {
        float BaseArray[2] = { Base.x, Base.y };

        ImGui::PushItemWidth(Width);
        ImGui::InputFloat2(("##" + std::string(Title)).c_str(), BaseArray);
        ImGui::PopItemWidth();

        ImGui::SameLine();
        ImGui::Text(Title);

        Base = glm::vec2(BaseArray[0], BaseArray[1]);
    }

    void ExplorerPanel::RenderProperties()
    {
        ImGui::Text(SelectedEntity->GetComponent<TagComponent>().Name.c_str());
        ImGui::PushStyleColor(ImGuiCol_Text, { 1.f, 1.f, 1.f, 0.5f });
        ImGui::Text(SelectedEntity->GetID().c_str());
        ImGui::PopStyleColor();

        ImGui::Separator();

        RenderComponentProperties<TagComponent>("Tag", *SelectedEntity,
            [](TagComponent& Component)
            {
                char buf[50] = {};
                strcpy_s(buf, Component.Name.c_str());

                ImGui::PushItemWidth(150.f);
                ImGui::InputText("##name", buf, 50, ImGuiInputTextFlags_None);
                ImGui::PopItemWidth();

                ImGui::SameLine();
                ImGui::Text("Name");

                Component.Name = buf;
            });

        RenderComponentProperties<TransformComponent>("Transform", *SelectedEntity,
            [this](TransformComponent& Component)
            {

                //

                glm::vec3 LocalPosition3 = Component.GetLocalPosition();
                glm::vec2 LocalPosition = glm::vec2(LocalPosition3.x, LocalPosition3.y);
                DrawVec2("Local Position", LocalPosition);

                //

                glm::vec2 LocalScale = Component.GetLocalScale();
                DrawVec2("Local Scale", LocalScale);
                Component = LocalScale;

                //

                float LocalOrientation = Component.GetLocalOrientation();
                ImGui::PushItemWidth(55.f);
                ImGui::InputFloat("##orientation", &LocalOrientation);
                ImGui::PopItemWidth();


                ImGui::SameLine();
                ImGui::Text("Local Orientation");

                Component = LocalOrientation;

                //

                ImGui::PushItemWidth(55.f);
                ImGui::InputFloat("##z index", &LocalPosition3.z);
                ImGui::PopItemWidth();

                ImGui::SameLine();
                ImGui::Text("Relative Z-Index");

                Component = glm::vec3(LocalPosition, LocalPosition3.z);

                //

                char buf[50] = {};

                ImGui::PushItemWidth(120.f);
                ImGui::PushStyleColor(ImGuiCol_Text, { 1.f, 1.f, 1.f, 0.5f });
                ImGui::InputText("##parent", buf, 50, ImGuiInputTextFlags_ReadOnly);
                ImGui::PopStyleColor();
                ImGui::PopItemWidth();

                ImGui::SameLine(0.f, 0.f);
                if (ImGui::Button("C"))
                {
                    Component.SetParent(nullptr);
                    this->Refresh();
                }
                
                ImGui::SameLine();
                ImGui::Text("Parent");
            });

        RenderComponentProperties<SpriteComponent>("Sprite", *SelectedEntity,
            [](SpriteComponent& Component)
            {
                char buf[50] = {};

                ImGui::PushItemWidth(150.f);
                ImGui::PushStyleColor(ImGuiCol_Text, { 1.f, 1.f, 1.f, 0.5f });
                ImGui::InputText("##sprite name", buf, 50, ImGuiInputTextFlags_ReadOnly);
                ImGui::PopStyleColor();
                ImGui::PopItemWidth();

                if (ImGui::BeginDragDropTarget())
                {
                    const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("SPRITE_ENTRY");

                    if (Payload)
                    {

                    }

                    ImGui::EndDragDropTarget();
                }

                ImGui::SameLine();
                ImGui::Text("Sprite Name");

                
                float col[4] = { Component.SpriteColour.R, Component.SpriteColour.G, Component.SpriteColour.B, Component.SpriteColour.A };
                
                ImGui::PushItemWidth(200.f);
                ImGui::ColorEdit4("##sprite tint", col);
                ImGui::PopItemWidth();

                ImGui::SameLine(); 
                ImGui::Text("Colour Tint");

                Component.SpriteColour.R = col[0];
                Component.SpriteColour.G = col[1];
                Component.SpriteColour.B = col[2];
                Component.SpriteColour.A = col[3];
            });

        RenderComponentProperties<CameraComponent>("Camera", *SelectedEntity,
            [](CameraComponent& Component)
            {

            });

        RenderComponentProperties<ScriptComponent>("Script", *SelectedEntity,
            [](ScriptComponent& Component)
            {

            });

        auto WindowWidth = ImGui::GetWindowSize().x;
        auto TextWidth = ImGui::CalcTextSize("Create Component").x;
        ImGui::SetCursorPosX((WindowWidth - TextWidth) * 0.5f);

        if (ImGui::Button("Create Component"))
            ImGui::OpenPopup("CreateComponent");

        if (ImGui::BeginPopup("CreateComponent"))
        {
            CreateMenuEntry<TransformComponent>("Transform");
            CreateMenuEntry<TagComponent>("Tag");

            ImGui::Separator();

            CreateMenuEntry<CameraComponent>("Camera");
            
            CreateMenuEntry<SpriteComponent>("Sprite");
            CreateMenuEntry<AnimatorComponent>("Animator");

            ImGui::Separator();

            CreateMenuEntry<RigidBodyComponent>("Rigid Body");
            //CreateMenuEntry<BoxColliderComponent>("Box Collider");

            ImGui::Separator();

            CreateMenuEntry<ScriptComponent>("Script");

            ImGui::EndPopup();
        }
    }
}

/*
const char* Items[] = { "Cum", "Semen", "Ejaculation" };
const char* CurrentItem = Items[0];

if (ImGui::BeginCombo("##combo", "Add Component..."))
{

    for (int n = 0; n < IM_ARRAYSIZE(Items); n++)
    {
        bool IsSelected = (Items[n] == CurrentItem);
        if (ImGui::Selectable(Items[n], IsSelected))
        {
            CurrentItem = Items[n];
        }

        if (IsSelected)
        {
            ImGui::SetItemDefaultFocus();
        }
    }
    ImGui::EndCombo();
}*/