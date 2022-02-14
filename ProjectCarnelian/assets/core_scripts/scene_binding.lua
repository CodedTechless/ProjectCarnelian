

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

function SceneBinding:RegisterEntity(LightEntity)
	local ExistingEntity = self:GetEntityByID(LightEntity.ID)
	if ExistingEntity then
		--cprint("found an existing entity for " .. LightEntity.ID .. " so returned that instead");
		return ExistingEntity;
	end	

	local NewEntity = EntityBinding.new(self, LuaScriptableEntity.new(LightEntity));
	table.insert(self.Entities, NewEntity);

	--cprint("Registered " .. LightEntity.ID .. "!");

	return NewEntity;
end

function SceneBinding:CreateEntity(Tag)
	local LightEntity = self.LinkedScene:CreateEntity(Tag);
	return self:RegisterEntity(LightEntity);
end

function SceneBinding:GetEntityByTag(String)
	for i, v in next, self.Components["TagComponent"] do
		if v.Name == String then
			return self:GetEntityByID(i);
		end
	end
end

function SceneBinding:GetEntityByID(ID)
	for i, Ent in next, self.Entities do
		if Ent.ID == ID then
			return Ent, i;
		end
	end
end

function SceneBinding:SetActiveCamera(BindedEntity)
	self.LinkedScene:SetActiveCamera(BindedEntity.LinkedEntity:GetLightEntity());
end

function SceneBinding:Instantiate(PrefabObject)
	local LightEntity = self.LinkedScene:Instantiate(PrefabObject);
	return self:RegisterEntity(LightEntity);
end

function SceneBinding:ChangeComponent(LinkedEntity, ComponentName, ChangeQueryMode)
	return LinkedEntity:QueryComponent(ComponentName, ChangeQueryMode);
end

function SceneBinding:GetComponent(LinkedEntityID, ComponentName)
	if not self.Components[ComponentName] or not self.Components[ComponentName][LinkedEntityID] then
		error("Entity " .. LinkedEntityID .. " does not have " .. ComponentName);
		return
	end

	return self.Components[ComponentName][LinkedEntityID]
end

return SceneBinding;