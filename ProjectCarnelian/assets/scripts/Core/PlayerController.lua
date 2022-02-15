


WalkSpeedMax = 3;
WalkAccel = 0.75;
WalkFriction = 1.25;

Velocity = Vector2.new();



local function Approach(Value, Approach, Amount)
	if Value > Approach then
		Value = Value - Amount;
		return math.max(Value, Approach);
	else
		Value = Value + Amount;
		return math.min(Value, Approach);
	end
end

function OnCreated()
	local Transform = GetComponent("TransformComponent");
	Transform.LocalPosition.Z = 10;

	local Camera = Scene:GetEntityByTag("Camera");
	Camera.SetSubject(true, self, Vector2.new(0, -16));
end

function OnFixedUpdate(Delta)
	local Transform = GetComponent("TransformComponent");

	local KeyUp = Input.KeyDown(KeyCode.W);
	local KeyDown = Input.KeyDown(KeyCode.S);
	local KeyLeft = Input.KeyDown(KeyCode.A);
	local KeyRight = Input.KeyDown(KeyCode.D);

	local Horizontal = bool_tonumber(KeyRight) - bool_tonumber(KeyLeft);
	local Vertical = bool_tonumber(KeyDown) - bool_tonumber(KeyUp);

	local HorizontalAbs = math.abs(Horizontal);
	local VerticalAbs = math.abs(Vertical);

	local Angle = math.atan2(VerticalAbs, HorizontalAbs);

	local WalkSpeedMaxH = math.cos(Angle) * WalkSpeedMax;
	local WalkSpeedMaxV = math.sin(Angle) * WalkSpeedMax;

	Velocity = Velocity + Vector2.new(WalkAccel * Horizontal, WalkAccel * Vertical);

	if Horizontal == 0 then
		Velocity.X = Approach(Velocity.X, 0, WalkFriction);
	elseif math.abs(Velocity.X) > math.abs(WalkSpeedMaxH) then
		Velocity.X = Approach(Velocity.X, WalkSpeedMaxH * sign(Velocity.X), WalkFriction);
	end

	if Vertical == 0 then
		Velocity.Y = Approach(Velocity.Y, 0, WalkFriction);
	elseif math.abs(Velocity.Y) > math.abs(WalkSpeedMaxV) then
		Velocity.Y = Approach(Velocity.Y, WalkSpeedMaxV * sign(Velocity.Y), WalkFriction);
	end

	if math.abs(Velocity.X) > 0 or math.abs(Velocity.Y) > 0 then
		Transform.LocalPosition = Transform.LocalPosition + Vector3.new(Velocity.X, Velocity.Y, 0);
	end
end