#include "raia_app.h"

#include <stdio.h>
#define GLFW_INCLUDE_ES2
#define GL_GLEXT_PROTOTYPES
#include "GLFW/glfw3.h"
#include "EGL/egl.h"

static duk_context* ctx;

void regist_func(duk_context *duk_ctx, duk_c_function func, const char *str, int nargs) {
    duk_push_c_function(ctx, func, nargs);
    duk_put_global_string(ctx, str);
    ctx = duk_ctx;
}

static duk_ret_t regist_event_set_error_callback(duk_context *duk_ctx) {
    duk_dup(ctx, 0);
    duk_put_global_string(ctx, "_glfw_error_callback");
    ctx = duk_ctx;
    return 0;
}

static void event_error_callback(int error, const char *message) {
    fprintf(stderr, "GLFW: %s\n", message);
    duk_get_global_string(ctx, "_glfw_error_callback");
    duk_push_int(ctx, error);
    duk_push_string(ctx, message);
    duk_int_t rc = duk_pcall(ctx, 2);  /* no arguments in this example */
    if (rc != 0) {
        printf("Callback failed: %s\n", duk_safe_to_string(ctx, -1));
    }
    duk_pop(ctx);  /* pop result */
}

static duk_ret_t regist_event_set_key_callback(duk_context *duk_ctx) {
    duk_dup(ctx, 0);
    duk_put_global_string(ctx, "_glfw_key_callback");
    ctx = duk_ctx;
    return 0;
}

static void event_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    //if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    //    glfwSetWindowShouldClose(window, GL_TRUE);
    //}
    duk_get_global_string(ctx, "_glfw_key_callback");
    duk_push_int(ctx, key);
    duk_push_int(ctx, scancode);
    duk_push_int(ctx, action);
    duk_push_int(ctx, mods);
    duk_int_t rc = duk_pcall(ctx, 4);  /* no arguments in this example */
    //printf("%d %d %d %d\n",key,scancode,action,mods);
    //if (rc != 0) {
    //    printf("Callback failed: %s\n", duk_safe_to_string(ctx, -1));
    //}
    duk_pop(ctx);  /* pop result */
}

static duk_ret_t regist_event_set_update_callback(duk_context *duk_ctx){
    duk_dup(ctx, 0);
    duk_put_global_string(ctx, "_event_update_callback");
    set_exist_update_callback(true);
    ctx = duk_ctx;
    printf("Hello\n");
    return 0;
}

static void event_update_callback(void) {
    duk_get_global_string(ctx, "_event_update_callback");
    duk_int_t rc = duk_pcall(ctx, 0);  /* no arguments in this example */
    if (rc != 0) {
        printf("Callback failed: %s\n", duk_safe_to_string(ctx, -1));
    }
    duk_pop(ctx);  /* pop result */
}

static duk_ret_t regist_event_set_cursor_position_callback(duk_context *duk_ctx) {
    duk_dup(ctx, 0);
    duk_put_global_string(ctx, "_glfw_cursor_position_callback");
    ctx = duk_ctx;
    return 0;
}

static void event_cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    duk_get_global_string(ctx, "_glfw_cursor_position_callback");
    duk_push_number(ctx, xpos);
    duk_push_number(ctx, ypos);
    duk_int_t rc = duk_pcall(ctx, 2);  /* no arguments in this example */
    //if (rc != 0) {
    //    printf("Callback failed: %s\n", duk_safe_to_string(ctx, -1));
    //}
    duk_pop(ctx);  /* pop result */
}

static duk_ret_t regist_event_set_mouse_button_callback(duk_context *duk_ctx) {
    duk_dup(ctx, 0);
    duk_put_global_string(ctx, "_glfw_mouse_button_callback");
    ctx = duk_ctx;
    return 0;
}

static void event_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    duk_get_global_string(ctx, "_glfw_mouse_button_callback");
    duk_push_int(ctx, button);
    duk_push_int(ctx, action);
    duk_push_int(ctx, mods);
    duk_int_t rc = duk_pcall(ctx, 3);  /* no arguments in this example */
    //if (rc != 0) {
    //    printf("Callback failed: %s\n", duk_safe_to_string(ctx, -1));
    //}
    duk_pop(ctx);  /* pop result */
}

// Function: regist_callbacks
// コールバック群を登録する
//
static void regist_callbacks(duk_context *duk_ctx) {
    ctx = duk_ctx;
    glfwSetKeyCallback(get_raia_window(), event_key_callback); // キー入力コールバック
    glfwSetErrorCallback(event_error_callback); // エラーコールバック
    glfwSetCursorPosCallback(get_raia_window(), event_cursor_position_callback); // マウス移動
    glfwSetMouseButtonCallback(get_raia_window(), event_mouse_button_callback); // マウスボタン
    char* object =
            "var Event = {"
            "    setKeyCallback: function(a,b,c,d){return _event_set_key_callback(a,b,c,d);},"
            "    setErrorCallback: function(a,b){return _event_set_error_callback(a,b);},"
            "    setUpdateCallback: function(a){return _event_set_update_callback(a);},"
            "    setCursorPositionCallback: function(a,b,c,d){return _event_set_cursor_position_callback(a,b,c,d);},"
            "    setMouseButtonCallback: function(a,b,c,d){return _event_set_mouse_button_callback(a,b,c,d);},"
            "};"
    ;
    duk_eval_string(ctx, object);
}

/// GLFWの開始
void glfw_start(void) {
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
void glfw_redraw(void) {
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

duk_ret_t raia_redraw(duk_context *ctx) {
    glfw_redraw();
    return 0;
}

duk_ret_t raia_pool_events(duk_context *ctx) {
    glfwPollEvents();
    return 0;
}

duk_ret_t raia_window_should_close(duk_context *ctx) {
    GLFWwindow *window = get_raia_window();
    duk_push_number(ctx, glfwWindowShouldClose(window));
    return 1;
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

duk_ret_t run_update(duk_context *ctx) {
    raia_header_t header = get_raia_header();
    if(header.exist_update_callback == true) {
        loop_update();
    }
    return 0;
}

duk_ret_t make_window(duk_context *duk_ctx) {
    ctx = duk_ctx;
    init_raia_header(); // ヘッダー情報を初期化
    raia_header_t header = get_raia_header(); // ヘッダーを取得
    init_pixel_data(header.window_width, header.window_height, header.samples_per_pixel); // ピクセルデータを初期化
    glfw_start(); // GLFWを開始

    regist_callbacks(ctx); // コールバック群を登録する
    regist_func(ctx, regist_event_set_key_callback, "_event_set_key_callback", 1);
    regist_func(ctx, regist_event_set_error_callback, "_event_set_error_callback", 1);
    regist_func(ctx, regist_event_set_update_callback, "_event_set_update_callback", 1);
    regist_func(ctx, regist_event_set_cursor_position_callback, "_event_set_cursor_position_callback", 1);
    regist_func(ctx, regist_event_set_mouse_button_callback, "_event_set_mouse_button_callback", 1);
    init_raia_shader(get_pixel_data(), header.window_width, header.window_height);
    glfw_redraw(); // 画面を再描画しておく
    /*
    GLFWwindow *window = get_raia_window();
    while (!glfwWindowShouldClose(window)) {
        glfw_redraw();
        glfwPollEvents();
    }
    glfwTerminate(); // GLFWを終了
    free_pixel_data(); // ピクセルデータを解放する
    exit(EXIT_SUCCESS);
     */
    ctx = duk_ctx;
    return 0;
}