

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
	if ComponentName == "TagComponent" then
		return LinkedEntity.QueryComponent_Tag(ChangeQueryMode);
	
	elseif ComponentName == "TransformComponent" then
		return LinkedEntity.QueryComponent_Transform(ChangeQueryMode);
	
	elseif ComponentName == "RigidBodyComponent" then
		return LinkedEntity.QueryComponent_RigidBody(ChangeQueryMode);
		
	elseif ComponentName == "SpriteComponent" then
		return LinkedEntity.QueryComponent_Sprite(ChangeQueryMode);
		
	elseif ComponentName == "CameraComponent" then
		return LinkedEntity.QueryComponent_Camera(ChangeQueryMode);
		
	end

	error("Invalid component name used in Scene:ChangeComponent: " .. ComponentName)
end

function SceneBinding:GetComponent(LinkedEntityID, ComponentName)
	if not self.Components[ComponentName] or not self.Components[ComponentName][LinkedEntityID] then
		error("Entity " .. LinkedEntityID .. " does not have " .. ComponentName);
		return
	end

	return self.Components[ComponentName][LinkedEntityID]
end

return SceneBinding;