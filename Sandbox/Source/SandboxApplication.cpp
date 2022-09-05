#include <Viking.hpp>
#include <Viking/Core/Entrypoint.hpp>

#include "ExampleLayer.hpp"

class Sandbox: public Viking::Application {
public:
	Sandbox(const Viking::ApplicationCommandLineArgs args): Application("Sandbox", args) {
		pushLayer(new ExampleLayer());
	}

	~Sandbox() override = default;
};

std::unique_ptr<Viking::Application> createApplication(Viking::ApplicationCommandLineArgs args) {
	return std::make_unique<Sandbox>(args);
}
