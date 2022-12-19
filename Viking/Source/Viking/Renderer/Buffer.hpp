#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    bool operator==(const Vertex& other) const {
        return pos == other.pos && color == other.color && texCoord == other.texCoord;
    }
};

template<> struct std::hash<Vertex> {
    size_t operator()(Vertex const& vertex) const noexcept {
        return (hash<glm::vec3>()(vertex.pos) ^ hash<glm::vec3>()(vertex.color) << 1) >> 1 ^ hash<glm::vec2>()(vertex.texCoord) << 1;
    }
};

namespace Viking {
	class IndexBuffer {
	public:
		virtual ~IndexBuffer() = default;

		static Ref<IndexBuffer> create(const std::vector<uint32_t>& indices);
	};

	class VertexBuffer {
	public:
		virtual ~VertexBuffer() = default;

		static Ref<VertexBuffer> create(const std::vector<Vertex>& vertices);
	};
}
