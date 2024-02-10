project("Viking_Engine")

add_library(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
    PRIVATE
        source/Viking/core/Application.cpp
        source/Viking/core/Application.hpp
        source/Viking/core/Entrypoint.hpp
        source/Viking.hpp
)

target_include_directories(${PROJECT_NAME} PUBLIC source)

if(CMAKE_VERSION VERSION_GREATER 3.28)
    set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 23)
endif()