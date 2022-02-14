

local PlayerPrefab = PrefabAtlas.Get("assets/prefabs/Player.prefab");

function OnCreated()

	-- Camera
	local Camera = Scene:CreateEntity("Camera");
	Camera.AddComponent("CameraComponent")

	local cameraScript = Camera.AddComponent("LuaScriptComponent");
	cameraScript:Bind("CameraController");
	
	Scene:SetActiveCamera(Camera);

	-- Player
	local Player = Scene:Instantiate(PlayerPrefab);

	local playerScript = Player.AddComponent("LuaScriptComponent");
	playerScript:Bind("PlayerController");

end

function OnRemoved()

end

function OnUpdate()

end

function OnFixedUpdate()

end

function OnInputEvent(InputEvent, Processed)
	return 0
end

function OnWindowEvent(WindowEvent)

end