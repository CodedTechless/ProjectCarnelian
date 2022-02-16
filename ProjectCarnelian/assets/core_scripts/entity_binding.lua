

local EntityBinding = {}

function EntityBinding.new(Scene, LinkedEntity) -- LightEntity is a C++ through-type. It has to be converted to LuaScriptableEntity.
	local self = {};

	self.LinkedEntity = LinkedEntity; -- wrapped type
	self.ID = LinkedEntity.ID;

	self.Scene = Scene;

	self.self = self;

	function self.AddComponent(ComponentName)
		return self.Scene:ChangeComponent(self.LinkedEntity, ComponentName, QueryMode.Add);
	end

	function self.RemoveComponent(ComponentName)
		self.Scene:ChangeComponent(self.LinkedEntity, ComponentName, QueryMode.Remove);
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
	function self.OnInputEvent(InputEvent, Processed) return InputFilter.Ignore; end
	function self.OnWindowEvent(WindowEvent) end


	return self;
end

return EntityBinding;