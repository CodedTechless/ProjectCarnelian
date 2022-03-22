

local ItemIndex = {};

ItemIndex.Items = {

    ["BaseGame"] = {
        ["TestItem"] = {
            MaxStack = 10
        },
        ["TestItem2"] = {
            MaxStack = 15
        }
    }

}

function ItemIndex.GetItemData(ID, ContentLocation)
    ContentLocation = ContentLocation or "BaseGame";
    local Content = ItemIndex.Items[ContentLocation];

    return Content[ID];
end

return ItemIndex;