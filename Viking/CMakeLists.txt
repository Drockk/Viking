# CMakeList.txt : CMake project for Viking, include source and define
# project specific logic here.
#

project("Viking")

# Add source to this project's executable.
find_package(Vulkan REQUIRED)

add_executable(Viking   "main.cpp" "vk_engine.cpp" "vk_initializers.cpp"
        vk_mesh.cpp
        Window.cpp
        Window.hpp)

target_include_directories(Viking
	PUBLIC
        ${VULKAN_INCLUDE_DIRS}
        ${PROJECT_SOURCE_DIR}/../dependencies/tinyobjloader
)

CPMAddPackage("gh:glfw/glfw#3.3.8")
CPMAddPackage("gh:charles-lunarg/vk-bootstrap#v0.7")
CPMAddPackage("gh:GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator#v3.0.1")
CPMAddPackage("gh:g-truc/glm#0.9.9.8")

target_link_libraries(Viking
    PUBLIC
        glfw
        glm::glm
        vk-bootstrap::vk-bootstrap
        Vulkan::Vulkan
        VulkanMemoryAllocator
)


if(CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Viking PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
