#pragma once

#ifdef VI_PLATFORM_WINDOWS
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
#endif

#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

//#include "Viking/Core/Base.hpp"
#include "Viking/Core/Log.hpp"

#ifdef VI_PLATFORM_WINDOWS
	#include <Windows.h>
#endif
