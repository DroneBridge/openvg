#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <EGL/egl.h>
#include <VG/openvg.h>
#include "bcm_host.h"
#include "eglstate.h"
#include "oglinit.h"

static inline int32_t limit(int32_t x, int32_t min, int32_t max)
{
        return x < min ? min : (x > max ? max : x);
}

// setWindowParams sets the dispmanx rects used for displaying the windows,
// Need to prevent it from going fully off screen.
static void setWindowParams(STATE_T * state, window_t * window, VC_RECT_T * src_rect, VC_RECT_T * dst_rect) {
	uint32_t dx, dy, w, h, sx, sy;
        int32_t min_x, min_y, max_x, max_y;

        if (window == &state->render_base.window) {
                min_x = 0;
                min_y = 0;
                max_x = state->screen_width;
                max_y = state->screen_height;
        }
        else {
                min_x = state->render_base.window.xpos;
                min_y = state->render_base.window.ypos;
                max_x = state->render_base.window.width + min_x;
                max_y = state->render_base.window.height + min_y;
        }
                
	// Set source & destination rectangles so that the image is
	// clipped if it goes off screen.
	if (window->xpos < (min_x + 1 - (int)window->width)) {   // Too far off left
		window->xpos = min_x + 1 - (int)window->width;
		dx = min_x;
		sx = window->width - 1;
		w = 1;
	} else if (window->xpos < min_x) {			   // Part of left is off
		dx = min_x;
		sx = min_x - window->xpos;
		w = window->width - sx;
	} else if (window->xpos < (int)(max_x - window->width)) {	// On
		dx = window->xpos;
		sx = 0;
		w = window->width;
	} else if (window->xpos < max_x) {	   // Part of right is off
		dx = window->xpos;
		sx = 0;
		w = max_x - window->xpos;
	} else {					   // Too far off right
		window->xpos = max_x - 1;
		dx = state->screen_width - 1;
		sx = 0;
		w = 1;
	}

	if (window->ypos < (min_y + 1 - (int)window->height)) {	   // Too far off top
		window->ypos = min_y + 1 - (int)window->height;
		dy = min_y;
		sy = window->height - 1;
		h = 1;
	} else if (window->ypos < min_y) {				   // Part of top is off
		dy = min_y;
		sy = min_y - window->ypos;
		h = window->height - sy;
	} else if (window->ypos < (int)(max_y - window->height)) {	// On
		dy = window->ypos;
		sy = 0;
		h = window->height;
	} else if (window->ypos < max_y) {	   // Part of bottom is off
		dy = window->ypos;
		sy = 0;
		h = max_y - window->ypos;
	} else {					   // Wholly off bottom
		window->ypos = max_y - 1;
		dy = max_y - 1;
		sy = 0;
		h = 1;
	}

	vc_dispmanx_rect_set(dst_rect, dx, dy, w, h);
	vc_dispmanx_rect_set(src_rect, sx << 16, sy << 16, w << 16, h << 16);
}

// oglinit sets the display, OpenVGL context and screen information
// state holds the display information
__attribute__((visibility("hidden")))
void oglinit(STATE_T * state)
{
	int32_t success = 0;
	EGLBoolean result;
	EGLint num_config;

	DISPMANX_ELEMENT_HANDLE_T dispman_element;
	DISPMANX_DISPLAY_HANDLE_T dispman_display;
	DISPMANX_UPDATE_HANDLE_T dispman_update;
	VC_RECT_T dst_rect;
	VC_RECT_T src_rect;

	static const EGLint attribute_list[] = {
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_RENDERABLE_TYPE, EGL_OPENVG_BIT,
		EGL_NONE
	};

	EGLConfig config;
        window_t *window = &state->render_base.window;
        
	// get an EGL display connection
	state->egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	assert(state->egl_display != EGL_NO_DISPLAY);

	// initialize the EGL display connection
	result = eglInitialize(state->egl_display, NULL, NULL);
	assert(EGL_FALSE != result);

	// bind OpenVG API
	eglBindAPI(EGL_OPENVG_API);

	// get an appropriate EGL frame buffer configuration
	result = eglChooseConfig(state->egl_display, attribute_list, &config,
                                 1, &num_config);
	assert(EGL_FALSE != result);

	// create an EGL rendering context
	window->context = eglCreateContext(state->egl_display, config,
                                           EGL_NO_CONTEXT, NULL);
	assert(window->context != EGL_NO_CONTEXT);

	// create an EGL window surface
	success = graphics_get_display_size(0, &state->screen_width,
					    &state->screen_height);
	assert(success >= 0);
        state->screen_pitch = 4 * ((state->screen_width + 15) & ~15);
        
	if ((window->width == 0) || (window->width > state->screen_width))
		window->width = state->screen_width;
	if ((window->height == 0) || (window->height > state->screen_height))
		window->height = state->screen_height;

	// adjust position so that at least one pixel is on screen and
	// set up the dispman rects
	setWindowParams(state, window, &src_rect, &dst_rect);

	dispman_display = vc_dispmanx_display_open(0 /* LCD */ );
	state->dmx_display = dispman_display;
	dispman_update = vc_dispmanx_update_start(0);

        window->layer = 0;
	window->alpha = (VC_DISPMANX_ALPHA_T){
		DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS,
		255, 0
	};
	dispman_element = vc_dispmanx_element_add(dispman_update,
                                                  dispman_display, 0,
                                                  &dst_rect, 0,
						  &src_rect,
                                                  DISPMANX_PROTECTION_NONE,
                                                  &window->alpha, 0,
                                                  DISPMANX_NO_ROTATE);

	window->element = dispman_element;
        window->nativewindow.element = dispman_element;
        window->nativewindow.width = window->width;
        window->nativewindow.height = window->height;
	vc_dispmanx_update_submit_sync(dispman_update);

	window->surface = eglCreateWindowSurface(state->egl_display, config,
                                                 &window->nativewindow, NULL);
	assert(window->surface != EGL_NO_SURFACE);

	// preserve the buffers on swap
	result = eglSurfaceAttrib(state->egl_display, window->surface,
                                  EGL_SWAP_BEHAVIOR, EGL_BUFFER_PRESERVED);
	assert(EGL_FALSE != result);

	// connect the context to the surface
	result = eglMakeCurrent(state->egl_display, window->surface,
                                window->surface, window->context);
	assert(EGL_FALSE != result);

        state->render_base.prev = NULL;
        state->render_base.next = NULL;
        state->render_base.type = RENDEROBJ_MAIN;
        state->render_list = &state->render_base;
        state->render_target = &state->render_base;
}

__attribute__((visibility("hidden")))
void oglEnd(STATE_T * state)
{
        renderobj_t *curr;
        
        eglMakeCurrent(state->egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE,
                       EGL_NO_CONTEXT);
        for(curr=state->render_list; curr != NULL; curr = curr->next) {
                delRenderObj(state, curr);
        }
        
	eglDestroySurface(state->egl_display,
                          state->render_base.window.surface);
	eglDestroyContext(state->egl_display,
                          state->render_base.window.context);
	eglTerminate(state->egl_display);
        state->egl_display = 0;
}

// dispmanMoveWindow repositions the openVG window to given coords
// -ve coords are allowed upto (1-width,1-height),
// max (screen_width-1,screen_height-1). i.e. at least one pixel must be
// on the screen.
__attribute__((visibility("hidden")))
void dispmanMoveWindow(STATE_T * state, window_t * window,
                       int32_t x, int32_t y) {
	VC_RECT_T src_rect, dst_rect;
	DISPMANX_UPDATE_HANDLE_T update;
        bool move_sub;
        int32_t offset_x, offset_y;

        if (window == &state->render_base.window) {
                offset_x = x - window->xpos;
                offset_y = y - window->ypos;
                window->xpos = x;
                window->ypos = y;
                move_sub = true;
        }
        else {
                window->xpos = x + state->render_base.window.xpos;
                window->ypos = y + state->render_base.window.ypos;
                move_sub = false;
        }

	update = vc_dispmanx_update_start(0);
        setWindowParams(state, window, &src_rect, &dst_rect);
	vc_dispmanx_element_change_attributes(update,
                                              window->element, 0, 0, 0,
                                              &dst_rect, &src_rect, 0,
                                              DISPMANX_NO_ROTATE);
        if (move_sub) {
                renderobj_t *entry;
                for(entry = state->render_list; entry != NULL; entry = entry->next) {
                        if (entry->type == RENDEROBJ_WINDOW) {
                                entry->window.xpos += offset_x;
                                entry->window.ypos += offset_y;
                                setWindowParams(state, &entry->window,
                                                &src_rect, &dst_rect);
                                vc_dispmanx_element_change_attributes(update, entry->window.element, 0, 0, 0,
                                                                      &dst_rect, &src_rect, 0, DISPMANX_NO_ROTATE);
                        }
                }
        }
        
	vc_dispmanx_update_submit_sync(update);
        /*****
         * TODO: Move sub windows relative to the main window
         *****/
}

// dispmanChangeWindowOpacity changes the window's opacity
// 0 = transparent, 255 = opaque
__attribute__((visibility("hidden")))
void dispmanChangeWindowOpacity(window_t * window, uint32_t alpha) {
	DISPMANX_UPDATE_HANDLE_T dispman_update;
        
        if (window->alpha.flags != DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS)
                return;

	if (alpha > 255)
		alpha = 255;

        dispman_update = vc_dispmanx_update_start(0);
	// The 1<<1 below means update the alpha value
	vc_dispmanx_element_change_attributes(dispman_update, window->element,
                                              1 << 1, 0, (uint8_t) alpha,
                                              0, 0, 0, DISPMANX_NO_ROTATE);
	vc_dispmanx_update_submit_sync(dispman_update);
}

// Custom cursor pointer code.

static inline int align_up(int x, int y) {
	return (x + y - 1) & ~(y - 1);
}

// Create a cursor
__attribute__((visibility("hidden")))
cursor_t *createCursor(STATE_T * state, const uint32_t * data, uint32_t w, uint32_t h, uint32_t hx, uint32_t hy, bool upsidedown) {
	if (w == 0 || w > state->render_base.window.width ||
            h == 0 || h > state->render_base.window.height ||
            hx >= w || hy >= h)
		return NULL;
	int32_t pitch = align_up(w * 4, 64);
	cursor_t *cursor = calloc(1, sizeof *cursor);
	if (cursor == NULL)
		return NULL;

	cursor->window.width = w;
	cursor->window.height = h;
	cursor->hot_x = (int32_t) hx;
	cursor->hot_y = (int32_t) hy;

	// Copy image data
	char *image = calloc(1, pitch * h);
	if (image == NULL) {
		free(cursor);
		return NULL;
	}
	uint32_t img_p;
	cursor->resource = vc_dispmanx_resource_create(VC_IMAGE_RGBA32,
                                                       w, h, &img_p);
	if (cursor->resource == 0) {
		free(image);
		free(cursor);
		return NULL;
	}

	uint32_t row;
	int32_t incr = (int32_t) w;
	if (upsidedown) {
		data += w * (h - 1);
		incr = -incr;
	}
	for (row = 0; row < h; row++) {
		memcpy(image + (row * pitch), data, w * 4);
		data += incr;
	}

	VC_RECT_T dst_rect = { 0, 0, w, h };
	vc_dispmanx_resource_write_data(cursor->resource, VC_IMAGE_RGBA32,
                                        pitch, image, &dst_rect);
	free(image);
	cursor->window.element = 0;
	return cursor;
}

// Show the cursor on screen
__attribute__((visibility("hidden")))
void showCursor(STATE_T * state, cursor_t * cursor) {
	if (cursor && !cursor->window.element) {
		VC_RECT_T src_rect, dst_rect;
                DISPMANX_ELEMENT_HANDLE_T element;
		DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
		setWindowParams(state, &cursor->window, &src_rect, &dst_rect);
                element = vc_dispmanx_element_add(update, state->dmx_display,
                                                  CURSOR_LAYER, &dst_rect,
                                                  cursor->resource, &src_rect,
                                                  DISPMANX_PROTECTION_NONE,
                                                  NULL, NULL, VC_IMAGE_ROT0);
                cursor->window.element = element;
		vc_dispmanx_update_submit_sync(update);
	}
}

// Hide the cursor
__attribute__((visibility("hidden")))
void hideCursor(cursor_t * cursor) {
	if (cursor && cursor->window.element) {
		DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
		vc_dispmanx_element_remove(update, cursor->window.element);
		vc_dispmanx_update_submit_sync(update);
		cursor->window.element = 0;
	}
}

// Move the cursor
__attribute__((visibility("hidden")))
void moveCursor(STATE_T * state, cursor_t * cursor, int32_t x, int32_t y) {
/*
        if (x < 0)
                x = 0;
        if (x >= (int32_t) state->render_base.window.width)
                x = (int32_t) state->render_base.window.width - 1;
        if (y < 0)
                y = 0;
        if (y >= (int32_t) state->render_base.window.height)
                y = (int32_t) state->render_base.window.height - 1;
*/
        x = limit(x, 0, (int32_t)state->render_base.window.width - 1);
        y = limit(y, 0, (int32_t)state->render_base.window.height - 1);
        VC_RECT_T src_rect, dst_rect;
        cursor->xpos = x;
        cursor->ypos = y;
        cursor->window.xpos = state->render_base.window.xpos + x - cursor->hot_x;
        cursor->window.ypos = state->render_base.window.ypos + y - cursor->hot_y;
        setWindowParams(state, &cursor->window, &src_rect, &dst_rect);
        if (cursor->window.element) {
                DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
                vc_dispmanx_element_change_attributes(update,
                                                      cursor->window.element,
                                                      0, 0, 0,
                                                      &dst_rect, &src_rect, 0,
                                                      DISPMANX_NO_ROTATE);
                vc_dispmanx_update_submit_sync(update);
	}
}

// Delete a cursor
__attribute__((visibility("hidden")))
void deleteCursor(cursor_t * cursor) {
        if (cursor->window.element) {
                DISPMANX_UPDATE_HANDLE_T update;
                update = vc_dispmanx_update_start(0);
                vc_dispmanx_element_remove(update, cursor->window.element);
                vc_dispmanx_update_submit_sync(update);
        }
        vc_dispmanx_resource_delete(cursor->resource);
        free(cursor);
}

// Dim screen
// level goes from 0 = black -> 255 = Normal. Over 255 is clamped.
__attribute__((visibility("hidden")))
void screenBrightness(STATE_T * state, uint32_t level) {
	static uint32_t brightnessLevel = 255;
	static DISPMANX_RESOURCE_HANDLE_T brightnessLayer = 0;
	static DISPMANX_ELEMENT_HANDLE_T brightnessElement = 0;
        static VC_DISPMANX_ALPHA_T alpha = {
                DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS,
                255, 0
        };
        int ret;

	if (level > 255)
		level = 255;
	if (brightnessLevel == level)
		return;

        VC_RECT_T src_rect, dst_rect;
	if (!brightnessLayer) {
		uint32_t img_p;
		brightnessLayer = vc_dispmanx_resource_create(VC_IMAGE_RGBA32,
                                                              1, 1, &img_p);
		if (!brightnessLayer)
			return;
                uint32_t image = 0;
                vc_dispmanx_rect_set(&dst_rect, 0, 0, 1, 1);
		ret = vc_dispmanx_resource_write_data(brightnessLayer,
                                                      VC_IMAGE_RGBA32,
                                                      sizeof image, &image,
                                                      &dst_rect);
                if (ret) {
                        vc_dispmanx_resource_delete(brightnessLayer);
                        brightnessLayer = 0;
                        return;
                }
	}

	brightnessLevel = level;

        DISPMANX_UPDATE_HANDLE_T update;
	if (level == 255) {
		update = vc_dispmanx_update_start(0);
		vc_dispmanx_element_remove(update, brightnessElement);
		vc_dispmanx_update_submit_sync(update);
		vc_dispmanx_resource_delete(brightnessLayer);
		brightnessLayer = 0;
		brightnessElement = 0;
	} else {
		if (brightnessElement) {
			update = vc_dispmanx_update_start(0);
			vc_dispmanx_element_change_attributes(update, brightnessElement, 1 << 1, 0, (uint8_t) (255 - level), 0, 0,
							      0, DISPMANX_NO_ROTATE);
			vc_dispmanx_update_submit_sync(update);
		} else {
                        alpha.opacity = 255 - level;
			update = vc_dispmanx_update_start(0);
                        vc_dispmanx_rect_set(&src_rect, 0, 0, 1 << 16, 1 << 16);
                        vc_dispmanx_rect_set(&dst_rect, 0, 0,
                                             state->screen_width,
                                             state->screen_height);
			brightnessElement = vc_dispmanx_element_add(update,
                                                                    state->dmx_display, DIM_LAYER, &dst_rect,
                                                                    brightnessLayer, &src_rect,
                                                                    DISPMANX_PROTECTION_NONE, &alpha, NULL, VC_IMAGE_ROT0);
			vc_dispmanx_update_submit_sync(update);
		}
	}
}

////
// OpenVG render targets
////
// Images

// Allocate a new render object whose drawable is an OpenVG Image
__attribute__((visibility("hidden")))
renderobj_t *makeRenderObjImage(STATE_T *state, VGImage image)
{
        static const EGLint attribute_list[] = {
                EGL_RED_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
                EGL_RENDERABLE_TYPE, EGL_OPENVG_BIT,
		EGL_NONE
	};
        EGLContext context;
        EGLConfig config;
        EGLBoolean result;
        EGLint num_configs;
        
        result = eglChooseConfig(state->egl_display, attribute_list,
                                 &config, 1, &num_configs);
        if (result == EGL_FALSE)
                return NULL;
        context = eglCreateContext(state->egl_display, config,
                                   state->render_base.window.context, NULL);
        if (context == NULL)
                return NULL;

        EGLSurface surface;
        surface = eglCreatePbufferFromClientBuffer(state->egl_display,
                                                   EGL_OPENVG_IMAGE,
                                                   (EGLClientBuffer)image,
                                                   config, NULL);
        if (surface == NULL) {
                eglDestroyContext(state->egl_display, context);
                return NULL;
        }
        
        renderobj_t *entry = addRenderObj(state);
        if (entry != NULL) {
                entry->type = RENDEROBJ_IMAGE;
                entry->image = image;
                entry->window.width = vgGetParameteri(image, VG_IMAGE_WIDTH);
                entry->window.height = vgGetParameteri(image, VG_IMAGE_HEIGHT);
                entry->window.context = context;
                entry->window.surface = surface;
        }
        else {
                eglDestroySurface(state->egl_display, surface);
                eglDestroyContext(state->egl_display, context);
        }
        return entry;
}

// Allocate a render object and add it to the state's render object list
__attribute__((visibility("hidden")))
renderobj_t *addRenderObj(STATE_T *state)
{
        renderobj_t *entry = calloc(1, sizeof *entry);
        if (entry != NULL) {
                entry->prev = NULL;
                entry->next = state->render_list;
                if (state->render_list != NULL)
                        state->render_list->prev = entry;
                state->render_list = entry;
        }
        return entry;
}

// Deallocate render object and remove it from the state's list.
// This assumes that the object is in the list (it's the only list),
// don't allow the object to be deleted if it is active.
__attribute__((visibility("hidden")))
bool delRenderObj(STATE_T *state, renderobj_t *entry)
{
        if (entry->type == RENDEROBJ_NONE || entry->type == RENDEROBJ_MAIN
            || state->render_target == entry)
                return false;

        EGLBoolean res;

        if (entry->type == RENDEROBJ_WINDOW) {
		if (entry->window.element) {
			DISPMANX_UPDATE_HANDLE_T update;
			update = vc_dispmanx_update_start(0);
			vc_dispmanx_element_remove(update, entry->window.element);
			vc_dispmanx_update_submit_sync(update);
                        entry->window.element = 0;
		}
        }
        
        if (entry->type == RENDEROBJ_IMAGE || entry->type == RENDEROBJ_WINDOW) {
                res = eglDestroySurface(state->egl_display,
                                        entry->window.surface);
                if (res == EGL_FALSE)
                        return false;
                res = eglDestroyContext(state->egl_display,
                                        entry->window.context);
                if (res == EGL_FALSE) {
                        entry->window.surface = 0;
                        return false;
                }
        }
        
        renderobj_t *prev = entry->prev;
        renderobj_t *next = entry->next;
        if (next != NULL)
                next->prev = prev;
        if (prev != NULL)
                prev->next = next;
        else
                state->render_list = next;
        free(entry);
        return true;
}

// Find the render object related to the OpenVG Image in the state's list
__attribute__((visibility("hidden")))
renderobj_t *findRenderObjImage(STATE_T *state, VGImage image)
{
        renderobj_t *curr;
        for(curr = state->render_list; curr != NULL; curr = curr->next) {
                if (curr->type == RENDEROBJ_IMAGE && curr->image == image)
                        break;
        }
        return curr;
}

// Make the render object the current render target
__attribute__((visibility("hidden")))
bool makeRenderObjCurrent(STATE_T *state, renderobj_t *entry)
{
        EGLBoolean result = EGL_FALSE;
        window_t *window = &entry->window;
        if (window->surface && window->context) {
                result = eglMakeCurrent(state->egl_display, window->surface,
                                        window->surface, window->context);
                if (result == EGL_TRUE) {
                        state->render_target = entry;
                }
        }
        return result == EGL_TRUE;
}

/////
// Windows

// Allocate a new render object whose drawable is a window
__attribute__((visibility("hidden")))
renderobj_t *makeRenderObjWindow(STATE_T *state, uint32_t layer,
                                 int32_t xpos, int32_t ypos,
                                 uint32_t width, uint32_t height)
{
        static const EGLint attribute_list[] = {
                EGL_RED_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_RENDERABLE_TYPE, EGL_OPENVG_BIT,
		EGL_NONE
	};
        EGLContext context;
        EGLConfig config;
        EGLBoolean result;
        EGLint num_configs;

        if (layer == CURSOR_LAYER || layer == DIM_LAYER)
                return NULL;
        
        result = eglChooseConfig(state->egl_display, attribute_list,
                                 &config, 1, &num_configs);
        if (result == EGL_FALSE)
                return NULL;
        context = eglCreateContext(state->egl_display, config,
                                   state->render_base.window.context, NULL);
        if (context == NULL)
                return NULL;

        renderobj_t *entry = addRenderObj(state);
        if (entry == NULL) {
                eglDestroyContext(state->egl_display, context);
                return NULL;
        }

        window_t *window = &entry->window;
        entry->type = RENDEROBJ_WINDOW;
        window->width = width;
        window->height = height;
        window->xpos = xpos + state->render_base.window.xpos;
        window->ypos = ypos + state->render_base.window.ypos;
        window->layer = layer;

        VC_RECT_T src_rect, dst_rect;
        setWindowParams(state, window, &src_rect, &dst_rect);
	DISPMANX_UPDATE_HANDLE_T dispman_update;
	dispman_update = vc_dispmanx_update_start(0);
	window->element = vc_dispmanx_element_add(dispman_update,
                                                  state->dmx_display, layer,
                                                  &dst_rect, 0, &src_rect,
                                                  DISPMANX_PROTECTION_NONE, 0,
                                                  0, DISPMANX_NO_ROTATE);

	vc_dispmanx_update_submit_sync(dispman_update);
        window->context = context;
        window->nativewindow.element = window->element;
        window->nativewindow.width = width;
        window->nativewindow.height = height;
	window->surface = eglCreateWindowSurface(state->egl_display, config,
                                                 &window->nativewindow, NULL);
	if (window->surface == EGL_NO_SURFACE) {
                delRenderObj(state, entry);
                return NULL;
        }

	result = eglSurfaceAttrib(state->egl_display, window->surface,
                                  EGL_SWAP_BEHAVIOR, EGL_BUFFER_PRESERVED);
	if (EGL_FALSE == result) {
                delRenderObj(state, entry);
                return NULL;
        }
        return entry;
}

// Find the render object related to the handle in the state's list.
// A handle is really a pointer to the internal struct but traverse
// the list to make sure we were really passed a valid handle.
__attribute__((visibility("hidden")))
renderobj_t *findRenderObj(STATE_T *state, renderobj_t *handle)
{
        renderobj_t *curr;
        for(curr = state->render_list; curr != NULL; curr = curr->next) {
                if (curr == handle)
                        break;
        }
        return curr;
}

// Change the layer that the window appears on.
// layer cannot be CURSOR_LAYER or DIM_LAYER as these are reserved
__attribute__((visibility("hidden")))
bool changeWindowLayer(window_t *window, int32_t layer)
{
        int32_t ret = ~0;
        if (layer != CURSOR_LAYER && layer != DIM_LAYER &&
            window != NULL && window->element != 0) {
                DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
                if (update) {
                        ret = vc_dispmanx_element_change_layer(update, window->element, layer);
                        ret |= vc_dispmanx_update_submit_sync(update);
                }
	}
        return ret == 0;
}
// Take a copy of an area of the entire screen ready for saving
__attribute__((visibility("hidden")))
char *grabScreen(STATE_T *state) {
        uint32_t width = state->screen_width;
        uint32_t height = state->screen_height;
        uint32_t pitch = state->screen_pitch;
        uint32_t vc_err;
	char *screen_buffer = malloc((size_t) (pitch * height));
	if (screen_buffer != NULL) {
                DISPMANX_RESOURCE_HANDLE_T screenshot;
                screenshot = vc_dispmanx_resource_create(VC_IMAGE_RGBA32, width, height, &vc_err);
                if (screenshot != 0) {
                        vc_err = vc_dispmanx_snapshot(state->dmx_display, screenshot, 0);
                        if (vc_err == 0) {
                                VC_RECT_T area;
                                vc_dispmanx_rect_set(&area, 0, 0, width, height);
                                vc_err = vc_dispmanx_resource_read_data(screenshot, &area, screen_buffer, pitch);
                                if (vc_err != 0) {
                                        fprintf(stderr, "ERROR %s:%d libshapes failed to read vc_resource.\n", __FUNCTION__, __LINE__);
                                }
                        }
                        else {
                                fprintf(stderr, "ERROR %s:%d libshapes failed to take vg_snapshot.\n", __FUNCTION__, __LINE__);
                        }
                        vc_dispmanx_resource_delete(screenshot);
                }
                else {
                        fprintf(stderr, "ERROR: %s:%d libshapes failed to create vc_resource\n", __FUNCTION__, __LINE__);
                        vc_err = -1;
                }
                if (vc_err != 0) {
                        free(screen_buffer);
                        screen_buffer = NULL;
                }
        }
        else {
                fprintf(stderr, "ERROR: %s:%d libshapes failed to allocate memory for screenshot.\n", __FUNCTION__, __LINE__);
        }
	return screen_buffer;
}
