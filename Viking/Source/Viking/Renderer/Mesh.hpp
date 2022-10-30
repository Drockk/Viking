#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

namespace Viking {
	struct Vertex {
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 texCoord;

        bool operator==(const Vertex& other) const {
            return pos == other.pos && color == other.color && texCoord == other.texCoord;
        }
	};

	class Mesh {
	public:
		Mesh() = default;
		~Mesh() = default;

        void loadModel(const std::string& path);

        [[nodiscard]] std::vector<Vertex> getVertices() const;
		[[nodiscard]] std::vector<uint32_t> getIndices() const;

	private:
        std::vector<Vertex> m_Vertices;
        std::vector<uint32_t> m_Indices;
	};
}

template<> struct std::hash<Viking::Vertex> {
    size_t operator()(Viking::Vertex const& vertex) const noexcept {
        return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
    }
};
