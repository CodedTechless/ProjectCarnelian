
local Item = require("Item");
local ContainerItem = require("ContainerItem");

---@class Container
---@field Size number
---@field Contents table
local ContainerClass = {};

ContainerClass.__class = "Container";
ContainerClass.__index = ContainerClass;

function ContainerClass.new(Size)
	local self = setmetatable({}, ContainerClass);

	self.Size = Size;
	self.Contents = {};

	for _ = 1, Size do
		table.insert(self.Contents, 0);
	end

	return self;
end

---@param _callback function<ContainerItem, number>
function ContainerClass:ForEach(_callback)
	for i, ItemContent in ipairs(self.Contents) do
		_callback(ItemContent, i);
	end
end


---@param ItemObject Item
---@return table
function ContainerClass:GetIndexesOf(ItemObject)
    local Items = {};

    for i, ContainerItemObject in next, self.Contents do
        if type(ContainerItemObject) == "table" and ItemObject.ID == ContainerItemObject.Item.ID then
            table.insert(Items, i);
        end
    end

    return Items;
end

---@return number
function ContainerClass:GetNextEmptySlot()
	for i, v in ipairs(self.Contents) do
		if v == 0 then
			return i;
		end
	end

	return -1;
end

-- Takes in an Index, an Item and an amount. Attempts to add it to that slot, and returns
-- a ContainerItem with the Item and the remainding Quantity.
---@param Index number
---@param InsertingItem Item
---@param Quantity number
---@return number Remainder
function ContainerClass:Insert(Index, InsertingItem, Quantity)
    local ContainingItem = self.Contents[Index];

    if ContainingItem == 0 then
        local NewItem, Remainder = ContainerItem.new(Item.copy(InsertingItem), Quantity);
        self.Contents[Index] = NewItem;

        return Remainder;
    elseif ContainingItem.Item.ID == InsertingItem.ID then
        local MaxStack = ContainingItem.Item.Data.MaxStack;

        if ContainingItem.Quantity >= MaxStack then
            return Quantity;
        end

        local NewAmount = ContainingItem.Quantity + Quantity; -- could be larger than MaxStack
        local Remainder = math.max(NewAmount - MaxStack, 0);
        
        ContainingItem.Quantity = math.min(NewAmount, MaxStack);
        
        return Remainder;
    end

    return Quantity;
end

---@param ItemObject Item
---@param Quantity number
function ContainerClass:Add(ItemObject, Quantity)
    if Quantity == 0 then
        return 0;
    end

    local Items = self:GetIndexesOf(ItemObject);

    while #Items > 0 do
        local CurrentIndex = Items[1];

        Quantity = self:Insert(CurrentIndex, ItemObject, Quantity);
        table.remove(Items, 1);
    end

    while Quantity > 0 do
        local NextSlot = self:GetNextEmptySlot();
        if NextSlot == -1 then
            break;
        end

        Quantity = self:Insert(NextSlot, ItemObject, Quantity);
    end

    return Quantity; -- if the add was successful, it should return 0
end


return ContainerClass;