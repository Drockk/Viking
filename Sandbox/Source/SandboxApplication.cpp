#include <Viking.hpp>

#include <iostream>
#include <cstdlib>

class HelloTriangleApplication: public Viking::Application {

};

int main() {

    try {
        HelloTriangleApplication app;
        app.init();
        app.run();
        app.shutdown();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}