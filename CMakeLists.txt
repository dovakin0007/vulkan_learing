cmake_minimum_required(VERSION 3.24)

project(VulkanLearning LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Vulkan REQUIRED)

add_executable(the_engine
    src/main.cpp
    src/window.cpp
    src/first_app.cpp
    src/pipeline.cpp
    src/engine_device.cpp
    src/swap_chain.cpp
)

target_include_directories(the_engine PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(the_engine PRIVATE
    Vulkan::Vulkan
    ${CMAKE_SOURCE_DIR}/lib/glfw3.lib
)

if(MSVC)
    target_compile_options(the_engine PRIVATE
        /W4
    )
endif()


# ---------------------------------------------------------
# GLSL -> SPIR-V
# ---------------------------------------------------------

find_program(GLSLC
    NAMES glslc
    PATHS "$ENV{VULKAN_SDK}/Bin"
    REQUIRED
)

set(SHADER_SOURCE_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/shader"
)

# Put compiled shaders next to the executable:
# build/Debug/shader/
set(SHADER_BINARY_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/shader"
)

file(GLOB SHADERS
    "${SHADER_SOURCE_DIR}/*.vert"
    "${SHADER_SOURCE_DIR}/*.frag"
)

set(SPIRV_FILES)

foreach(SHADER ${SHADERS})

    get_filename_component(SHADER_NAME
        "${SHADER}"
        NAME
    )

    set(SPIRV
        "${SHADER_BINARY_DIR}/${SHADER_NAME}.spv"
    )

    message(STATUS "GLSL: ${SHADER}")
    message(STATUS "SPIR-V: ${SPIRV}")

    add_custom_command(
        OUTPUT "${SPIRV}"

        COMMAND ${CMAKE_COMMAND} -E make_directory
            "${SHADER_BINARY_DIR}"

        COMMAND "${GLSLC}"
            "${SHADER}"
            -o "${SPIRV}"

        DEPENDS "${SHADER}"

        COMMENT "Compiling GLSL: ${SHADER_NAME}"

        VERBATIM
    )

    list(APPEND SPIRV_FILES "${SPIRV}")

endforeach()


add_custom_target(shaders ALL
    DEPENDS ${SPIRV_FILES}
)

add_dependencies(the_engine shaders)