

function inspect(table)
	print(table, "{");
	for k,v in pairs(table) do
		if type(v) == "string" or type(v) == "number" or type(v) == "table" then
			print("	[".. k .."] :", v)
		elseif type(v) == "boolean" then
			print("	[" .. k .. "] : " .. (v and "true" or "false"));
		else
			print("	[".. k .."] : (" .. type(v) .. ")")
		end
	end
	print("}");
end

function sign(value)
	return (value > 0 and 1) or (value < 0 and -1) or 0
end

function bool_tonumber(bool)
	return bool and 1 or 0
end

--[[

to-do: 
 - implement print/warn/error replacements to work with console.
 - make working in-engine console!

function print(...)
	local PrintArgs = {...};


end
]]

--_G.Layers = {};
_G.Scenes = {};

_G.ComponentTypes = {};

_G.CurrentSceneAddress = 0;
--_G.CurrentLayerAddress = 0;

--[[
function RegisterLayer(LightLayer)
	CurrentLayerAddress = CurrentLayerAddress + 1;

	Layers[CurrentLayerAddress] = LayerBinding.new(LightLayer)
	return CurrentLayerAddress;
end]]

function GetSceneBinding(SceneAddress)
	return Scenes[SceneAddress]
end

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

	return BindedScene:GetEntityByID(LightEntityID);
end

function RegisterEntity(SceneAddress, LightEntity)
	local BindedScene = Scenes[SceneAddress]
	BindedScene:RegisterEntity(LightEntity);
end

function DeregisterEntity(SceneAddress, LinkedEntityID)
	local BindedScene = Scenes[SceneAddress]
--	print("DeregisterEntity", SceneAddress, BindedScene)

	local _, i = BindedScene:GetEntityByID(LinkedEntityID)
	if i then table.remove(BindedScene.Entities, i); end

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