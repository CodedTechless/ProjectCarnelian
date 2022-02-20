

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

to-do: (list)
 - implement print/warn/error replacements to work with console.
 - make working in-engine console!

function print(...)
	local PrintArgs = {...};


end
]]

_G.Scenes = {};
_G.CurrentSceneAddress = 0;

_G.ComponentTypes = {};

function RegisterScene(LightScene)
	CurrentSceneAddress = CurrentSceneAddress + 1;

	Scenes[CurrentSceneAddress] = SceneBinding.new(LightScene);

	return CurrentSceneAddress;
end

function DeregisterScene(SceneID)
	Scenes[SceneID] = nil
end	

function GetSceneBinding(SceneID)
	return Scenes[SceneID]
end

function CallScene(SceneID, FuncName, ...)
    local BindedScene = Scenes[SceneID];

    if BindedScene[FuncName] then
        return BindedScene[FuncName](BindedScene, ...);
    else
        cerror("Attempted to run CallScene on " .. tostring(SceneID) .. " but function " .. FuncName .. " doesn't exist.");
    end
end


-- Entities

function GetEntityBinding(SceneID, LightEntityID)
	local BindedScene = Scenes[SceneID]

	return BindedScene:GetEntityByID(LightEntityID);
end

function RegisterEntity(SceneID, LightEntity)
	local BindedScene = Scenes[SceneID]
	BindedScene:RegisterEntity(LightEntity);
end

function DeregisterEntity(SceneID, LinkedEntityID)
	local BindedScene = Scenes[SceneID]

	local _, i = BindedScene:GetEntityByID(LinkedEntityID)
	if i then table.remove(BindedScene.Entities, i); end

	for _, ComponentSet in ipairs(BindedScene.Components) do
		if ComponentSet[LinkedEntityID] then
			ComponentSet[LinkedEntityID] = nil;

			break;
		end
	end
end

function ResetEntity(SceneID, LightEntity)
	local BindedScene = Scenes[SceneID]

	local _, i = BindedScene:GetEntityByID(LightEntity)
	if i then table.remove(BindedScene.Entities, i); end

    BindedScene:RegisterEntity(LightEntity);
end


-- Components

function RegisterComponent(SceneID, LinkedEntityID, InternalComponentName, LinkedComponent)
	local BindedScene = Scenes[SceneID]

	local ComponentName = GetComponentType(InternalComponentName);

	if not BindedScene.Components[ComponentName] then
		BindedScene.Components[ComponentName] = {};
	end

	BindedScene.Components[ComponentName][LinkedEntityID] = LinkedComponent;
end

function DeregisterComponent(SceneID, LinkedEntityID, InternalComponentName)
	local BindedScene = Scenes[SceneID]

	local ComponentName = GetComponentType(InternalComponentName);

	if not BindedScene.Components[ComponentName] then
		cerror("Entity " .. LinkedEntityID .. " with component of type " .. ComponentName .. " does not exist.");
	end

	BindedScene.Components[ComponentName][LinkedEntityID] = nil;
end

function RegisterComponentType(InternalTypeName, TypeName)
	ComponentTypes[InternalTypeName] = TypeName; -- allows us to convert from c++ typeid(T).name() type-names to internal Lua type-names.
end

function GetComponentType(InternalTypeName)
	if not ComponentTypes[InternalTypeName] then
		cerror(InternalTypeName .. " is not a registered component type name");
	end

	return ComponentTypes[InternalTypeName];
end