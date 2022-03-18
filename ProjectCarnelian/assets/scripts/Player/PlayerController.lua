

WalkSpeedMax = 125;
WalkAccel = WalkSpeedMax * 12;
WalkFriction = WalkAccel * 1.2;

Velocity = Vector2.new();

local MoveDirectionLast = Vector2.new();
MoveDirection = Vector2.new();

local MoveInputAngle = 0;
local MoveInputAngleLast = 0;
local MoveDirectionVector = Vector2.new();

-- Physics Math (to be moved to engine) --

local function Approach(Value, Approach, Amount)
	if Value > Approach then
		Value = Value - Amount;
		return math.max(Value, Approach);
	else
		Value = Value + Amount;
		return math.min(Value, Approach);
	end
end

local function CalculateFriction(Direction, OldComponent, MaxSpeed)
	if Direction == 0 then
		OldComponent = Approach(OldComponent, 0, WalkFriction);
	elseif math.abs(OldComponent) > math.abs(MaxSpeed) then
		OldComponent = Approach(OldComponent, MaxSpeed * sign(OldComponent), WalkFriction * 3);
	end

	return OldComponent;
end

-- Inputs --

local function ProcessMovementInputs()

	local KeyUp = Input.KeyDown(Enum.KeyCode.W);
	local KeyDown = Input.KeyDown(Enum.KeyCode.S);
	local KeyLeft = Input.KeyDown(Enum.KeyCode.A);
	local KeyRight = Input.KeyDown(Enum.KeyCode.D);

	MoveDirection = Vector2.new(bool_tonumber(KeyRight) - bool_tonumber(KeyLeft), bool_tonumber(KeyDown) - bool_tonumber(KeyUp));
	
end

-- Visuals --

local function UpdatePose()
	local Legs = GetChildByTag("Legs");
	local LegsAnim = Legs.GetComponent("SpriteAnimatorComponent");

	if MoveDirection:Magnitude() > 0 then
		if not LegsAnim:IsPlaying("Walking") then
			LegsAnim:Play("Walking");

			if math.random(100) > 50 then
				LegsAnim.Frame = 1
			end
		end
	else
		if not LegsAnim:IsPlaying("Idle") then
			LegsAnim:Play("Idle");
		end
	end
	
	local Body = GetChildByTag("Body")
	local BodyAnim = Body.GetComponent("SpriteAnimatorComponent")

	local Face = Body.GetChildByTag("FaceAttachment");
	local Transform = Face.GetComponent("TransformComponent");

	local RightEye = Face.GetChildByTag("RightEye");
	local LeftEye = Face.GetChildByTag("LeftEye");

	local RightEyeSprite = RightEye.GetComponent("SpriteComponent");
	local LeftEyeSprite = LeftEye.GetComponent("SpriteComponent");

	LeftEyeSprite.Visible = true;
	RightEyeSprite.Visible = true;
	
	local FinalEyePosition = Vector2.new();

	if MoveInputAngleLast > -math.pi * 0.125 and MoveInputAngleLast <= math.pi * 0.125 then
		FinalEyePosition = Vector2.new(2, -2);
	elseif MoveInputAngleLast > math.pi * 0.125 and MoveInputAngleLast <= math.pi * 0.375 then
		FinalEyePosition = Vector2.new(1.5, -1);
	elseif MoveInputAngleLast > math.pi * 0.375 and MoveInputAngleLast <= math.pi * 0.625 then
		FinalEyePosition = Vector2.new();
	elseif MoveInputAngleLast > math.pi * 0.625 and MoveInputAngleLast <= math.pi * 0.875 then
		FinalEyePosition = Vector2.new(-1.5, -1);
	elseif MoveInputAngleLast > math.pi * 0.875 or MoveInputAngleLast <= -math.pi * 0.875 then
		FinalEyePosition = Vector2.new(-2, -2);
	elseif MoveInputAngleLast > -math.pi * 0.875 and MoveInputAngleLast <= -math.pi * 0.625 then
		FinalEyePosition = Vector2.new(-10, -3);
		RightEyeSprite.Visible = false;
	elseif MoveInputAngleLast > -math.pi * 0.625 and MoveInputAngleLast <= -math.pi * 0.375 then
		LeftEyeSprite.Visible = false;
		RightEyeSprite.Visible = false;
	elseif MoveInputAngleLast > -math.pi * 0.375 and MoveInputAngleLast <= -math.pi * 0.125 then
		FinalEyePosition = Vector2.new(10, -3);
		LeftEyeSprite.Visible = false;
	end

	Transform.Position = Vector3.new(FinalEyePosition.X, FinalEyePosition.Y + ((BodyAnim.Frame == 1) and 1 or 0), Transform.Position.Z);
end

-- Work functions --

function OnCreated()
	local Transform = GetComponent("TransformComponent");
	Transform:SetEngineInterpolationEnabled(true);
	Transform.Position = Vector3.new(0, 0, 50);

	local Camera = Scene:GetEntityByTag("Camera");
	Camera.SetSubject(true, self, Vector2.new(0, -16));
end

function OnUpdate(Delta)
	UpdatePose();
end

function OnFixedUpdate(Delta)
	ProcessMovementInputs();

	if MoveDirectionLast.X ~= MoveDirection.X or MoveDirectionLast.Y ~= MoveDirection.Y then
		MoveInputAngle = math.atan2(MoveDirection.Y, MoveDirection.X);

		if MoveDirection.X ~= 0 or MoveDirection.Y ~= 0 then
			MoveInputAngleLast = MoveInputAngle;
		end

		MoveDirectionVector = Vector2.new(math.cos(MoveInputAngle), math.sin(MoveInputAngle)):Abs() * MoveDirection;
		MoveDirectionLast = MoveDirection;
	end

	Velocity = Velocity + WalkAccel * MoveDirectionVector;

	Velocity.X = CalculateFriction(MoveDirection.X, Velocity.X, WalkSpeedMax * math.abs(MoveDirectionVector.X));
	Velocity.Y = CalculateFriction(MoveDirection.Y, Velocity.Y, WalkSpeedMax * math.abs(MoveDirectionVector.Y));

	if math.abs(Velocity.X) > 0 or math.abs(Velocity.Y) > 0 then
		local Transform = GetComponent("TransformComponent");
		Transform.Position = Transform.Position + Vector3.new(Velocity.X, Velocity.Y, 0) * Delta;
	end
end

function OnInputEvent(InputEvent)
	if InputEvent.InputType == Enum.InputType.Mouse and InputEvent.MouseCode == Enum.MouseCode.Button0 and InputEvent.InputState == Enum.InputState.Begin then
		local Camera = Scene:GetEntityByTag("Camera");
		local CameraComp = Camera.GetComponent("CameraComponent");

		print(InputEvent.Position.X, InputEvent.Position.Y);
		local pos = CameraComp:ScreenToWorldCoordinates(InputEvent.Position);
		print(pos.X, pos.Y);
	end
end