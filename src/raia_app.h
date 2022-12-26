#ifndef RAIA_WINDOW_RAIA_APP_H
#define RAIA_WINDOW_RAIA_APP_H

#include "duktape/duktape.h"
#include "static/static_header.h"
#include "static/static_window.h"
#include "static/static_pixel_data.h"
#include "static/static_shader.h"
#include "static/static_ctx.h"
#include "wrapper/wrapper_gles.h"
#include "util/util_math.h"

static void glfw_loop(void);
static void glfw_start(void);
static void glfw_redraw(void);
void regist_callbacks(duk_context *ctx);
void regist_func(duk_context *ctx, duk_c_function func, const char *str, int nargs);

duk_ret_t raia_app_event_key_callback(duk_context *ctx);
duk_ret_t raia_app_event_error_callback(duk_context *ctx);
duk_ret_t raia_app_event_update_callback(duk_context *ctx);
duk_ret_t raia_app_event_cursor_position_callback(duk_context *ctx);
duk_ret_t raia_app_event_mouse_button_callback(duk_context *ctx);
duk_ret_t raia_app_event_update_enable(duk_context *ctx);
void init(void);

#endif //RAIA_WINDOW_RAIA_APP_H
