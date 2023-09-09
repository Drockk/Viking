# CMakeList.txt : CMake project for Viking, include source and define
# project specific logic here.
#

project ("Viking")

# Add source to this project's executable.
find_package (Vulkan REQUIRED FATAL_ERROR)

add_executable (Viking   "main.cpp" "vk_engine.cpp")

target_include_directories (Viking
	PUBLIC ${GLFW_INCLUDE_DIRS}
	PUBLIC ${VULKAN_INCLUDE_DIRS}
)

target_link_libraries (Viking glfw)
target_link_libraries (Viking Vulkan::Vulkan)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Viking PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
