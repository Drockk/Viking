#include "Viking/renderer/Renderer.hpp"

#include "Viking/renderer/Context.hpp"

namespace 
{
    class InternalRenderer
    {
    public:
        static void init(const std::string_view p_app_name, const std::shared_ptr<vi::Window>& p_window)
        {
            m_context = vi::Context::create();
            m_context->init(p_app_name, p_window);
        }

        static void shutdown()
        {
            m_context->cleanup();
        }

    private:
        inline static std::unique_ptr<vi::Context> m_context{};
    };
}

namespace vi
{
    void Renderer::init(const std::string_view p_app_name, const std::shared_ptr<Window>& p_window)
    {
        InternalRenderer::init(p_app_name, p_window);
    }

    void Renderer::shutdown()
    {
        InternalRenderer::shutdown();
    }
}
