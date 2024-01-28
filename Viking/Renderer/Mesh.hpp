#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace vi
{
    struct VertexInputDescription
    {
        std::vector<VkVertexInputBindingDescription> bindings;
        std::vector<VkVertexInputAttributeDescription> attributes;

        VkPipelineVertexInputStateCreateFlags flags{ 0 };
    };

    struct Vertex
    {
        glm::vec3 m_position{};
        glm::vec3 m_normal{};
        glm::vec3 m_color{};
        glm::vec2 m_uv{};

        static VertexInputDescription get_vertex_description();
    };

    class Mesh
    {
    public:
        Mesh(std::string_view p_filename);
        Mesh(const std::vector<Vertex>& p_vertices);
        ~Mesh();

    private:
        void load_from_obj(std::string_view p_filename);
        void upload_mesh();

        std::vector<Vertex> m_vertices;
    };
}
