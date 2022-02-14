

function inspect(table)
	print(table, "{");
	for k,v in pairs(table) do
		if type(v) == "string" or type(v) == "number" or type(v) == "table" then
			print("	[".. k .."] :", v)
		else
			print("	[".. k .."] : (" .. type(v) .. ")")
		end
	end
	print("}");
end

--[[

to-do: 
 - implement print/warn/error replacements to work with console.
 - make working in-engine console!

function print(...)
	local PrintArgs = {...};


end
]]


_G.Scenes = {};
_G.ComponentTypes = {};

_G.CurrentSceneAddress = 0;

function RegisterScene(LightScene)
	CurrentSceneAddress = CurrentSceneAddress + 1;

	Scenes[CurrentSceneAddress] = SceneBinding.new(LightScene);
--	inspect(Scenes[CurrentSceneAddress]);

	return CurrentSceneAddress;
end

function DeregisterScene(SceneAddress)
	Scenes[SceneAddress] = nil
end	

function GetEntityBinding(SceneAddress, LightEntityID)
	local BindedScene = Scenes[SceneAddress]
--	print("GetEntityBinding", SceneAddress, BindedScene)

	for _, Ent in pairs(BindedScene.Entities) do
		if Ent.ID == LightEntityID then
--			print("Found", Ent, "(" .. Ent.ID .. ")");
			return Ent;
		end
	end
end

function RegisterEntity(SceneAddress, LightEntity)
	local BindedScene = Scenes[SceneAddress]
--	print("RegisterEntity", SceneAddress, BindedScene)

	local LuaEntity = EntityBinding.new(BindedScene, LuaScriptableEntity.new(LightEntity));
	table.insert(BindedScene.Entities, LuaEntity);
	
--	inspect(LuaEntity);
--	print("Registered Entity", LightEntity.ID)
end

function DeregisterEntity(SceneAddress, LinkedEntityID)
	local BindedScene = Scenes[SceneAddress]
--	print("DeregisterEntity", SceneAddress, BindedScene)

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

--	print("Removed " .. LinkedEntityID);
end

function RegisterComponent(SceneAddress, LinkedEntityID, InternalComponentName, LinkedComponent)
	local BindedScene = Scenes[SceneAddress]
--	print("RegisterComponent", SceneAddress, BindedScene)

	local ComponentName = GetComponentType(InternalComponentName);

	if not BindedScene.Components[ComponentName] then
		BindedScene.Components[ComponentName] = {};
	end

	BindedScene.Components[ComponentName][LinkedEntityID] = LinkedComponent;

--	print("Registered Component " .. ComponentName .. "(" .. tostring(LinkedComponent) .. ") to " .. LinkedEntityID)
end

function DeregisterComponent(SceneAddress, LinkedEntityID, InternalComponentName)
	local BindedScene = Scenes[SceneAddress]
--	print("DeregisterComponent", SceneAddress, BindedScene)


	local ComponentName = GetComponentType(InternalComponentName);

	if not BindedScene.Components[ComponentName] then
		error("Entity " .. LinkedEntityID .. " with component of type " .. ComponentName .. " does not exist.");
	end

	BindedScene.Components[ComponentName][LinkedEntityID] = nil;

--	print("Deregistered Component " .. ComponentName .. " from " .. LinkedEntityID)
end

function RegisterComponentType(InternalTypeName, TypeName)
	ComponentTypes[InternalTypeName] = TypeName; -- allows us to convert from c++ typeid(T).name() type-names to internal Lua type-names.
end

function GetComponentType(InternalTypeName)
	if not ComponentTypes[InternalTypeName] then
		error(InternalTypeName .. " is not a registered component type name");
	end

	return ComponentTypes[InternalTypeName];
end