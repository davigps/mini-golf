cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.x
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# Create src directory structure if it doesn't exist
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/entities)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/core)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/utils)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/systems)

set(SOURCE_FILES
    src/main.cpp
    src/core/Game.cpp
    src/utils/Entity.hpp
    src/utils/Colors.hpp
    src/utils/ResourceManager.hpp
    src/entities/Ball.cpp
    src/entities/Obstacle.cpp
    src/entities/Particle.cpp
    src/systems/PhysicsSystem.cpp
    src/systems/InputHandler.cpp
    src/systems/ObstacleGenerator.cpp
    src/systems/ParticleSystem.cpp
)

add_executable(main ${SOURCE_FILES})
target_compile_features(main PRIVATE cxx_std_17)
target_link_libraries(main PRIVATE SFML::Graphics)
