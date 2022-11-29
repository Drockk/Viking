#pragma once

#include <memory>

#define VI_EXPAND_MACRO(x) x
#define VI_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#define VI_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

namespace Viking {

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> createScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> createRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}
