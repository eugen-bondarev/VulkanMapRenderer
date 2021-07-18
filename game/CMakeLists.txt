cmake_minimum_required(VERSION 3.16)

project(Game VERSION 1.0.0)

file(GLOB_RECURSE src src/*.cpp)
add_executable(game ${src})

# target_include_directories(game PUBLIC ${ENGINE_INCLUDE_DIRS})
# target_link_libraries(game ${ENGINE_LIBS})
target_include_directories(game PUBLIC ../engine/src)
target_include_directories(game PUBLIC external)
target_link_libraries(game engine)