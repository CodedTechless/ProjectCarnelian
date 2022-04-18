
local Item = require("Item");
local ContainerItem = require("ContainerItem");

function OnCreated()

    Scene.Items = {};

	-- Camera
	local Camera = Scene:CreateEntity("Camera");
	Camera.AddComponent("CameraComponent")

	local cameraScript = Camera.AddComponent("LuaScriptComponent");
	cameraScript:Bind("CameraController");
	
	Scene:SetActiveCamera(Camera);

	-- Player
    local PlayerPrefab = PrefabAtlas.Get("assets/prefabs/Player.prefab");
    local player = Scene:Instantiate(PlayerPrefab);
    player.AddComponent("BoxColliderComponent");


    local ItemPrefabTest = PrefabAtlas.Get("assets/prefabs/ItemWorld.prefab");
    local item = Scene:Instantiate(ItemPrefabTest);

    local Transform = item.GetComponent("TransformComponent");
    Transform.Position = Vector2.new(200, 200);

    item.SetContainingItem(Item.new("TestItem"), 5);
    item.AddComponent("BoxColliderComponent");

	-- box
	--[[
	
	local box = Scene:CreateEntity("Box");
	local boxSprite = box.AddComponent("SpriteComponent");
	boxSprite.Sprite = SpriteAtlas.Get("checkers");

	box.GetComponent("TransformComponent").Position = Vector3.new(100, 0, 0);
	
	local boxScript = box.AddComponent("LuaScriptComponent");
	boxScript:Bind("test");
	
	]]
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