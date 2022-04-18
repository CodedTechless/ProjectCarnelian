#pragma once

#include "components_base.h"
#include "components_phys.h"

namespace Techless
{

	struct UDim
	{
		UDim(float scale, float offset)
			: Offset(offset), Scale(scale) {};

		float Scale = 0.f;
		float Offset = 0.f;
	};

	struct UDim2
	{
		UDim2(UDim UDimX, UDim UDimY)
			: X(UDimX), Y(UDimY) {};

		UDim2(float scaleX, float offsetX, float scaleY, float offsetY)
			: X({ scaleX, offsetX }), Y({ scaleY, offsetY }) {};

		UDim X = { 0.f, 0.f };
		UDim Y = { 0.f, 0.f };
	};

	struct UITransformComponent : public BaseComponent
	{
		UITransformComponent() = default;
		UITransformComponent(UITransformComponent& UITransformComponent) = default;

		UDim2 GetPosition()
		{

		}

		UDim2 GetSize()
		{

		}

		void SetPosition(UDim2& Position)
		{

		}

		void SetSize(UDim2& Size)
		{

		}


	private:
		UDim2 Position = { 0.f, 0.f, 0.f, 0.f };
		UDim2 Size = { 0.f, 0.f, 0.f, 0.f };

		Vector2 AnchorPoint = { 0.f, 0.f }; // where 0, 0 is top left

	};

	struct UIImageComponent : public BaseComponent
	{

	};

	struct UIButtonComponent : public BaseComponent
	{

	};

	struct UITextComponent : public BaseComponent
	{

	};

}