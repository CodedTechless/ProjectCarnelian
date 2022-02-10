

-- just to really magnify the fact that these are global scope!

Scenes = {};
ComponentTypes = {};

CurrentSceneAddress = 0;

function RegisterScene(LightScene)
	CurrentSceneAddress = CurrentSceneAddress + 1;

	Scenes[CurrentSceneAddress] = SceneBinding.new(LightScene);
	return CurrentSceneAddress;
end

function DeregisterScene(SceneAddress)
	Scenes[SceneAddress] = nil
end	

function GetEntityBinding(SceneAddress, LightEntityID)
	local BindedScene = Scenes[SceneAddress]
	print("GetEntityBinding", SceneAddress, BindedScene)

	for _, Ent in ipairs(BindedScene.Entities) do
		if Ent.ID == LinkedEntityID then
			return Ent;
		end
	end
end

function RegisterEntity(SceneAddress, LightEntity)
	local BindedScene = Scenes[SceneAddress]
	print("RegisterEntity", SceneAddress, BindedScene)

	local LuaEntity = EntityBinding.new(BindedScene, LightEntity);
	table.insert(BindedScene.Entities, LuaEntity);

	print("Registered Entity", LightEntity.ID)
end

function DeregisterEntity(SceneAddress, LinkedEntityID)
	local BindedScene = Scenes[SceneAddress]
	print("DeregisterEntity", SceneAddress, BindedScene)


	for i, Ent in ipairs(BindedScene.Entities) do
		if Ent.ID == LinkedEntityID then
			table.remove(BindedScene.Entities, i);
			
			break;
		end
	end

	for i, ComponentSet in ipairs(BindedScene.Components) do
		if ComponentSet[LinkedEntityID] then
			ComponentSet[LinkedEntityID] = nil;

			break;
		end
	end

	print("Removed " .. LinkedEntityID);
end

function RegisterComponent(SceneAddress, LinkedEntityID, InternalComponentName, LinkedComponent)
	local BindedScene = Scenes[SceneAddress]
	print("RegisterComponent", SceneAddress, BindedScene)

	local ComponentName = GetComponentType(InternalComponentName);

	if not BindedScene.Components[ComponentName] then
		BindedScene.Components[ComponentName] = {};
	end

	BindedScene.Components[ComponentName][LinkedEntityID] = LinkedComponent;

	print("Registered Component " + ComponentName + " to " + LinkedEntityID)
end

function DeregisterComponent(SceneAddress, LinkedEntityID, InternalComponentName)
	local BindedScene = Scenes[SceneAddress]
	print("DeregisterComponent", SceneAddress, BindedScene)


	local ComponentName = GetComponentType(InternalComponentName);

	if not BindedScene.Components[ComponentName] then
		warn("Entity " + LinkedEntityID + " with component of type " + ComponentName + " does not exist.");
	end

	BindedScene.Components[ComponentName][LinkedEntityID] = nil;

	print("Deregistered Component " + ComponentName + " from " + LinkedEntityID)
end

function RegisterComponentType(InternalTypeName, TypeName)
	ComponentTypes[InternalTypeName] = TypeName; -- allows us to convert from c++ typeid(T).name() type-names to internal Lua type-names.
end

function GetComponentType(InternalTypeName)
	return ComponentTypes[InternalTypeName];
end