#pragma once

#define _USE_MATH_DEFINES
#define MaxEntities 5000

#include <cmath>

#include <memory>
#include <filesystem>

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

	template <typename T>
	constexpr int sign(T val) {
		return (T(0) < val) - (val < T(0));
	}
}
