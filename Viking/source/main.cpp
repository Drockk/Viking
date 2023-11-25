#include "vk_engine.hpp"
#include "Debug/Profiler.hpp"

#include <iostream>

int main()
{
    PROFILER_APP("Viking Engine");
    try {
        ViEngine engine;

        engine.init();
        engine.run();
        engine.cleanup();
    }
    catch (const std::exception& exception) {
        std::cerr << exception.what() << "\n";
        return -1;
    }

    return 0;
}
