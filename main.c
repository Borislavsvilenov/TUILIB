/* example use of TUILIB */

#include "TUILIB.c"

int main(void) {
    printf("\x1b[2J\x1b[?25l"); 

    Matrix* mat = initMatrix((Vec2){4, 8});

    setBit(mat, (Vec2){0, 0}, 1);
    setBit(mat, (Vec2){1, 1}, 1);
    setBit(mat, (Vec2){0, 2}, 1);
    setBit(mat, (Vec2){1, 3}, 1);

    setBit(mat, (Vec2){3, 0}, 1);
    setBit(mat, (Vec2){2, 1}, 1);
    setBit(mat, (Vec2){3, 2}, 1);
    setBit(mat, (Vec2){2, 3}, 1);

    setBit(mat, (Vec2){0, 4}, 1);
    setBit(mat, (Vec2){1, 5}, 1);
    setBit(mat, (Vec2){0, 6}, 1);
    setBit(mat, (Vec2){1, 7}, 1);

    setBit(mat, (Vec2){3, 4}, 1);
    setBit(mat, (Vec2){2, 5}, 1);
    setBit(mat, (Vec2){3, 6}, 1);
    setBit(mat, (Vec2){2, 7}, 1);
    
    while (1) {
        gridClear();

        drawString((Vec2){2, 1}, "TUILIB test", (Color){255, 255, 0, 0, 0, 0});

        drawFullBlock((Vec2){2, 2}, (Color){0, 255, 0, 0, 0, 0});
        drawTopBlock((Vec2){3, 2}, (Color){0, 255, 0, 0, 0, 0});
        drawBottomBlock((Vec2){4, 2}, (Color){0, 255, 0, 0, 0, 0});
        drawFullBlock((Vec2){5, 2}, (Color){0, 255, 0, 0, 0, 0});
    
        drawSquare((Vec2){7, 2}, (Color){0, 255, 0, 0, 0, 0});
        drawCircle((Vec2){9, 2}, (Color){0, 255, 0, 0, 0, 0});
        drawTriangleUp((Vec2){11, 2}, (Color){0, 255, 0, 0, 0, 0});
        drawTriangleDown((Vec2){13, 2}, (Color){0, 255, 0, 255, 0, 0});
        drawDiamond((Vec2){15, 2}, (Color){0, 255, 0, 0, 0, 0});

        drawBrail(mat, (Vec2){17, 2}, (Color){0, 0, 255, 0, 0, 0});

        drawBoxTitle((Vec2){0, 0}, (Vec2){79, 23}, (Color){0, 255, 255, 0, 0, 0}, "Hello World!", 'd');

        gridFlush();

        usleep(16666);
    }

    return 0;
}
