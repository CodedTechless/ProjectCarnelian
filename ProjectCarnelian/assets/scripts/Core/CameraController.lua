

-- Settings
FreeCamera = false;
local Subject = nil;

-- Movement
CameraSpeed = 600;

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
	
		Transform.Position = subject.GetComponent("TransformComponent").Position + Vector3.new(offset.X, offset.Y, 0);
		PositionTo = Transform.Position;
	end

	Offset = offset;
	Subject = subject;
end

function OnCreated()
	local Transform = GetComponent("TransformComponent")
	Transform:SetEngineInterpolationEnabled(true);

	PositionTo = Transform.Position;
end

function OnFixedUpdate(Delta)
	if FreeCamera then
		local Horizontal = bool_tonumber(Input.KeyDown(Enum.KeyCode.Right)) - bool_tonumber(Input.KeyDown(Enum.KeyCode.Left));
		local Vertical = bool_tonumber(Input.KeyDown(Enum.KeyCode.Down)) - bool_tonumber(Input.KeyDown(Enum.KeyCode.Up));

		PositionTo = PositionTo + Vector3.new(Horizontal, Vertical, 0) * CameraSpeed * ZoomLevel * Delta;
	elseif Subject then
		PositionTo = Subject.GetComponent("TransformComponent").GlobalPosition + Vector3.new(Offset.X, Offset.Y, 0);
	end

	local Transform = GetComponent("TransformComponent");
	Transform.Position = Transform.Position + ((PositionTo - Transform.Position) / 0.1) * Delta;
end

function OnUpdate(Delta)
	local Camera = GetComponent("CameraComponent");
	Camera.Size = Window.Size * ZoomLevel;

	ZoomLevel = ZoomLevel + ((ZoomLevelTo - ZoomLevel) / 0.08) * Delta;
end

function OnInputEvent(InputEvent, Processed)
	if InputEvent.InputType == Enum.InputType.Scrolling then
		ZoomLevelTo = ZoomLevelTo + (-InputEvent.Delta.Y * ZoomSpeed * ZoomLevelTo);

		return Enum.InputFilter.Continue;
	elseif InputEvent.InputType == Enum.InputType.Keyboard then
		if InputEvent.KeyCode == Enum.KeyCode.E and InputEvent.InputState == Enum.InputState.Begin then
			FreeCamera = not FreeCamera;

			cprint("Freecam " .. (FreeCamera and "Enabled" or "Disabled"));
		end
	end


	return Enum.InputFilter.Ignore;
end

function OnWindowEvent(WindowEvent)
	local Camera = GetComponent("CameraComponent");
	Camera.Size = WindowEvent.Size * ZoomLevel;
end