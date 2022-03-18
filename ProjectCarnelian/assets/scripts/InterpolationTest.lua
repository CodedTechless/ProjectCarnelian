

LastPosition = Vector3.new();
CurrentPosition = Vector3.new();

CameraSpeed = 600;

function lerp(a,b,t) 
	return a * (1-t) + b * t 
end

function OnCreated()

	local sprite = AddComponent("SpriteComponent")
	sprite.Sprite = SpriteAtlas.Get("Hand");

	local Transform = GetComponent("TransformComponent");
	LastPosition = Transform.Position;

	local Camera = Scene:GetEntityByTag("Camera");
	Camera.SetSubject(true, self, Vector2.new(0, -16));
end



function OnFixedUpdate(Delta)
	
	local Horizontal = bool_tonumber(Input.KeyDown(Enum.KeyCode.Right)) - bool_tonumber(Input.KeyDown(Enum.KeyCode.Left));
	local Vertical = bool_tonumber(Input.KeyDown(Enum.KeyCode.Down)) - bool_tonumber(Input.KeyDown(Enum.KeyCode.Up));


	LastPosition = CurrentPosition;
	CurrentPosition = CurrentPosition + Vector3.new(Horizontal, Vertical, 0) * CameraSpeed * Delta;

end

function OnUpdate(Delta)
	local SimRatio = Application.SimulationRatio;


	local Transform = GetComponent("TransformComponent");
	Transform.Position = Vector3.new(lerp(LastPosition.X, CurrentPosition.X, SimRatio), lerp(LastPosition.Y, CurrentPosition.Y, SimRatio), 60);

end