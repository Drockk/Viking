#include "vk_engine.hpp"

int main()
{
    ViEngine engine;

    engine.init();
    engine.run();
    engine.cleanup();

    return 0;
}
