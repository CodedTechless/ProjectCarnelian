
local ItemIndex = require("ItemIndex");

---@class Item
---@field ID string
---@field Attributes table
---@field Data table
local ItemClass = {}

ItemClass.__class = "Item";
ItemClass.__index = ItemClass;

function ItemClass.new(ItemID, ContentLocation)
	local self = setmetatable({}, ItemClass);

    self.ContentLocation = ContentLocation or "BaseGame";
	self.ID = ItemID;
    self.Data = ItemIndex.GetItemData(ItemID, ContentLocation)
	self.Attributes = {};

	return self;
end

function ItemClass.copy(ItemObject)
    local NewItem = ItemClass.new(ItemObject.ID, ItemObject.ContentLocation);
    NewItem.Attributes = table.copy(ItemObject.Attributes);

    return NewItem;
end


return ItemClass;