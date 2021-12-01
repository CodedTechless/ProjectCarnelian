#pragma once

#include <memory>

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

#define MaxEntities 5000

namespace Techless
{
	template <typename T>
	using Ptr = std::shared_ptr<T>;

	template <typename T, typename ...Args>
	constexpr Ptr<T> CreatePtr(Args&& ...args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}
