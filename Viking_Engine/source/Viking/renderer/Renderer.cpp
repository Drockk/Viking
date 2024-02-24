#include "Viking/renderer/Renderer.hpp"

#include "Viking/renderer/Context.hpp"

namespace 
{
    class InternalRenderer
    {
    public:
        static void init()
        {
            m_context = vi::Context::create();
        }

        static void shutdown()
        {
            
        }

    private:
        inline static std::unique_ptr<vi::Context> m_context{};
    };
}

namespace vi
{
    void Renderer::init()
    {
        InternalRenderer::init();
    }

    void Renderer::shutdown()
    {
        InternalRenderer::shutdown();
    }
}
