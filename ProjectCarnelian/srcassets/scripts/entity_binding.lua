

local EntityBinding = {}

function EntityBinding.new(Scene, LinkedEntity) -- LightEntity is a C++ through-type. It has to be converted to LuaScriptableEntity.
	local self = {};

	self.LinkedEntity = LinkedEntity; -- wrapped type
	self.ID = LinkedEntity.ID;

	self.Parent = nil;
	self.Children = {};

	self.Scene = Scene;

	self.self = self;

	function self.Destroy()
		self.LinkedEntity:GetLightEntity():Destroy();
	end

	function self.AddComponent(ComponentName)
		return self.Scene:ChangeComponent(self.LinkedEntity, ComponentName, Enum.QueryMode.Add);
	end

	function self.RemoveComponent(ComponentName)
		self.Scene:ChangeComponent(self.LinkedEntity, ComponentName, Enum.QueryMode.Remove);
	end

	function self.GetComponent(ComponentName)
		return self.Scene:GetComponent(self.ID, ComponentName);
	end

	function self.HasComponent(ComponentName)
		return self.Scene:GetComponent(self.ID, ComponentName) ~= nil;
	end

	function self.HasLuaScript()
		return self.HasComponent("LuaScriptComponent");
	end

	function self.OnCreated() end
	function self.OnDestroy() end
	function self.OnFixedUpdate(Delta) end
	function self.OnUpdate(Delta) end
	function self.OnInputEvent(InputEvent, Processed) return Enum.InputFilter.Ignore; end
	function self.OnWindowEvent(WindowEvent) end

	function self.GetChildByTag(Name)
		for _, v in next, self.Children do
			local Tag = v.GetComponent("TagComponent");
		
			if Tag.Name == Name then
				return v;
			end
		end
	end

	return self;
end

return EntityBinding;