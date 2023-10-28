//
// Created by batzi on 03.10.2023.
//

#include "vk_mesh.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <iostream>

VertexInputDescription Vertex::getVertexDescription() {
    VertexInputDescription description;

    //We will have just 1 vertex buffer binding, with a per-vertex rate
    VkVertexInputBindingDescription mainBinding = {};
    mainBinding.binding = 0;
    mainBinding.stride = sizeof(Vertex);
    mainBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    description.m_bindings.emplace_back(mainBinding);

    //Position will be stored at Location 0
    VkVertexInputAttributeDescription positionAttribute = {};
    positionAttribute.binding = 0;
    positionAttribute.location = 0;
    positionAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    positionAttribute.offset = offsetof(Vertex, position);

    //Normal will be stored at Location 1
    VkVertexInputAttributeDescription normalAttribute = {};
    normalAttribute.binding = 0;
    normalAttribute.location = 1;
    normalAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    normalAttribute.offset = offsetof(Vertex, normal);

    //Color will be stored at Location 2
    VkVertexInputAttributeDescription colorAttribute = {};
    colorAttribute.binding = 0;
    colorAttribute.location = 2;
    colorAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    colorAttribute.offset = offsetof(Vertex, color);

    description.m_attributes.emplace_back(positionAttribute);
    description.m_attributes.emplace_back(normalAttribute);
    description.m_attributes.emplace_back(colorAttribute);
    return description;
}

bool Mesh::loadFromObj(const char *filename) {
    //attrib will contain the vertex arrays of the file
    tinyobj::attrib_t attrib;
    //shapes contain the info for each object in the file
    std::vector<tinyobj::shape_t> shapes;
    //materials contain the information about the material of each shape, but we won't use it.
    std::vector<tinyobj::material_t> materials;

    //error and warning output from the load function
    std::string warn;
    std::string err;

    //load the OBJ file
    tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename, nullptr);
    //make sure to output the warnings to the console, in case there are issues with the file
    if (!warn.empty()) {
        std::cout << "WARN: " << warn << std::endl;
    }
    //If we have any error, print it to the console, and break the mesh loading.
    //This happens if the file can't be found or is malformed
    if (!err.empty()) {
        std::cerr << err << std::endl;
        return false;
    }

    //Loop over
    std::for_each(shapes.begin(), shapes.end(), [&](const tinyobj::shape_t& t_shape){
        // Loop over faces(polygon)
        size_t index_offset{ 0 };
        for (auto f{ 0 }; f < t_shape.mesh.num_face_vertices.size(); ++f) {

            //hardcode loading to triangles
            auto fv = 3;

            // Loop over vertices in the face.
            for (auto v{ 0 }; v < fv; v++) {
                // access to vertex
                auto idx = t_shape.mesh.indices[index_offset + v];

                //vertex position
                auto vx = attrib.vertices[3 * idx.vertex_index + 0];
                auto vy = attrib.vertices[3 * idx.vertex_index + 1];
                auto vz = attrib.vertices[3 * idx.vertex_index + 2];
                //vertex normal
                auto nx = attrib.normals[3 * idx.normal_index + 0];
                auto ny = attrib.normals[3 * idx.normal_index + 1];
                auto nz = attrib.normals[3 * idx.normal_index + 2];

                //copy it into our vertex
                Vertex new_vert = {};
                new_vert.position.x = vx;
                new_vert.position.y = vy;
                new_vert.position.z = vz;

                new_vert.normal.x = nx;
                new_vert.normal.y = ny;
                new_vert.normal.z = nz;

                //We are setting the vertex color as the vertex normal.
                // This is just for display purposes
                new_vert.color = new_vert.normal;

                m_vertices.push_back(new_vert);
            }
            index_offset += fv;
        }
    });

    return true;
}
