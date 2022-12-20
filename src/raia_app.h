#ifndef RAIA_WINDOW_RAIA_APP_H
#define RAIA_WINDOW_RAIA_APP_H

#include "duktape/duktape.h"

#include "static/static_header.h"
#include "static/static_window.h"
#include "static/static_pixel_data.h"
#include "static/static_shader.h"
#include "wrapper/wrapper_gles.h"
#include "util/util_math.h"

static void regist_callbacks(duk_context *ctx);
static void loop_update(void);
void regist_func(duk_context *ctx, duk_c_function func, const char *str, int nargs);
static void glfw_start(void);
static void glfw_redraw(void);

duk_ret_t raia_app_event_key_callback(duk_context *ctx);
duk_ret_t raia_app_event_error_callback(duk_context *ctx);
duk_ret_t raia_app_event_update_callback(duk_context *ctx);
duk_ret_t raia_app_event_cursor_position_callback(duk_context *ctx);
duk_ret_t raia_app_event_mouse_button_callback(duk_context *ctx);
duk_ret_t raia_app_event_update_enable(duk_context *ctx);

#endif //RAIA_WINDOW_RAIA_APP_H
