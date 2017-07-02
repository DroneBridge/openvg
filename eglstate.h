#ifndef LIBSHAPES_EGLSTATE_H
#define LIBSHAPES_EGLSTATE_H

#include <stdbool.h>
#include <VG/openvg.h>

#define CURSOR_LAYER 128
#define DIM_LAYER 255

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
	// dispman window 
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
        // dispmanx display
        DISPMANX_DISPLAY_HANDLE_T dmx_display;
        // EGL display (common to all windows hence in here not window_t)
        EGLDisplay egl_display;
        // Renderable surfaces
        renderobj_t render_base;
        renderobj_t *render_list;
        renderobj_t *render_target;
} STATE_T;

extern void oglinit(STATE_T *);
extern void oglEnd(STATE_T * state);
extern void dispmanMoveWindow(STATE_T *state, window_t *window, int x, int y);
extern void dispmanChangeWindowOpacity(window_t *window, unsigned int alpha);
extern cursor_t *createCursor(STATE_T *state, const uint32_t *data,
                              uint32_t w, uint32_t h,
                              uint32_t hx, uint32_t hy, bool upsidedown);
extern void showCursor(STATE_T *state, cursor_t *cursor);
extern void hideCursor(cursor_t *cursor);
extern void moveCursor(STATE_T *state, cursor_t *cursor, int32_t x, int32_t y);
extern void deleteCursor(cursor_t *cursor);
extern void screenBrightness(STATE_T *state, uint32_t level);
extern renderobj_t *makeRenderObjImage(STATE_T *state, VGImage image);
extern renderobj_t *addRenderObj(STATE_T *state);
extern bool delRenderObj(STATE_T *state, renderobj_t *entry);
extern renderobj_t *findRenderObjImage(STATE_T *state, VGImage image);
extern renderobj_t *findRenderObj(STATE_T *state, renderobj_t *handle);
extern bool makeRenderObjCurrent(STATE_T *state, renderobj_t *entry);
extern renderobj_t *makeRenderObjWindow(STATE_T *state, uint32_t layer,
                                        int32_t xpos, int32_t ypos,
                                        uint32_t width, uint32_t height);
extern bool changeWindowLayer(window_t *window, int32_t layer);

#endif
