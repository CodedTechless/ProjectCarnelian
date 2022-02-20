
#include "asset_manager.h"

#include <imgui/imgui.h>
#include <Engine.h>

#define THUMBNAIL_SIZE 100
#define THUMBNAIL_PADDING 10

using namespace Techless;
namespace fs = std::filesystem;

namespace PrefabEditor
{

	static void CreateDirectoryElement(const std::string& Name, fs::path Directory, fs::path& ToDirectory)
	{
//		Bounds spriteBounds = sprite->GetAbsoluteBounds();

//		ImTextureID RendererID = (ImTextureID)sprite->GetTexture()->GetRendererID();

		ImGui::PushID((Name + "_dir").c_str());
//		ImGui::ImageButton(RendererID, { THUMBNAIL_SIZE, THUMBNAIL_SIZE }, { spriteBounds.TopLeft.x, spriteBounds.BottomRight.y }, { spriteBounds.BottomRight.x, spriteBounds.TopLeft.y });
		ImGui::Button("Directory", { THUMBNAIL_SIZE + THUMBNAIL_PADDING, THUMBNAIL_SIZE + THUMBNAIL_PADDING });

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			ToDirectory = Directory;
		}

		ImGui::TextWrapped(Name.c_str());
		
		ImGui::PopID();
	}

	template<typename Object>
	static void CreateFileElement(const std::string& Name, Ptr<Sprite> sprite, std::string PayloadID, Object* PayloadObject, int Size)
	{
		Bounds spriteBounds = sprite->GetAbsoluteBounds();

		ImTextureID RendererID = (ImTextureID)sprite->GetTexture()->GetRendererID();

		ImGui::PushID(Name.c_str());
		
		if (sprite)
			ImGui::ImageButton(RendererID, { THUMBNAIL_SIZE, THUMBNAIL_SIZE }, { spriteBounds.TopLeft.x, spriteBounds.BottomRight.y }, { spriteBounds.BottomRight.x, spriteBounds.TopLeft.y });
		else
			ImGui::Button(Name.c_str(), { THUMBNAIL_SIZE + THUMBNAIL_PADDING, THUMBNAIL_SIZE + THUMBNAIL_PADDING });

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
		CurrentDirectory = fs::path("assets");
	}

	void AssetManagerPanel::RenderImGuiElements()
	{
		ImGui::Begin("Asset Manager");

		ImGui::BeginTabBar("##asset explorer");
		
		if (CurrentDirectory != fs::path("assets"))
		{
			if (ImGui::Button("<"))
			{
				CurrentDirectory = CurrentDirectory.parent_path();
			}
		}

		float CellSize = THUMBNAIL_SIZE + THUMBNAIL_PADDING;

		ImVec2 Region = ImGui::GetContentRegionAvail();
		int Columns = std::max(std::floor(Region.x / CellSize), 1.f);

		if (ImGui::BeginTable("##asset manager", Columns))
		{
			int Column = 0;
			
			auto prefabSprite = SpriteAtlas::Get("prefab");

			for (auto& Entry : fs::directory_iterator(CurrentDirectory))
			{

				fs::path path = Entry.path();
				std::string ext = path.extension().generic_string();
				std::string fileName = path.stem().generic_string();

				if (Entry.is_directory())
				{
					ImGui::TableNextColumn();
					CreateDirectoryElement(fileName, path, CurrentDirectory);

					++Column;
				}
				else if (ext == ".png")
				{
					if (SpriteAtlas::Has(fileName))
					{
						ImGui::TableNextColumn();
						CreateFileElement(fileName, SpriteAtlas::Get(fileName), "SPRITE_ASSET_DRAG", fileName.c_str(), (fileName.length() * sizeof(char)) + 1);

						++Column;
					}
				}
				else if (ext == ".prefab")
				{
					ImGui::TableNextColumn();
					std::string stringpath = path.generic_string();

					CreateFileElement(fileName, prefabSprite, "PREFAB_ASSET_DRAG", stringpath.c_str(), (stringpath.length() * sizeof(char)) + 1);

					++Column;
				}
				else if (ext == ".lua")
				{
					
				}

				
			}

			ImGui::EndTable();
		}

		ImGui::EndTabBar();

		ImGui::End();
	}


}
