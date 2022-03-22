
---@class ContainerItem
---@field Item Item
---@field Quantity number
local ContainerItemClass = {};

ContainerItemClass.__class = "ContainerItem";
ContainerItemClass.__index = ContainerItemClass;

function ContainerItemClass.new(ItemObject, Quantity)
    local self = setmetatable({}, ContainerItemClass);

    Quantity = Quantity or 1;

    self.Item = ItemObject;
    self.Quantity = math.min(Quantity, ItemObject.Data.MaxStack); -- ContainerItem requires capped values.

    return self, math.max(Quantity - ItemObject.Data.MaxStack, 0); 
    -- the constructor also returns the remainder. this is in case an unacceptable Quantity
    -- is inserted into the constructor!
end

return ContainerItemClass;