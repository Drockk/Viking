#ifndef SANDBOX_LAYER
#define SANDBOX_LAYER

#include <Viking.hpp>

namespace sandbox
{
    class SandboxLayer final: public vi::Layer
    {
    public:
        SandboxLayer(): Layer("Sandbox") {}
    };
}

#endif // !SANDBOX_LAYER
