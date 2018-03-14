#ifndef LIBSHAPES_EGLSTATE_H
#define LIBSHAPES_EGLSTATE_H

#include <stdbool.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include "bcm_host.h"
#pragma GCC diagnostic pop
#include <EGL/egl.h>
#include <VG/openvg.h>

#define CURSOR_LAYER 128
#define DIM_LAYER 255

#if defined(__cplusplus)
extern "C" 
{
#endif

        typedef enum renderobj_e {
                RENDEROBJ_NONE = 0,
                RENDEROBJ_MAIN,
                RENDEROBJ_IMAGE,
                RENDEROBJ_WINDOW
        } renderobj_e;

        typedef struct window_t {
                // Window dimentions
                int32_t xpos;
                int32_t ypos;
                uint32_t width;
                uint32_t height;
                // dispmanx window 
                DISPMANX_ELEMENT_HANDLE_T element;
                EGL_DISPMANX_WINDOW_T nativewindow;
                VC_DISPMANX_ALPHA_T alpha;
                uint32_t layer;
                // EGL data
                EGLSurface surface;
                EGLContext context;
        } window_t;

        typedef struct renderobj_t {
                struct renderobj_t *prev;
                struct renderobj_t *next;
                renderobj_e type;
                VGImage image;
                window_t window;
        } renderobj_t;

        typedef struct cursor_t {
                int32_t xpos;
                int32_t ypos;
                int32_t hot_x;
                int32_t hot_y;
                window_t window;
                DISPMANX_RESOURCE_HANDLE_T resource;
        } cursor_t;

        typedef struct STATE_T {
                // Screen dimentions
                uint32_t screen_width;
                uint32_t screen_height;
                uint32_t screen_pitch;
                // dispmanx display
                DISPMANX_DISPLAY_HANDLE_T dmx_display;
                // EGL display (common to all windows hence in here not window_t)
                EGLDisplay egl_display;
                // Renderable surfaces
                renderobj_t render_base;
                renderobj_t *render_list;
                renderobj_t *render_target;
} STATE_T;

#if defined(__cplusplus)
}
#endif

#endif
