
local ContainerItem = require("ContainerItem")

---@type ContainerItem
ContainingItem = nil;

---@param Item Item
---@param Quantity number
function SetContainingItem(Item, Quantity)
    ContainingItem = ContainerItem.new(Item, Quantity);
end