
local Container = require("Container");
local Item = require("Item");

-- Player

LinkedEli = nil
Inventory = Container.new(5);

--[[
Inventory:Add(Item.new("TestItem"), 9);
Inventory:Add(Item.new("TestItem"), 5);
Inventory:Add(Item.new("TestItem2"), 20);
Inventory:Add(Item.new("TestItem"), 15);
]]



-- Movement

MoveMaxVelocity = 125;
MoveAcceleration = MoveMaxVelocity * 8;
MoveDeceleration = MoveAcceleration * 1.3;

Velocity = Vector2.new();

MoveDirection = Vector2.new();
local MoveDirectionLast = Vector2.new();

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

local function CalculateFriction(Delta, Direction, DirectionNormal, Component, ComponentNormalised)
    local NormalisedMaxSpeed = MoveMaxVelocity * math.abs(DirectionNormal);
    local NormalisedDeceleration = MoveDeceleration * math.abs(ComponentNormalised);
    
	if Direction == 0 then
		Component = Approach(Component, 0, NormalisedDeceleration * Delta);
	elseif math.abs(Component) > NormalisedMaxSpeed then
		Component = Approach(Component, NormalisedMaxSpeed * sign(Component), NormalisedDeceleration * 3 * Delta);
	end

	return Component;
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

local EyePoses = require("EyePoses");

local function UpdatePose()
	local Model = GetChildByTag("Model")
	local Legs = Model.GetChildByTag("Legs");
	local LegsAnim = Legs.GetComponent("SpriteAnimatorComponent");

	if MoveDirection.Magnitude > 0 then
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
	
	local Body = Model.GetChildByTag("Body")
	local Face = Body.GetChildByTag("FaceAttachment");
	
	local FinalEyePosition = EyePoses(Face, MoveInputAngleLast);
    
	local BodyAnim = Body.GetComponent("SpriteAnimatorComponent");
	local Transform = Face.GetComponent("TransformComponent");
	Transform.Position = Vector3.new(FinalEyePosition.X, FinalEyePosition.Y + ((BodyAnim.Frame == 1) and 1 or 0), Transform.Position.Z);
end

-- Work functions --

function OnCreated()
	local Transform = GetComponent("TransformComponent");
	Transform:SetEngineInterpolationEnabled(true);

	local Camera = Scene:GetEntityByTag("Camera");
	Camera.SetSubject(true, self, Vector2.new(0, -16));

	local EliPrefab = PrefabAtlas.Get("assets/prefabs/Eli.prefab");
	LinkedEli = Scene:Instantiate(EliPrefab);

	LinkedEli.SetPlayerOwner(self);
end

function OnUpdate(Delta)
	UpdatePose();
end

function OnFixedUpdate(Delta)
	ProcessMovementInputs();

	if MoveDirectionLast.X ~= MoveDirection.X or MoveDirectionLast.Y ~= MoveDirection.Y then
		MoveInputAngle = math.atan(MoveDirection.Y, MoveDirection.X);

		if MoveDirection.X ~= 0 or MoveDirection.Y ~= 0 then
			MoveInputAngleLast = MoveInputAngle;
		end

		MoveDirectionVector = Vector2.new(math.cos(MoveInputAngle), math.sin(MoveInputAngle)).Abs * MoveDirection;
		MoveDirectionLast = MoveDirection;
	end

    local Acceleration = MoveAcceleration * MoveDirectionVector * Delta;
	Velocity = Velocity + Acceleration;

    local NormalisedVelocity = math.atan(Velocity.Y, Velocity.X);
    local NormalisedVelocityVector = Vector2.new(math.cos(NormalisedVelocity), math.sin(NormalisedVelocity));

	Velocity.X = CalculateFriction(Delta, MoveDirection.X, MoveDirectionVector.X, Velocity.X, NormalisedVelocityVector.X);
	Velocity.Y = CalculateFriction(Delta, MoveDirection.Y, MoveDirectionVector.Y, Velocity.Y, NormalisedVelocityVector.Y);

    local Transform = GetComponent("TransformComponent");
    Transform.Position = Vector3.new(Transform.Position.X, Transform.Position.Y, Transform.Position.Y * 0.0001) + Vector3.new(Velocity.X, Velocity.Y, 0) * Delta;

	--[[
    for _, FloorItem in pairs(Scene.Entities) do
        local FloorItemTransform = FloorItem.GetComponent("TransformComponent");

        if (Transform.Position - FloorItemTransform.GlobalPosition).Magnitude < 32 then
            local Quantity = Inventory:Add(FloorItem.ContainingItem.Item, FloorItem.ContainingItem.Quantity)
            
            if Quantity == 0 then
                FloorItem.Destroy();
            else
                FloorItem.ContainingItem.Quantity = Quantity;
            end
        end
    end]]
end

function OnInputEvent(InputEvent)
--[[
	if InputEvent.InputType == Enum.InputType.Mouse and InputEvent.MouseCode == Enum.MouseCode.Button0 and InputEvent.InputState == Enum.InputState.Begin then
		local Camera = Scene:GetEntityByTag("Camera");
		local CameraComp = Camera.GetComponent("CameraComponent");

		print(InputEvent.Position.X, InputEvent.Position.Y);
		local pos = CameraComp:ScreenToWorldCoordinates(InputEvent.Position);
		print(pos.X, pos.Y);
	end]]
end