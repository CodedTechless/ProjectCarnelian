
print("I'm working!")


function OnCreated()
	print("OnCreated")

	print(ID);
	inspect(_ENV);

	local Tag = GetComponent("TagComponent");
	print(Tag.Name);

	local Transform = GetComponent("TransformComponent");
	print(Transform)
	Transform.Position = Vector3.new(Transform.Position.X + 100, Transform.Position.Y, Transform.Position.Z);

	print(ID);
	
	print(Scene);
end

function OnDestroy()
	print("OnDestroy")
end

function OnFixedUpdate(Delta)
	print("OnFixedUpdate")
end

function OnUpdate(Delta)
	print("OnUpdate")
end

function OnInputEvent(InputEvent, Processed)
	print("OnInputEvent")
end

function OnWindowEvent(WindowEvent)
	print("OnWindowEvent")
end
