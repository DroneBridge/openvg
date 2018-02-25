#ifndef LIBSHAPES_OGLINIT_H
#define LIBSHAPES_OGLINIT_H

#include <stdbool.h>
#include <VG/openvg.h>
#include "eglstate.h"

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
extern char *grabScreen(STATE_T *state);

#endif
