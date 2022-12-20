#include "raia_app.h"

#include <stdio.h>
#define GLFW_INCLUDE_ES2
#define GL_GLEXT_PROTOTYPES
#include "GLFW/glfw3.h"
#include "EGL/egl.h"

static duk_context* _ctx;

duk_ret_t raia_app_event_error_callback(duk_context *ctx) {
    duk_dup(_ctx, 0);
    duk_put_global_string(_ctx, "_glfw_error_callback");
    _ctx = ctx;
    return 0;
}

duk_ret_t raia_app_event_key_callback(duk_context *ctx) {
    duk_dup(_ctx, 0);
    duk_put_global_string(_ctx, "_glfw_key_callback");
    _ctx = ctx;
    return 0;
}

duk_ret_t raia_app_event_update_callback(duk_context *ctx) {
    duk_dup(_ctx, 0);
    duk_put_global_string(_ctx, "_event_update_callback");
    set_exist_update_callback(true);
    _ctx = ctx;
    return 0;
}

duk_ret_t raia_app_event_cursor_position_callback(duk_context *ctx) {
    duk_dup(_ctx, 0);
    duk_put_global_string(_ctx, "_glfw_cursor_position_callback");
    _ctx = ctx;
    return 0;
}

duk_ret_t raia_app_event_mouse_button_callback(duk_context *ctx) {
    duk_dup(_ctx, 0);
    duk_put_global_string(_ctx, "_glfw_mouse_button_callback");
    _ctx = ctx;
    return 0;
}

duk_ret_t raia_app_event_update_enable(duk_context *ctx) {
    raia_header_t header = get_raia_header();
    if(header.exist_update_callback == true) {
        loop_update();
    }
    return 0;
}

duk_ret_t raia_app_window_redraw(duk_context *ctx) {
    glfw_redraw();
    return 0;
}

duk_ret_t raia_app_window_pool_events(duk_context *ctx) {
    glfwPollEvents();
    return 0;
}

duk_ret_t raia_app_window_should_close(duk_context *ctx) {
    GLFWwindow *window = get_raia_window();
    duk_push_number(ctx, glfwWindowShouldClose(window));
    return 1;
}

duk_ret_t raia_app_window_init(duk_context *ctx) {
    _ctx = ctx;
    init_raia_header(); // ヘッダー情報を初期化
    raia_header_t header = get_raia_header(); // ヘッダーを取得
    init_pixel_data(header.window_width, header.window_height, header.samples_per_pixel); // ピクセルデータを初期化
    glfw_start(); // GLFWを開始
    regist_callbacks(_ctx); // コールバック群を登録する
    init_raia_shader(get_pixel_data(), header.window_width, header.window_height);
    glfw_redraw(); // 画面を再描画しておく
    return 0;
}

static void event_error_callback(int error, const char *message) {
    fprintf(stderr, "GLFW: %s\n", message);
    duk_get_global_string(_ctx, "_glfw_error_callback");
    duk_push_int(_ctx, error);
    duk_push_string(_ctx, message);
    duk_pcall(_ctx, 2);
    duk_pop(_ctx);  /* pop result */
}

static void event_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    duk_get_global_string(_ctx, "_glfw_key_callback");
    duk_push_int(_ctx, key);
    duk_push_int(_ctx, scancode);
    duk_push_int(_ctx, action);
    duk_push_int(_ctx, mods);
    duk_pcall(_ctx, 4);
    duk_pop(_ctx);  /* pop result */
}

static void event_update_callback(void) {
    duk_get_global_string(_ctx, "_event_update_callback");
    duk_pcall(_ctx, 0);
    duk_pop(_ctx);  /* pop result */
}

static void event_cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    duk_get_global_string(_ctx, "_glfw_cursor_position_callback");
    duk_push_number(_ctx, xpos);
    duk_push_number(_ctx, ypos);
    duk_pcall(_ctx, 2);
    duk_pop(_ctx);  /* pop result */
}

static void event_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    duk_get_global_string(_ctx, "_glfw_mouse_button_callback");
    duk_push_int(_ctx, button);
    duk_push_int(_ctx, action);
    duk_push_int(_ctx, mods);
    duk_pcall(_ctx, 3);
    duk_pop(_ctx);  /* pop result */
}

static void regist_callbacks(duk_context *ctx) {
    glfwSetKeyCallback(get_raia_window(), event_key_callback); // キー入力コールバック
    glfwSetErrorCallback(event_error_callback); // エラーコールバック
    glfwSetCursorPosCallback(get_raia_window(), event_cursor_position_callback); // マウス移動
    glfwSetMouseButtonCallback(get_raia_window(), event_mouse_button_callback); // マウスボタン
}

static void wait_time_with_fps(double now, double prev, int fps) {
    while(!glfwWindowShouldClose(get_raia_window())) { // ほぼ 60fps になるように待つ
        now = (double)clock() / CLOCKS_PER_SEC;
        if ((now - prev) > 1.0 / (double)fps) {
            break;
        }
        glfwPollEvents();
    }
}

static void loop_update(void) {
    double now = (double)clock() / CLOCKS_PER_SEC;
    double prev = (double)clock() / CLOCKS_PER_SEC;
    while(!glfwWindowShouldClose(get_raia_window())) {
        now = (double)clock() / CLOCKS_PER_SEC;
        event_update_callback();
        glfw_redraw();
        prev = now;
        wait_time_with_fps(now, prev, 60);
        glfwPollEvents();
    }
}

/// GLFWの開始
static void glfw_start(void) {
    glfw_init();
    raia_header_t header = get_raia_header();
    init_raia_window(header.window_width, header.window_height, header.window_title);
    GLFWwindow* glfw_window = get_raia_window();
    glfwMakeContextCurrent(glfw_window);
    glfwSwapInterval(1);

    // HiDPI判定
    int now_width, now_height;
    glfwGetFramebufferSize(glfw_window, &now_width, &now_height);
    if (now_width > header.window_width) {
        set_resolution_size(header.window_width * 2, header.window_height * 2);
    } else {
        set_resolution_size(header.window_width, header.window_height);
    }
}

/// 再描画する
static void glfw_redraw(void) {
    raia_header_t header = get_raia_header();
    raia_shader_t shader = get_raia_shader();
    GLFWwindow* glfw_window = get_raia_window();
    uint8_t* pixel_data = get_pixel_data();

    // 描画の準備
    glViewport(0, 0, header.resolution_width, header.resolution_height); // Set the viewport
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer
    glUseProgram(shader.program); // Use the program object
    glVertexAttribPointer(shader.pos_location, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), shader.vertices); // Load the vertex position
    glVertexAttribPointer(shader.tex_location, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), shader.vertices + 3); // Load the texture coordinate
    glEnableVertexAttribArray(shader.pos_location);
    glEnableVertexAttribArray(shader.tex_location);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, header.window_width, header.window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixel_data);
    glActiveTexture(GL_TEXTURE0); // Bind the texture
    glBindTexture(GL_TEXTURE_2D, shader.texture);
    glUniform1i(shader.sampler_location, 0); // Set the texture sampler to texture unit to 0
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, shader.indices);
    glfwSwapBuffers(glfw_window);
}