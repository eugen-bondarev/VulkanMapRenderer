cmake_minimum_required(VERSION 3.16)

project(Engine VERSION 1.0.0)

file(GLOB_RECURSE src src/*.cpp)
add_library(engine ${src})

find_package(Vulkan REQUIRED)

target_include_directories(
	engine PUBLIC 
	${ENGINE_INCLUDE_DIRS}/engine 
	${Vulkan_INCLUDE_DIR} 
	../external/stb_image/include
	../external/glfw/include 
	../external/spdlog/include 
	../external/glm
)

target_precompile_headers(engine PRIVATE src/engine/core/graphics/common.h)

target_link_libraries(engine 
    glfw
	spdlog
    ${Vulkan_LIBRARIES}
)