#pragma once
#include <memory>

namespace Viking {
	class Application {
	public:
		Application() = default;
		virtual ~Application() = default;

		virtual void init() = 0;
		virtual void run() = 0;
		virtual void shutdown() = 0;
	};

	std::unique_ptr<Application> createApplication();
}
