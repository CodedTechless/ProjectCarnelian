
#include "engineincl.h"


namespace Techless::JSONUtil
{
	
	Vector2 JSONToVec2(const JSON& json)
	{
		return { json.at("X").get<float>(), json.at("Y").get<float>() };
	}

	Vector3 JSONToVec3(const JSON& json)
	{
		return { json.at("X").get<float>(), json.at("Y").get<float>(), json.at("Z").get<float>() };
	}

	Vector4 JSONToVec4(const JSON& json)
	{
		return { json.at("X").get<float>(), json.at("Y").get<float>(), json.at("Z").get<float>(), json.at("W").get<float>() };
	}

	Colour JSONToColour(const JSON& json)
	{
		return { json.at("R").get<float>(), json.at("G").get<float>(), json.at("B").get<float>(), json.at("A").get<float>() };
	}

	JSON Vec2ToJSON(const Vector2& vec)
	{
		return { {"X", vec.x}, {"Y", vec.y} };
	}

	JSON Vec3ToJSON(const Vector3& vec)
	{
		return { {"X", vec.x}, {"Y", vec.y}, {"Z", vec.z} };
	}

	JSON Vec4ToJSON(const Vector4& vec)
	{
		return { {"X", vec.x}, {"Y", vec.y}, {"Z", vec.z}, {"W", vec.z} };
	}

	JSON ColourToJSON(const Colour& vec)
	{
		return { {"R", vec.r}, {"G", vec.g}, {"B", vec.b}, {"A", vec.a} };
	}
}