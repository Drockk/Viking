#ifndef SANDBOX_LAYER
#define SANDBOX_LAYER

#include <Viking.hpp>

namespace sandbox
{
    class SandboxLayer final: public vi::Layer
    {
    public:
        SandboxLayer(): Layer("Sandbox") {}

        void on_attach() override;
        void on_update(const vi::TimeStep& p_time_step) override;
    };
}

#endif // !SANDBOX_LAYER
