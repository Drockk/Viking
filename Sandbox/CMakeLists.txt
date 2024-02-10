project("Sandbox")

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
        PRIVATE
        source/SandboxApplication.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC source)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Viking_Engine
)

if(CMAKE_VERSION VERSION_GREATER 3.28)
    set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 23)
endif()