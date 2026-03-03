cmake_minimum_required(VERSION 3.10)
project(VulkanTest)

set(CMAKE_CXX_STANDARD 17)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

find_program(GLSL_COMPILER glslc REQUIRED)

function(compile_shader SHADER_FILE OUTPUT_FILE)
    add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        COMMAND ${GLSL_COMPILER} ${SHADER_FILE} -o ${OUTPUT_FILE}
        DEPENDS ${SHADER_FILE}
        COMMENT "Compiling ${SHADER_FILE} to ${OUTPUT_FILE}"
    )
endfunction()

compile_shader(${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.vert ${CMAKE_CURRENT_BINARY_DIR}/vert.spv)
compile_shader(${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.frag ${CMAKE_CURRENT_BINARY_DIR}/frag.spv)

add_executable(VulkanTest main.cpp 
    ${CMAKE_CURRENT_BINARY_DIR}/vert.spv 
    ${CMAKE_CURRENT_BINARY_DIR}/frag.spv
)

target_link_libraries(VulkanTest Vulkan::Vulkan glfw)
target_include_directories(VulkanTest PRIVATE 
    ${Vulkan_INCLUDE_DIRS} 
    ${GLM_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
)
