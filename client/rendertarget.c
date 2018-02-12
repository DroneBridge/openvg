#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "fontinfo.h"
#include "shapes.h"

/*
 * NOTE: Watch out with multiple transforms, they are applied in
 * reverse order! Due to this if you use rotation/scales then the
 * translate part of DrawImageAt() and DrawImageAtFit() will be
 * applied incorrectly (the translate will effectively happen *before*
 * your custom transforms, not after). I can alter it so it happens
 * last but then it will be wrong for anybody using transforms
 * correctly.
 * 
 * In the case of the rotating cursor here we need to
 * move the origin to the hotspot of the cursor (it's tip at 3,3) so
 * when we say draw it at say (400,500) we mean place the tip at that
 * position. Since the origin is normally at the bottom-left, the
 * hotspot is at (3,height-3) from there. Negative values are used for the
 * hotspot location as we need to move the image left & down.
 * So in full we want to translate the origin to the hotspot, rotate
 * the image and then translate the cursor to it's position. Doing
 * this in reverse we end up with :-
 * Translate the cursor's hotspot into position, rotate the image
 * around the hotspot and finally translate the hotspot to the origin.
 * 
 * DrawImageAt() applies the translation effectively *before* any
 * custom transforms so having the cursor position translated in
 * DrawImageAt() won't work. I'm using the raw vgDrawImage() call to
 * avoid the extra set-up (I could pass the hotspot transform to it
 * but that makes it look wrong). I could change the DrawImageAt()
 * function to apply the translation before the user's transforms...
 */

int main(void) {
	int width, height;
        bool okay;
        
        InitWindowSize(100, 100, 1720, 880);
        
        if (!InitShapes(&width, &height))
                return 1;
        EnableOpenVGErrorCheck(true);
        
        // Create an image to draw into
        VGImage my_image = vgCreateImage(VG_sABGR_8888, 100, 100, VG_IMAGE_QUALITY_BETTER);
        // Set it as the current destination for drawing
        renderobj_t *target_A;
        target_A = CreateRenderTargetToImage(my_image);
        if (target_A == NULL) {
                printf("Error in creating render target_A\n");
        }
        else {
                if (SetRenderTarget(target_A) == false) {
                        printf("Error setting render target_A\n");
                }
                else {
                        Start(100, 100);
                        BackgroundRGBA(128, 128, 128, 0.25f);
                        Fill(255, 0, 0, 0.75f);
                        Circle(50.0f, 50.0f, 40.0f);
                        End();
                }
        }

        okay = DeleteRenderTarget(target_A);
        if (!okay) {
                printf("Error releasing render target_A\n");
        }
        
        renderobj_t *sub_window = CreateRenderTargetWindow(4, 700, 450, 900, 100);
        if (sub_window == NULL) {
                printf("Error creating sub window");
        }
        else {
                okay = SetRenderTarget(sub_window);
                if (!okay) {
                        printf("Error setting sub window\n");
                }
                else {
                        Start(50, 50);
                        BackgroundRGBA(0, 0, 0, 0.0f);
                        Fill(200, 50, 200, 0.75f);
                        Roundrect(0, 0, 900, 100, 10, 10);
                        End();
                }
        }

        // Set the main window as the destination for drawing
        if (SetRenderTarget(NULL) == false) {
                printf("Error restoring main rendering context\n");
        }

	Start(width, height);
        Background(0, 0, 0);
        Fill(44, 77, 232, 1);

        int count;
        for (count = 0; count < 256; count++) {
                WindowClear();
                Circle(width / 2, 0, width); // Background
                vgLoadIdentity(); // Reset transform matrix
                if (!End()) {
                        fprintf(stderr, "Error @ count = %d\n", count);
                        break;
                }
        }
        if (sub_window) {
                DeleteRenderTarget(sub_window);
        }
        
        // It's safe to destroy objects that don't exist -
        // OpenVG will give us an error but we already know
        // that and are exiting anyway.
        vgDestroyImage(my_image);
        FinishShapes();
	return 0;
}
