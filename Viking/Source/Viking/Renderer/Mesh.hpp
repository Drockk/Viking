#pragma once
#include "Viking/Renderer/Buffer.hpp"
#include "Viking/Renderer/Texture.hpp"

namespace Viking {
    class Mesh {
    public:
        Mesh(const std::string& filename);

        void addColorTexture(const std::string& filename);

        std::vector<Vertex>& getVertices();
        std::vector<uint32_t>& getIndices();

        Ref<Texture2D> getColorTexture();

        Ref<VertexBuffer> getVertexBuffer();
        Ref<IndexBuffer> getIndexBuffer();

    private:
        std::vector<Vertex> m_Vertices;
        std::vector<uint32_t> m_Indices;

        Ref<Texture2D> m_ColorTexture;

        Ref<VertexBuffer> m_VertexBuffer;
        Ref<IndexBuffer> m_IndexBuffer;
    };
}
