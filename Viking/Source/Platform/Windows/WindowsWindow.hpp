#pragma once

#include <GLFW/glfw3.h>

#include "Viking/Core/Window.hpp"

namespace Viking {
	class WindowsWindow: public Window {
	public:
		WindowsWindow(const WindowProps& props);
		~WindowsWindow() override;

		void onUpdate() override;
	
		[[nodiscard]] uint32_t getWidth() const override;
		[[nodiscard]] uint32_t getHeight() const override;

		void setEventCallback(const EventCallbackFn& callback) override;

		void setVSync(bool enabled) override;

		[[nodiscard]] bool isVSync() const override;

		void* getNativeWindow() const override;

	private:
		WindowProps m_Props;
		GLFWwindow* m_Window{ nullptr };
	};
}
