
#include "asset_manager.h"

#include <imgui/imgui.h>
#include <Engine.h>

#define THUMBNAIL_SIZE 100
#define THUMBNAIL_PADDING 10

using namespace Techless;
namespace fs = std::filesystem;

namespace PrefabEditor
{

	template<typename Object>
	static void CreateFileElement(const std::string& Name, Ptr<Sprite> sprite, std::string PayloadID, Object* PayloadObject, int Size)
	{
		Bounds spriteBounds = sprite->GetAbsoluteBounds();

		ImTextureID RendererID = (ImTextureID)sprite->GetTexture()->GetRendererID();

		ImGui::PushID(Name.c_str());
		ImGui::ImageButton(RendererID, { THUMBNAIL_SIZE, THUMBNAIL_SIZE }, { spriteBounds.TopLeft.x, spriteBounds.BottomRight.y }, { spriteBounds.BottomRight.x, spriteBounds.TopLeft.y });

		if (ImGui::BeginDragDropSource())
		{
			ImGui::Image(RendererID, { THUMBNAIL_SIZE, THUMBNAIL_SIZE }, { spriteBounds.TopLeft.x, spriteBounds.TopLeft.y }, { spriteBounds.BottomRight.x, spriteBounds.BottomRight.y });
			ImGui::TextWrapped(Name.c_str());

			ImGui::SetDragDropPayload(PayloadID.c_str(), PayloadObject, Size);
			ImGui::EndDragDropSource();
		}
		
		ImGui::TextWrapped(Name.c_str());
		ImGui::PopID();
	}

	AssetManagerPanel::AssetManagerPanel()
	{
		RefreshPrefabs();
	}

	void AssetManagerPanel::RefreshSprites()
	{

	}

	void AssetManagerPanel::RefreshPrefabs(const std::string& Path)
	{
		PrefabFiles.clear();

		for (const auto& File : fs::directory_iterator(Path))
		{
			auto FsPath = File.path();
			auto Path = FsPath.generic_string();

			if (File.is_directory())
			{
				RefreshPrefabs(Path);
			}
			else if (File.is_regular_file() && Path.substr(Path.size() - 7) == ".prefab")
			{
				PrefabFiles.push_back({ Path, FsPath.stem().string()});
			}
		}
	}


	void AssetManagerPanel::RenderImGuiElements()
	{
		ImGui::Begin("Asset Manager");

		ImGui::BeginTabBar("##asset explorer");

		float CellSize = THUMBNAIL_SIZE + THUMBNAIL_PADDING;

		ImVec2 Region = ImGui::GetContentRegionAvail();
		int Columns = std::max(std::floor(Region.x / CellSize), 1.f);

		if (ImGui::BeginTabItem("Sprites", nullptr, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton))
		{
			if (ImGui::BeginTable("##asset manager", Columns))
			{
				int Column = 0;

				for (auto& [spriteName, sprite] : SpriteAtlas::GetSpriteCache())
				{
					if (Column >= Columns)
					{
						ImGui::TableNextRow();
						Column = 0;
					}

					ImGui::TableNextColumn();
					CreateFileElement(spriteName, sprite, "SPRITE_ASSET_DRAG", spriteName.c_str(), (spriteName.length() * sizeof(char)) + 1);

					++Column;
				}

				ImGui::EndTable();
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Prefabs"))
		{
			Ptr<Sprite> FileIcon = SpriteAtlas::Get("prefab");

			if (ImGui::BeginTable("##asset manager", Columns))
			{
				int Column = 0;
				
				for (auto& Item : PrefabFiles)
				{
					if (Column >= Columns)
					{
						ImGui::TableNextRow();
						Column = 0;
					}

					ImGui::TableNextColumn();
					CreateFileElement(Item.FileName, FileIcon, "PREFAB_ASSET_DRAG", &Item, sizeof(Item));

					++Column;
				}

				ImGui::EndTable();
			}

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();

		ImGui::End();
	}


}
