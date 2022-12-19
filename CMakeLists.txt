cmake_minimum_required(VERSION 3.24)
project(raia_app C)

include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)

set(CMAKE_C_STANDARD 11)

add_library(raia_app SHARED
        src/raia_app.c
        src/wrapper/wrapper_gles.c
        src/wrapper/wrapper_glfw.c
        src/static/static_header.c
        src/static/static_pixel_data.c
        src/static/static_shader.c
        src/static/static_window.c
        src/util/util_string.c
        src/util/util_math.c
        src/duktape/duktape.c)

target_link_libraries(raia_app
        $<$<PLATFORM_ID:Darwin>: egl glfw GLESv2>
        $<$<PLATFORM_ID:Linux>: >
        $<$<PLATFORM_ID:Windows>: >
        )