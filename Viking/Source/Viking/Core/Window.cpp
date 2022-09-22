#include "vipch.hpp"
#include "Viking/Core/Window.hpp"

#ifdef VI_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsWindow.hpp"
#endif

namespace Viking {
	Scope<Window> Window::create(const WindowProps& props) {
#ifdef VI_PLATFORM_WINDOWS
		return createScope<WindowsWindow>(props);
#else
		VI_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
	}
}
