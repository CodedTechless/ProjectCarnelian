

local SceneBinding = {}
SceneBinding.__index = SceneBinding;

function SceneBinding.new(LinkedScene)
	local self = setmetatable({}, SceneBinding);

	self.Entities = {};

	self.LinkedScene = LinkedScene;

	return self;
end

function SceneBinding:OnUpdate(Delta)
	if not self.LinkedScene.ScriptRuntimeEnabled then
		return
	end

	for _, e in next, self.Entities do
		if e.HasLuaScript() then
            e.OnUpdate(Delta);
		end
	end
end

function SceneBinding:OnFixedUpdate(Delta)
	if not self.LinkedScene.ScriptRuntimeEnabled then
		return
	end

	for _, e in next, self.Entities do
		if e.HasLuaScript() then
            e.OnFixedUpdate(Delta);
		end
	end
end

function SceneBinding:OnInputEvent(InputObject, Processed)
	local FinalFilter = Enum.InputFilter.Ignore;

	if not self.LinkedScene.ScriptRuntimeEnabled then
		return FinalFilter
	end

	for _, e in next, self.Entities do
		if e.HasLuaScript() then
            local Filter = e.OnInputEvent(InputObject, Processed);

            if Filter ~= Enum.InputFilter.Ignore then
                FinalFilter = Enum.InputFilter;
            end
    
            if Filter == Enum.InputFilter.Stop then
                break;
            elseif Filter == Enum.InputFilter.Continue then
                Processed = true;
            end
		end
	end

	return FinalFilter;
end

function SceneBinding:OnWindowEvent(WindowEventObject)
	if not self.LinkedScene.ScriptRuntimeEnabled then
		return
	end

	for _, e in next, self.Entities do
        if e.HasLuaScript() then
		    e.OnWindowEvent(WindowEventObject);
        end
	end
end

function SceneBinding:RegisterEntity(LightEntity)
	local ExistingEntity = self:GetEntityByID(LightEntity.ID)

	if ExistingEntity then
		--cprint("found an existing entity for " .. LightEntity.ID .. "! maybe it's getting updated with a new location?");
		return self.Entities[LightEntity.ID];
	end	

	local NewEntity = EntityBinding.new(self, LuaScriptableEntity.new(LightEntity));
	self.Entities[LightEntity.ID] = NewEntity

	return NewEntity;
end

function SceneBinding:DeregisterEnttiy(EntityID)
	self.Entities[EntityID] = nil;
end

function SceneBinding:CreateEntity(Tag)
	local LightEntity = self.LinkedScene:CreateEntity(Tag);
	return self:RegisterEntity(LightEntity);
end

function SceneBinding:GetEntityByTag(String)
	for _, Entity in next, self.Entities do
		local Tag = Entity.GetComponent("TagComponent");
	
		if Tag and Tag.Name == String then
			return Entity;
		end
	end
end

function SceneBinding:GetEntityByID(ID)
	return self.Entities[ID];
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

return SceneBinding;