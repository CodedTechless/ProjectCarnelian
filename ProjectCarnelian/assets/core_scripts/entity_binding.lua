

local EntityBinding = {}
EntityBinding.__index = EntityBinding;

function EntityBinding.new(Scene, LightEntity) -- LightEntity is a C++ through-type. It has to be converted to LuaScriptableEntity.
	local self = setmetatable({}, EntityBinding);

	self.LinkedEntity = LuaScriptableEntity.new(LightEntity); -- wrapped type
	self.ID = LightEntity.ID;

	self.Scene = Scene.LinkedScene;

	return self;
end

function EntityBinding:AddComponent(ComponentName)
	return self.Scene:ChangeComponent(self.LinkedEntity, ComponentName, QueryMode.Add);
end

function EntityBinding:RemoveComponent(ComponentName)
	self.Scene:ChangeComponent(self.LinkedEntity, ComponentName, QueryMode.Remove);
end

function EntityBinding:GetComponent(ComponentName)
	return self.Scene:GetComponent(self.ID, ComponentName);
end

function EntityBinding:HasComponent(ComponentName)
	return self.Scene:GetComponent(self.ID, ComponentName) ~= nil;
end


return EntityBinding;