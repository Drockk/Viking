# CMakeList.txt : CMake project for Viking, include source and define
# project specific logic here.
#

project("Viking")

# Add source to this project's executable.
find_package(Vulkan REQUIRED)

add_executable(Viking
        Core/DeletionQueue.cpp
        Core/DeletionQueue.hpp
        Core/Log.cpp
        Core/Log.hpp
        Core/Window.cpp
        Core/Window.hpp
        Renderer/GraphicsContext.cpp
        Renderer/GraphicsContext.hpp
        Renderer/Shader.cpp
        Renderer/Shader.hpp
        main.cpp
        vk_engine.cpp
        vk_initializers.cpp
        vk_mesh.cpp
        vk_textures.hpp
        vk_textures.cpp
        Renderer/Buffer.cpp
        Renderer/Swapchain.cpp
 "Renderer/Mesh.cpp")

include_directories(.)

CPMAddPackage("gh:charles-lunarg/vk-bootstrap#v0.7")
CPMAddPackage("gh:fmtlib/fmt#10.1.1")
CPMAddPackage("gh:gabime/spdlog@1.13.0")
CPMAddPackage("gh:glfw/glfw#3.3.8")
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


if(CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Viking PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
