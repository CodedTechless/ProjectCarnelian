#pragma once

#include <glm/glm.hpp>

namespace Techless
{

	class Colour
	{
		Colour() = default;

		Colour(float r, float g, float b, float a = 1.f)
			: R(r), G(g), B(b), A(a) {};

		Colour(int r, int g, int b, int a = 255)
			: R((float)r / 255.f), G((float)g / 255.f), B((float)b / 255.f), A((float)a / 255.f) {};

		float R = 0.f;
		float G = 0.f;
		float B = 0.f;
		float A = 1.f;

		inline void SetRGBColour(int r, int g, int b) {
			R = (float)r / 255.f;
			G = (float)g / 255.f;
			B = (float)b / 255.f;
		}

		operator glm::vec4() const { return glm::vec4(R, G, B, A); };
		operator glm::vec3() const { return glm::vec3(R, G, B); };

		// Returns a new colour where each component is multiplied by each other component.
		Colour operator*(Colour colour) const { return Colour(R * colour.R, G * colour.G, B * colour.B, A * colour.A); };
	};

}