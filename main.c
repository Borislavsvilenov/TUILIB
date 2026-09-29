/* example use of TUILIB */

#include "TUILIB.c"

int main(void) {
    printf("\x1b[2J\x1b[?25l"); 
    
    while (1) {
        gridClear();

        drawString(2, 1, "TUILIB test", (Color){255, 255, 0, 0, 0, 0});

        drawFullBlock(2, 2, (Color){0, 255, 0, 0, 0, 0});
        drawTopBlock(3, 2, (Color){0, 255, 0, 0, 0, 0});
        drawBottomBlock(4, 2, (Color){0, 255, 0, 0, 0, 0});
        drawFullBlock(5, 2, (Color){0, 255, 0, 0, 0, 0});
        
        drawSquare(7, 2, (Color){0, 255, 0, 0, 0, 0});
        drawCircle(9, 2, (Color){0, 255, 0, 0, 0, 0});
        drawTriangleUp(11, 2, (Color){0, 255, 0, 0, 0, 0});
        drawTriangleDown(13, 2, (Color){0, 255, 0, 0, 0, 0});
        drawDiamond(15, 2, (Color){0, 255, 0, 0, 0, 0});

        drawBoxTitle(0, 0, 79, 23, (Color){0, 255, 255, 0, 0, 0}, "Hello World!", 'd');

        gridFlush();

        usleep(16666);
    }

    return 0;
}
