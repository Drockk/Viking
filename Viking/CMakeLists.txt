project("Viking")

find_package(Vulkan REQUIRED)

add_executable(Viking
    "source/Viking/Core/DeletionQueue.cpp"
    "source/Viking/Core/DeletionQueue.hpp"
    "source/Viking/Core/Log.cpp"
    "source/Viking/Core/Log.hpp"
    "source/Viking/Core/Window.cpp"
    "source/Viking/Core/Window.hpp"
    "source/Viking/Renderer/Buffer.cpp"
    "source/Viking/Renderer/Buffer.hpp"
    "source/Viking/Renderer/GraphicsContext.cpp"
    "source/Viking/Renderer/GraphicsContext.hpp"
    "source/Viking/Renderer/Shader.cpp"
    "source/Viking/Renderer/Shader.hpp"
    "source/main.cpp"
    "source/vk_engine.cpp"
    "source/vk_initializers.cpp"
    "source/vk_mesh.cpp"
    "source/vk_textures.hpp"
    "source/vk_textures.cpp"
)

include_directories("source/Viking")

CPMAddPackage("gh:charles-lunarg/vk-bootstrap#v0.7")
CPMAddPackage("gh:fmtlib/fmt#10.1.1")
CPMAddPackage("gh:gabime/spdlog@1.12.0")
CPMAddPackage(
    NAME glfw
    GITHUB_REPOSITORY glfw/glfw
    GIT_TAG 3.3.8
    OPTIONS
        "GLFW_BUILD_TESTS OFF"
        "GLFW_BUILD_EXAMPLES OFF"
        "GLFW_BULID_DOCS OFF"
)
CPMAddPackage("gh:GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator#v3.0.1")
CPMAddPackage("gh:g-truc/glm#0.9.9.8")

target_include_directories(Viking
	PUBLIC
        ${VULKAN_INCLUDE_DIRS}
        ${PROJECT_SOURCE_DIR}/../dependencies/tinyobjloader
        ${PROJECT_SOURCE_DIR}/../dependencies/stb
)

target_link_directories(Viking
    PUBLIC
        $ENV{VULKAN_SDK}/Lib
)

target_link_libraries(Viking
    PUBLIC
        glfw
        glm::glm
        fmt
        spdlog
        vk-bootstrap::vk-bootstrap
        Vulkan::Vulkan
        shaderc_sharedd
        spirv-cross-cored.lib
        spirv-cross-glsld
        SPIRV-Toolsd
        VulkanMemoryAllocator
)


if(CMAKE_VERSION VERSION_GREATER 3.20)
  set_property(TARGET Viking PROPERTY CXX_STANDARD 23)
endif()

# TODO: Add tests and install targets if needed.
