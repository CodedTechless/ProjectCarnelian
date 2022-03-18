

local ModelRiseLevel = 0;

function OnCreated()
	local Transform = GetComponent("TransformComponent");
	Transform:SetEngineInterpolationEnabled(true);
end


function OnUpdate(Delta)
	local Model = GetChildByTag("Model");
	local ModelTransform = Model.GetComponent("TransformComponent");

	local sinLevel = math.sin(ModelRiseLevel);
	ModelTransform.Position = Vector3.new(0, -50 + sinLevel * 3, 0);
	
	ModelRiseLevel = ModelRiseLevel + Delta;
	
	local Jets = Model.GetChildByTag("Jet");
	local JetsSprite = Jets.GetComponent("SpriteComponent");
	
	JetsSprite.Colour = Vector4.new(1, 1, 1, (-sinLevel * 0.2 + 0.8));
end