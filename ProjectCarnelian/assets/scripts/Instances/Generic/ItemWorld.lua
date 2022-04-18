
local ContainerItem = require("ContainerItem")

---@type ContainerItem
ContainingItem = nil;

---@param Item Item
---@param Quantity number
function SetContainingItem(Item, Quantity)
    ContainingItem = ContainerItem.new(Item, Quantity);
end

function OnCreated()
    table.insert(Scene.Items, self);
end

function OnDestroy()
    for i, Item in next, Scene.Items do
        if Item.ID == ID then
           table.remove(Scene.Items, i);
           
           break;
        end
    end
end