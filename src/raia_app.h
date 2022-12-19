#ifndef RAIA_WINDOW_RAIA_APP_H
#define RAIA_WINDOW_RAIA_APP_H

#include "duktape/duktape.h"

#include "static/static_header.h"
#include "static/static_window.h"
#include "static/static_pixel_data.h"
#include "static/static_shader.h"
#include "wrapper/wrapper_gles.h"
#include "util/util_math.h"

void regist_func(duk_context *ctx, duk_c_function func, const char *str, int nargs);
void glfw_start(void);
void glfw_redraw(void);

duk_ret_t raia_redraw(duk_context *ctx);
duk_ret_t raia_pool_events(duk_context *ctx);
duk_ret_t raia_window_should_close(duk_context *ctx);
duk_ret_t make_window(duk_context *duk_ctx);
duk_ret_t run_update(duk_context *ctx);

#endif //RAIA_WINDOW_RAIA_APP_H
