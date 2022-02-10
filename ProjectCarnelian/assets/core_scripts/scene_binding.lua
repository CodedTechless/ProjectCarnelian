

local SceneBinding = {}
SceneBinding.__index = SceneBinding;

function SceneBinding.new(LinkedScene)
	local self = setmetatable({}, SceneBinding);

	self.Entities = {};
	self.Components = {};
	self.Registry = {};

	self.LinkedScene = LinkedScene;

	return self;
end

function SceneBinding:CreateEntity(Tag)
	return self.LinkedScene:CreateEntity(Tag);
end

function SceneBinding:ChangeComponent(LinkedEntity, ComponentName, ChangeQueryMode)
	return LinkedEntity.QueryComponent(ComponentName, ChangeQueryMode);
end

function SceneBinding:GetComponent(LinkedEntityID, ComponentName)
	if not self.Components[ComponentName] or not self.Components[ComponentName][LinkedEntityID] then
		error("Entity " .. LinkedEntityID .. " does not have " .. ComponentName);
		return
	end

	return self.Components[ComponentName][LinkedEntityID]
end

return SceneBinding;