#include "Platform/Vulkan/Renderer.hpp"
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
            m_renderer.init(m_context);
        }

        static void shutdown()
        {
            m_renderer.cleanup();
            m_context->cleanup();
        }

        static void draw()
        {
            m_renderer.draw();
        }

    private:
        inline static std::shared_ptr<vi::Context> m_context{};
        inline static vulkan::Renderer m_renderer;
    };
}

namespace vi
{
    void Renderer::init(const std::string_view p_app_name, const std::shared_ptr<Window>& p_window)
    {
        InternalRenderer::init(p_app_name, p_window);
    }

    void Renderer::draw()
    {
        InternalRenderer::draw();
    }

    void Renderer::shutdown()
    {
        InternalRenderer::shutdown();
    }
}
