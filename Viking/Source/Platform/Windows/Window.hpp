#pragma once

#include "Viking/Core/Window.hpp"

#include <GLFW/glfw3.h>

namespace Windows {
	class Window: public Viking::Window {
	public:
		Window(const Viking::WindowProperties& properties);
		~Window() override;

		void onUpdate() override;

		[[nodiscard]] uint32_t getWidth() override;
		[[nodiscard]] uint32_t getHeight() override;

		// Window attributes
		void setEventCallback(const EventCallbackFunction& callback) override;
		void setVSync(bool enabled) override;
		[[nodiscard]] bool isVSync() const override;

		[[nodiscard]] void* getNativeWindow() const override;
	private:
		void init(const Viking::WindowProperties& properties);
		void shutdown() const;

		GLFWwindow* m_Window{ nullptr };

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFunction EventCallback;
		};

		WindowData m_Data;
	};
}
