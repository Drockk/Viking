#include "Renderer/Mesh.hpp"

#include "Core/DeletionQueue.hpp"
#include "Core/Log.hpp"
#include "Renderer/Buffer.hpp"
#include "Renderer/GraphicsContext.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <vk_mem_alloc.h>

namespace vi
{
    VertexInputDescription Vertex::get_vertex_description()
    {
        VertexInputDescription description{};

        // We will have just 1 vertex buffer binding, with a per-vertex rate
        VkVertexInputBindingDescription mainBinding;
        mainBinding.binding = 0;
        mainBinding.stride = sizeof(Vertex);
        mainBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        description.bindings.emplace_back(mainBinding);

        // Position will be stored at Location 0
        VkVertexInputAttributeDescription positionAttribute;
        positionAttribute.binding = 0;
        positionAttribute.location = 0;
        positionAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
        positionAttribute.offset = offsetof(Vertex, m_position);

        // Normal will be stored at Location 1
        VkVertexInputAttributeDescription normalAttribute;
        normalAttribute.binding = 0;
        normalAttribute.location = 1;
        normalAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
        normalAttribute.offset = offsetof(Vertex, m_normal);

        // Position will be stored at Location 2
        VkVertexInputAttributeDescription colorAttribute;
        colorAttribute.binding = 0;
        colorAttribute.location = 2;
        colorAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
        colorAttribute.offset = offsetof(Vertex, m_color);

        // UV will be stored at Location 3
        VkVertexInputAttributeDescription uvAttribute;
        uvAttribute.binding = 0;
        uvAttribute.location = 3;
        uvAttribute.format = VK_FORMAT_R32G32_SFLOAT;
        uvAttribute.offset = offsetof(Vertex, m_uv);

        description.attributes.emplace_back(positionAttribute);
        description.attributes.emplace_back(normalAttribute);
        description.attributes.emplace_back(colorAttribute);
        description.attributes.push_back(uvAttribute);

        return description;
    }

    Mesh::Mesh(const std::string_view p_filename)
    {
        load_from_obj(p_filename);
        upload_mesh();
    }

    Mesh::Mesh(const std::vector<Vertex>& p_vertices): m_vertices(p_vertices)
    {
    }

    void Mesh::load_from_obj(const std::string_view p_filename)
    {
        // Attrib will contain the vertex arrays of the file
        tinyobj::attrib_t attrib;
        // Shapes contain the info for each object in the file
        std::vector<tinyobj::shape_t> shapes;
        // Materials contain the information about the material of each shape, but we won't use it, for now.
        std::vector<tinyobj::material_t> materials;

        // Error and warning output from the load function
        std::string warning{};
        std::string error{};

        // Load the OBJ file
        LoadObj(&attrib, &shapes, &materials, &warning, &error, p_filename.data(), nullptr);

        // Make sure to output the warnings to the console, in case there are issues with the file
        if (!warning.empty()) {
            VI_CORE_WARN(warning);
        }

        // If we have any error, print it to the console, and break the mesh loading.
        // This happens if the file can't be found or is malformed.
        if (!error.empty()) {
            throw std::runtime_error(error);
        }

        // Loop over shapes
        for (const auto& shape : shapes)
        {
            // Loop over faces(polygon)
            size_t index_offset{};
            for (size_t f{ 0 }; f < shape.mesh.num_face_vertices.size(); ++f)
            {
                //hardcode loading to triangles
                size_t fv{ 3 };

                // Loop over vertices in the face.
                for (size_t v = 0; v < fv; v++)
                {
                    // access to vertex
                    const auto idx = shape.mesh.indices[index_offset + v];

                    //vertex position
                    tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
                    tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
                    tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
                    //vertex normal
                    tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
                    tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
                    tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];

                    //copy it into our vertex
                    Vertex new_vert{};
                    new_vert.m_position.x = vx;
                    new_vert.m_position.y = vy;
                    new_vert.m_position.z = vz;

                    new_vert.m_normal.x = nx;
                    new_vert.m_normal.y = ny;
                    new_vert.m_normal.z = nz;

                    //We are setting the vertex color as the vertex normal.
                    // This is just for display purposes
                    new_vert.m_color = new_vert.m_normal;

                    //vertex uv
                    tinyobj::real_t ux = attrib.texcoords[2 * idx.texcoord_index + 0];
                    tinyobj::real_t uy = attrib.texcoords[2 * idx.texcoord_index + 1];

                    new_vert.m_uv.x = ux;
                    new_vert.m_uv.y = 1 - uy;

                    m_vertices.push_back(new_vert);
                }
                index_offset += fv;
            }
        }
    }

    void Mesh::upload_mesh()
    {
        const auto buffer_size = m_vertices.size() * sizeof(Vertex);

        auto staging_buffer = Buffer(buffer_size, Buffer::Usage::STAGING_BUFFER, Buffer::MemoryUsage::CPU_ONLY);
        staging_buffer.copy_data_to_buffer(m_vertices.data(), buffer_size);

        //// Allocate vertex buffer
        //VkBufferCreateInfo staging_buffer_info{};
        //staging_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        //staging_buffer_info.pNext = nullptr;
        //// This is the total size, in bytes, of the buffer we are allocating
        //staging_buffer_info.size = buffer_size;
        //// This buffer is going to be used as a Vertex Buffer
        //staging_buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

        //// Let the VMA library know that this data should be writeable by CPU, but also readable by GPU
        //VmaAllocationCreateInfo vma_alloc_info{};
        //vma_alloc_info.usage = VMA_MEMORY_USAGE_CPU_ONLY;

        //AllocatedBuffer staging_buffer;
        ////allocate the buffer
        //const auto allocator = GraphicsContext::get_allocator();
        //VK_CHECK(vmaCreateBuffer(allocator, &staging_buffer_info, &vma_alloc_info,
        //    &staging_buffer.m_buffer,
        //    &staging_buffer.m_allocation,
        //    nullptr));

        ////copy vertex data
        //void* data;
        //vmaMapMemory(allocator, staging_buffer.m_allocation, &data);

        //memcpy(data, m_vertices.data(), m_vertices.size() * sizeof(vi::Vertex));

        //vmaUnmapMemory(allocator, staging_buffer.m_allocation);


        ////allocate vertex buffer
        //VkBufferCreateInfo vertexBufferInfo{};
        //vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        //vertexBufferInfo.pNext = nullptr;
        ////this is the total size, in bytes, of the buffer we are allocating
        //vertexBufferInfo.size = buffer_size;
        ////this buffer is going to be used as a Vertex Buffer
        //vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

        ////let the VMA library know that this data should be gpu native
        //vma_alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        ////allocate the buffer
        //VK_CHECK(vmaCreateBuffer(allocator, &vertexBufferInfo, &vma_alloc_info,
        //    &p_mesh.vertexBuffer.m_buffer,
        //    &p_mesh.vertexBuffer.m_allocation,
        //    nullptr));
        ////add the destruction of triangle mesh buffer to the deletion queue
        //DeletionQueue::push_function([=, this] {
        //    vmaDestroyBuffer(allocator, p_mesh.vertexBuffer.m_buffer, p_mesh.vertexBuffer.m_allocation);
        //});

        //immediate_submit([=](const VkCommandBuffer cmd) {
        //    VkBufferCopy copy;
        //    copy.dstOffset = 0;
        //    copy.srcOffset = 0;
        //    copy.size = buffer_size;
        //    vkCmdCopyBuffer(cmd, staging_buffer.m_buffer, p_mesh.vertexBuffer.m_buffer, 1, &copy);
        //    });

        //vmaDestroyBuffer(allocator, staging_buffer.m_buffer, staging_buffer.m_allocation);
    }
}
