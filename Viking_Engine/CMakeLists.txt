project("Viking_Engine")

add_library(${PROJECT_NAME})

CPMAddPackage("gh:gabime/spdlog@1.13.0")
CPMAddPackage("gh:glfw/glfw#3.3.9")
CPMAddPackage("gh:wqking/eventpp@0.1.3")

target_sources(${PROJECT_NAME}
    PRIVATE
        source/Platform/Windows/Window.cpp
        source/Platform/Windows/Window.hpp
        source/Viking/core/Application.cpp
        source/Viking/core/Application.hpp
        source/Viking/core/Entrypoint.hpp
        source/Viking/core/Layer.hpp
        source/Viking/core/LayerStack.cpp
        source/Viking/core/LayerStack.hpp
        source/Viking/core/Log.cpp
        source/Viking/core/Log.hpp
        source/Viking/core/TimeStep.hpp
        source/Viking/core/Window.cpp
        source/Viking/core/Window.hpp
        source/Viking/event/Event.hpp
        source/Viking/event/ApplicationEvent.hpp
        source/Viking/event/DispatcherEvent.hpp
        source/Viking/event/DispatcherEvent.cpp
        source/Viking/renderer/Renderer.cpp
        source/Viking/renderer/Renderer.hpp
        source/Viking.hpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        source
        ${EVENTPP_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        eventpp::eventpp
        glfw
        spdlog
)

# if(CMAKE_VERSION VERSION_GREATER 3.28)
    set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 23)
# endif()
