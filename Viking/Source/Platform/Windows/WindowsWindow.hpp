#pragma once

#include <GLFW/glfw3.h>

#include "Viking/Core/Window.hpp"

#include "Viking/Renderer/Context.hpp"

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

		[[nodiscard]] void* getNativeWindow() const override;
		[[nodiscard]] Ref<Context> getContext() const override;
		
	private:
		struct WindowData {
			std::string Title;
			unsigned int Width;
			unsigned int Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
		GLFWwindow* m_Window{ nullptr };

		Ref<Context> m_Context;
	};
}
