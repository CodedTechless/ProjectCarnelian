

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
	local Eli = Scene:Instantiate(PrefabAtlas.Get("assets/prefabs/Eli.prefab"));
--	Player.AddComponent("SpriteComponent").Sprite = SpriteAtlas.Get("Hand")

--	local test = Scene:CreateEntity("test");
--	test.AddComponent("LuaScriptComponent"):Bind("InterpolationTest");

	-- box
	local box = Scene:CreateEntity("Box");
	local boxSprite = box.AddComponent("SpriteComponent");
	boxSprite.Sprite = SpriteAtlas.Get("checkers");

	box.GetComponent("TransformComponent").Position = Vector3.new(100, 0, 0);

--	local boxScript = box.AddComponent("LuaScriptComponent");
--	boxScript:Bind("test");

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