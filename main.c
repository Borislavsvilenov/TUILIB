/* example use of TUILIB */

#include "TUILIB.c"

int main(void) {
    printf("\x1b[2J\x1b[?25l"); 
    
    int cursor_x = 10, cursor_y = 10;

    while (1) {
        gridClear();

        gridSetString(2, 1, "Double-Buffered C TUI Engine", (Color){255, 255, 0, 0, 0, 0});
        gridSetChar(cursor_x, cursor_y, "█", (Color){0, 255, 128, 0, 0, 0});

        gridFlush();

        usleep(16666);
    }

    return 0;
}
