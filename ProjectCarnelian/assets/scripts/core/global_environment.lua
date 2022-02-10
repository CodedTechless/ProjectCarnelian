

-- just to really magnify the fact that these are global scope!

_G.Scenes = {};
_G.ComponentTypes = {};

function RegisterScene(LinkedScene)
	Scenes[LinkedScene] = SceneBinding.new(LinkedScene);
end

function DeregisterScene(LinkedScene)
	Scenes[LinkedScene] = nil
end	

function GetEntityBinding(LinkedScene, LightEntityID)
	for _, Ent in ipairs(LinkedScene.Entities) do
		if Ent.ID == LinkedEntityID then
			return Ent;
		end
	end
end

function RegisterEntity(LinkedScene, LightEntity)
	local LuaEntity = EntityBinding.new(self, LightEntity);
	table.insert(LinkedScene.Entities, LuaEntity);

	print("Registered Entity" .. LightEntity.ID)
end

function DeregisterEntity(LinkedScene, LinkedEntityID)
	for i, Ent in ipairs(LinkedScene.Entities) do
		if Ent.ID == LinkedEntityID then
			table.remove(LinkedScene.Entities, i);
			
			break;
		end
	end

	for i, ComponentSet in ipairs(LinkedScene.Components) do
		if ComponentSet[LinkedEntityID] then
			ComponentSet[LinkedEntityID] = nil;

			break;
		end
	end

	print("Removed " .. LinkedEntityID);
end

function RegisterComponent(LinkedScene, LinkedEntityID, InternalComponentName, LinkedComponent)
	local ComponentName = GetComponentType(InternalComponentName);

	if not LinkedScene.Components[ComponentName] then
		LinkedScene.Components[ComponentName] = {};
	end

	LinkedScene.Components[ComponentName][LinkedEntityID] = LinkedComponent;

	print("Registered Component " + ComponentName + " to " + LinkedEntityID)
end

function DeregisterComponent(LinkedEntityID, InternalComponentName)
	local ComponentName = GetComponentType(InternalComponentName);

	if not self.Components[ComponentName] then
		warn("Entity " + LinkedEntityID + " with component of type " + ComponentName + " does not exist.");
	end

	self.Components[ComponentName][LinkedEntityID] = nil;

	print("Deregistered Component " + ComponentName + " from " + LinkedEntityID)
end

function RegisterComponentType(InternalTypeName, TypeName)
	ComponentTypes[InternalTypeName] = TypeName; -- allows us to convert from c++ typeid(T).name() type-names to internal Lua type-names.
end

function GetComponentType(InternalTypeName)
	return ComponentTypes[InternalTypeName];
end