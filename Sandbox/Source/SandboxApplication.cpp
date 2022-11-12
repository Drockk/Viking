#include <Viking.hpp>
#include <Viking/Core/Entrypoint.hpp>

class HelloTriangleApplication: public Viking::Application {

};

std::unique_ptr<Viking::Application> createApplication() {
    return std::make_unique<HelloTriangleApplication>();
}