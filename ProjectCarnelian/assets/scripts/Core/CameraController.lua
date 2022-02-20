

-- Settings
FreeCamera = false;
local Subject = nil;

-- Movement
CameraSpeed = 15;

local PositionTo = Vector3.new();
local Offset = Vector2.new();

-- Zooming
ZoomLevel = 1;
ZoomLevelTo = 1;
ZoomSpeed = 0.1;

-- Camera-related
Near = -100;
Far = 100;

function SetSubject(snap, subject, offset)
	if snap then
		local Transform = GetComponent("TransformComponent");
	
		Transform.Position = subject.GetComponent("TransformComponent").Position + Vector3.new(offset, 0);
		PositionTo = Transform.Position;
	end

	Offset = offset;
	Subject = subject;
end

function OnCreated()
	PositionTo = GetComponent("TransformComponent").Position;
end

function OnUpdate(Delta)
	local Transform = GetComponent("TransformComponent");
	Transform.Position = Transform.Position + (PositionTo - Transform.Position) * 0.2 * Delta;

	local Camera = GetComponent("CameraComponent");
	Camera:SetProjection(Window.Size * ZoomLevel, Near, Far);

	ZoomLevel = ZoomLevel + (ZoomLevelTo - ZoomLevel) * 0.3 * Delta;
end

function OnFixedUpdate(Delta)
	if FreeCamera then
		local Horizontal = bool_tonumber(Input.KeyDown(KeyCode.D)) - bool_tonumber(Input.KeyDown(KeyCode.A));
		local Vertical = bool_tonumber(Input.KeyDown(KeyCode.S)) - bool_tonumber(Input.KeyDown(KeyCode.W));

		PositionTo = PositionTo + Vector3.new(Offset.X, Offset.Y, 0) + Vector3.new(Horizontal, Vertical, 0) * CameraSpeed * ZoomLevel * Delta;
	elseif Subject then
		PositionTo = Subject.GetComponent("TransformComponent").GlobalPosition + Vector3.new(Offset.X, Offset.Y, 0);
	end
end

function OnInputEvent(InputEvent, Processed)
	if InputEvent.InputType == InputType.Scrolling then
		ZoomLevelTo = ZoomLevelTo + (-InputEvent.Delta.Y * ZoomSpeed * ZoomLevelTo);

		return InputFilter.Continue;
	end

	return InputFilter.Ignore;
end

function OnWindowEvent(WindowEvent)
	local Camera = GetComponent("CameraComponent");
	Camera:SetProjection(WindowEvent.Size * ZoomLevel, Near, Far);
end