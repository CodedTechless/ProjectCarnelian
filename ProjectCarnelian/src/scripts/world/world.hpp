#pragma once

#include <Engine.h>

using namespace Techless;

namespace NativeScript
{

	class World : public ScriptableEntity
	{
	public:

		Ptr<Sprite> Grass = nullptr;

		void OnCreate()
		{
			Grass = SpriteAtlas::Get("Grass");
		}

		void OnUpdate(const float Delta)
		{
			Mat4x4 Transform{ 1.f };

			Transform *= glm::translate(Mat4x4(1.f), Vector3(0.f, 0.f, -99.f));

			for (int iY = 0; iY < 32; iY++)
			{
				Transform *= glm::translate(Mat4x4(1.f), Vector3(0.f, 24.f, 0.f));

				for (int iX = 0; iX < 32; iX++)
				{
					Transform *= glm::translate(Mat4x4(1.f), Vector3(24.f, 0.f, 0.f));

					Renderer::DrawSpriteExt(Grass, Transform);
				}

				Transform *= glm::translate(Mat4x4(1.f), Vector3(-24.f * 32.f, 0.f, 0.f));
			}

		}
	};

}