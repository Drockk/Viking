#include "vipch.hpp"
#include "Viking/Renderer/Buffer.hpp"

#include "Platform/Vulkan/VulkanBuffer.hpp"

namespace Viking {
	Ref<IndexBuffer> IndexBuffer::create(const std::vector<uint32_t>& indices) {
		return createRef<Vulkan::IndexBuffer>(indices);
	}

	Ref<VertexBuffer> VertexBuffer::create(const std::vector<Vertex>& vertices) {
		return createRef<Vulkan::VertexBuffer>(vertices);
	}
}
