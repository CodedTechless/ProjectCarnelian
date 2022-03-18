

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
	if self.Components[ComponentName] then
		return self.Components[ComponentName][LinkedEntityID];
	end
end

return SceneBinding;