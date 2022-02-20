
#include "explorer.h"

#include <engine/entity/components.h>

namespace Techless
{

    void ExplorerPanel::SetSceneContext(Ptr<Scene> sceneContext)
    {
        SceneContext = sceneContext;
    }

    void ExplorerPanel::SetSelectedEntity(Entity* entity)
    {
        SelectedEntity = entity;
    }

    bool ExplorerPanel::RenderExplorerEntity(Entity* entity)
    {
        if (!entity->Archivable)
        {
            return false;
        }

        bool RequiresRefresh = false;

        // set up the node flags
        ImGuiTreeNodeFlags node_flags = (SelectedEntity && SelectedEntity->GetID() == entity->GetID() ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

        // if it has children, turn it into a leaf
        bool HasChildren = entity->GetChildren().size();
        if (!HasChildren)
        {
            node_flags |= ImGuiTreeNodeFlags_Leaf;
        }

        // create the tree node. also create drag and drop sources and targets for reparenting!
        std::string Tag = entity->GetComponent<TagComponent>().Name;
        bool Opened = ImGui::TreeNodeEx(entity->GetID().c_str(), node_flags, Tag.c_str());

        if (ImGui::BeginDragDropSource())
        {
            ImGui::Text(Tag.c_str());

            std::string EntityID = entity->GetID();
            const char* c_EntityID = EntityID.c_str();

            ImGui::SetDragDropPayload("RE_PARENTING", c_EntityID, (EntityID.length() * sizeof(char)) + 1);
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget())
        {
            const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("RE_PARENTING");

            if (Payload)
            {
                const std::string DeliveryEntityID = (const char*)Payload->Data;


                if (DeliveryEntityID != entity->GetID())
                {
                    Entity& DeliveryEntity = SceneContext->GetInstanceByID<Entity>(DeliveryEntityID);
                    DeliveryEntity.SetParent(entity);

                    if (DeliveryEntity.GetParent() == entity)
                        Debug::Log("Set parent of " + DeliveryEntity.GetID() + " to " + entity->GetID(), "PrefabEditor");
                    else
                        Debug::Log("Unable to set parent.", "PrefabEditor");

                    RequiresRefresh = true;
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
                for (Entity* f_Entity : entity->GetChildren())
                {
                    bool NextRequiresRefresh = RenderExplorerEntity(f_Entity);

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
            bool FullRefresh = false;

            for (Entity& entity : *SceneContext->GetInstances<Entity>())
            {
                if (entity.GetParent() == nullptr)
                {
                    RenderExplorerEntity(&entity);
                }
            }
            
            if (ImGui::BeginPopupContextWindow())
            {
                if (ImGui::MenuItem("Duplicate"))
                {

                }

                if (ImGui::MenuItem("Delete"))
                {
                    SelectedEntity->Destroy();
                    SelectedEntity = nullptr;
                }

                ImGui::EndPopup();
            }
        }

        ImGui::End();

        ImGui::Begin("Properties");

        if (SceneContext && SelectedEntity)
            RenderProperties();

        ImGui::End();
	}

    template<typename Component, typename PropertyFunction>
    static void RenderComponentProperties(const char* Title, Entity* entity, PropertyFunction propertyFunction)
    {
        if (entity->HasComponent<Component>())
        {
            auto& component = entity->GetComponent<Component>();
            
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

    static void DrawReadOnly(const std::string& Name, const std::string& Text = "")
    {
        char buf[50] = {};
        strcpy_s(buf, Text.c_str());

        ImGui::PushItemWidth(150.f);
        ImGui::PushStyleColor(ImGuiCol_Text, { 1.f, 1.f, 1.f, 0.5f });
        ImGui::InputText(Name.c_str(), buf, 50, ImGuiInputTextFlags_ReadOnly);
        ImGui::PopStyleColor();
        ImGui::PopItemWidth();
    }

    static void DrawTextLabel(const std::string& Label)
    {
        ImGui::SameLine();
        ImGui::Text(Label.c_str());
    }

    void ExplorerPanel::RenderProperties()
    {
        ImGui::Text(SelectedEntity->GetComponent<TagComponent>().Name.c_str());
        ImGui::PushStyleColor(ImGuiCol_Text, { 1.f, 1.f, 1.f, 0.5f });
        ImGui::Text(SelectedEntity->GetID().c_str());
        ImGui::PopStyleColor();

        Entity* Parent = SelectedEntity->GetParent();
        std::string Text = Parent ? Parent->GetComponent<TagComponent>().Name.c_str() : "None";

        DrawReadOnly("##parent", Text);
        DrawTextLabel("Parent");

        ImGui::Separator();

        RenderComponentProperties<TagComponent>("Tag", SelectedEntity,
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

        RenderComponentProperties<TransformComponent>("Transform", SelectedEntity,
            [this](TransformComponent& Component)
            {

                //

                glm::vec3 LocalPosition3 = Component.GetLocalPosition();
                glm::vec2 LocalPosition = glm::vec2(LocalPosition3.x, LocalPosition3.y);
                DrawVec2("Local Position", LocalPosition);

                //

                glm::vec2 LocalScale = Component.GetLocalScale();
                DrawVec2("Local Scale", LocalScale);
                Component.SetLocalScale(LocalScale);

                //

                float LocalOrientation = Component.GetLocalOrientation();
                ImGui::PushItemWidth(55.f);
                ImGui::InputFloat("##orientation", &LocalOrientation);
                ImGui::PopItemWidth();


                ImGui::SameLine();
                ImGui::Text("Local Orientation");

                Component.SetLocalOrientation(LocalOrientation);

                //

                ImGui::PushItemWidth(55.f);
                ImGui::InputFloat("##z index", &LocalPosition3.z);
                ImGui::PopItemWidth();

                ImGui::SameLine();
                ImGui::Text("Relative Z-Index");

                Component.SetLocalPosition(glm::vec3(LocalPosition, LocalPosition3.z));

                //


            });

        RenderComponentProperties<SpriteComponent>("Sprite", SelectedEntity,
            [](SpriteComponent& Component)
            {

                Ptr<Sprite> sprite = Component.GetSprite();
                std::string Text = sprite ? sprite->GetName() : "None";

                DrawReadOnly("##sprite name", Text);

                if (ImGui::BeginDragDropTarget())
                {
                    const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("SPRITE_ASSET_DRAG");

                    if (Payload)
                    {
                        const std::string SpriteName = (const char*)Payload->Data;
                        
                        Component.SetSprite(SpriteAtlas::Get(SpriteName));
                    }

                    ImGui::EndDragDropTarget();
                }

                DrawTextLabel("Sprite");

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

        RenderComponentProperties<CameraComponent>("Camera", SelectedEntity,
            [](CameraComponent& Component)
            {

            });

        RenderComponentProperties<LuaScriptComponent>("Lua Script", SelectedEntity,
            [](LuaScriptComponent& Component)
            {
                std::string ScriptName = Component.GetScriptName();
                std::string Text = (ScriptName != "" ? ScriptName : "None");
                
                DrawReadOnly("##script", Text);
                DrawTextLabel("Script");
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

            //CreateMenuEntry<ScriptComponent>("Script");
            CreateMenuEntry<LuaScriptComponent>("Lua Script");

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