/* example use of TUILIB */

#include "TUILIB.c"

int main(void) {
    printf("\x1b[2J\x1b[?25l"); 
    
    while (1) {
        gridClear();

        gridSetString(2, 1, "TUILIB test", (Color){255, 255, 0, 0, 0, 0});
        gridSetString(2, 2, "###", (Color){0, 255, 0, 0, 0, 0});

        drawBoxR(0, 0, 20, 20, (Color){0, 0, 255, 0, 0, 0});

        gridFlush();

        usleep(16666);
    }

    return 0;
}
