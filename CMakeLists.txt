cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project(NaturaForge VERSION 0.0.1)


set(ENGINE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/engine/include)
set(ENGINE_LIBS engine)

add_subdirectory(external/glfw)
add_subdirectory(external/spdlog)

set(LINK_LIBRARIES 
    glfw
	spdlog
    ${Vulkan_LIBRARIES}
)

set(INCLUDE_DIRS 
    external/glfw/include
    external/glm
    external/spdlog/include
    ${Vulkan_INCLUDE_DIR}
)

add_subdirectory(engine)
add_subdirectory(game)

# target_link_libraries(${PROJECT_NAME} ${LINK_LIBRARIES})
# target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/bin/Release)
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/bin/Debug)
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/bin)