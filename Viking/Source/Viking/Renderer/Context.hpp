#pragma once

#include <GLFW/glfw3.h>

namespace Viking {
	class Context {
	public:
		Context() = default;
		virtual ~Context() = default;
		virtual void init(const std::string& name, GLFWwindow*) = 0;

		virtual void onUpdate() const = 0;

		virtual void* getInstance() = 0;
		virtual void* getSurface() = 0;

		static Ref<Context> create();
	};
}
