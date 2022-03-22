

return function(FaceAttachment, Angle)
	local RightEye = FaceAttachment.GetChildByTag("RightEye");
	local LeftEye = FaceAttachment.GetChildByTag("LeftEye");

	local RightEyeSprite = RightEye.GetComponent("SpriteComponent");
	local LeftEyeSprite = LeftEye.GetComponent("SpriteComponent");

	LeftEyeSprite.Visible = true;
	RightEyeSprite.Visible = true;

    local Position = Vector2.new();

	if Angle > -math.pi * 0.125 and Angle <= math.pi * 0.125 then
		Position = Vector2.new(2, -2);
	elseif Angle > math.pi * 0.125 and Angle <= math.pi * 0.375 then
		Position = Vector2.new(1.5, -1);
	elseif Angle > math.pi * 0.375 and Angle <= math.pi * 0.625 then
		Position = Vector2.new();
	elseif Angle > math.pi * 0.625 and Angle <= math.pi * 0.875 then
		Position = Vector2.new(-1.5, -1);
	elseif Angle > math.pi * 0.875 or Angle <= -math.pi * 0.875 then
		Position = Vector2.new(-2, -2);
	elseif Angle > -math.pi * 0.875 and Angle <= -math.pi * 0.625 then
		Position = Vector2.new(-10, -3);
		RightEyeSprite.Visible = false;
	elseif Angle > -math.pi * 0.625 and Angle <= -math.pi * 0.375 then
		LeftEyeSprite.Visible = false;
		RightEyeSprite.Visible = false;
	elseif Angle > -math.pi * 0.375 and Angle <= -math.pi * 0.125 then
		Position = Vector2.new(10, -3);
		LeftEyeSprite.Visible = false;
	end

    return Position;
end