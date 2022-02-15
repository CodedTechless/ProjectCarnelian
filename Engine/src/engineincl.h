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

namespace Techless
{
	template <typename T>
	using Ptr = std::shared_ptr<T>;

	template <typename T, typename ...Args>
	constexpr Ptr<T> CreatePtr(Args&& ...args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
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
