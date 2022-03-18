#pragma once

#define M_PI 3.14159265358979323846   // pi

#define MaxEntities 5000
#define TYPEID_STRING(Type) std::string(typeid(Type).name())

#define _USE_MATH_DEFINES
#include <cmath>

#include <memory>
#include <filesystem>

#include <functional>

#include <vector>
#include <assert.h>

#include <initializer_list>

#include <sstream>
#include <fstream>
#include <iostream>

#include <string>
#include <unordered_map>

#include <array>

#include <ctime>

#include <glm/glm.hpp>
#include <json/json.hpp>

namespace fs = std::filesystem;
using JSON = nlohmann::json;

namespace Techless
{
	// shorteners!! because i'm a typical c++ programmer.

	using uint = unsigned int;

	// neat vector types for my monke brain to make it easier to handle :))

	using Vector2 = glm::vec<2, float>;
	using Vector3 = glm::vec<3, float>;
	using Vector4 = glm::vec<4, float>;
	
	using Vector2i = glm::vec<2, int>;
	using Vector3i = glm::vec<3, int>;
	using Vector4i = glm::vec<4, int>;

	using Vector2u = glm::vec<2, uint>;
	using Vector3u = glm::vec<3, uint>;
	using Vector4u = glm::vec<4, uint>;

	using Colour = Vector4;

	using Mat4x4 = glm::mat<4, 4, glm::f32, glm::defaultp>;

	// extras

	struct ZPlane
	{
		float Near = -100.f;
		float Far = 100.f;
	};

	struct Viewport
	{
		Vector2 Position = { 0.f, 0.f };
		Vector2 Size = { 0.f, 0.f };
	};

	// json util

	namespace JSONUtil
	{
		Vector2 JSONToVec2(const JSON& json);
		Vector3 JSONToVec3(const JSON& json);
		Vector4 JSONToVec4(const JSON& json);
		Colour JSONToColour(const JSON& json);

		JSON Vec2ToJSON(const Vector2& vec);
		JSON Vec3ToJSON(const Vector3& vec);
		JSON Vec4ToJSON(const Vector4& vec);
		JSON ColourToJSON(const Colour& vec);
	}

	// neat smart pointers (might change this because idk!)

	template <typename T>
	using Ptr = std::shared_ptr<T>;

	template <typename T>
	using UPtr = std::unique_ptr<T>;

	template <typename T, typename ...Args>
	constexpr Ptr<T> CreatePtr(Args&& ...args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template <typename T, typename ...Args>
	constexpr UPtr<T> CreateUPtr(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T, typename U>
	constexpr Ptr<T> CastPtr(const U& u)
	{
		return std::static_pointer_cast<T>(u);
	}

	template <typename T>
	constexpr int sign(T val) {
		return (T(0) < val) - (val < T(0));
	}
}
