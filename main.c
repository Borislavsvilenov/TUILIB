/* example use of TUILIB */

#include "TUILIB.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
  printf("\x1b[2J\x1b[?25l"); 

  initLib();
  enableRawMode();
  int running = 1;

  Matrix* mat = initMatrix((Vec2i){4, 8});

  setBit(mat, (Vec2i){0, 0}, 1);
  setBit(mat, (Vec2i){1, 1}, 1);
  setBit(mat, (Vec2i){0, 2}, 1);
  setBit(mat, (Vec2i){1, 3}, 1);

  setBit(mat, (Vec2i){3, 0}, 1);
  setBit(mat, (Vec2i){2, 1}, 1);
  setBit(mat, (Vec2i){3, 2}, 1);
  setBit(mat, (Vec2i){2, 3}, 1);

  setBit(mat, (Vec2i){0, 4}, 1);
  setBit(mat, (Vec2i){1, 5}, 1);
  setBit(mat, (Vec2i){0, 6}, 1);
  setBit(mat, (Vec2i){1, 7}, 1);

  setBit(mat, (Vec2i){3, 4}, 1);
  setBit(mat, (Vec2i){2, 5}, 1);
  setBit(mat, (Vec2i){3, 6}, 1);
  setBit(mat, (Vec2i){2, 7}, 1);

  while (running) {
    int key = readKey();

    switch (key) {
      case 'q':
      case 'Q':
      case KEY_ESC:
        running = 0;
        break;
 
      default:
        break;
    }

    drawString((Vec2i){2, 1}, "TUILIB test", (Color){255, 255, 0, 0, 0, 0});

    drawFullBlock((Vec2i){2, 2}, (Color){0, 255, 0, 0, 0, 0});
    drawTopBlock((Vec2i){3, 2}, (Color){0, 255, 0, 0, 0, 0});
    drawBottomBlock((Vec2i){4, 2}, (Color){0, 255, 0, 0, 0, 0});
    drawFullBlock((Vec2i){5, 2}, (Color){0, 255, 0, 0, 0, 0});

    drawSquare((Vec2i){7, 2}, (Color){0, 255, 0, 0, 0, 0});
    drawCircle((Vec2i){9, 2}, (Color){0, 255, 0, 0, 0, 0});
    drawTriangleUp((Vec2i){11, 2}, (Color){0, 255, 0, 0, 0, 0});
    drawTriangleDown((Vec2i){13, 2}, (Color){0, 255, 0, 255, 0, 0});
    drawDiamond((Vec2i){15, 2}, (Color){0, 255, 0, 0, 0, 0});

    drawBrail(mat, (Vec2i){17, 2}, (Color){0, 0, 255, 0, 0, 0});

    drawBoxTitle((Vec2i){0, 0}, (Vec2i){WIDTH-1, HEIGHT-1}, (Color){0, 255, 255, 0, 0, 0}, "Hello World!", 'd');

    drawBoxText((Vec2i){1, 6}, (Vec2i){8, 8}, (Color){0, 255, 0, 0, 0, 0}, "TEXT", 'd');

    updateFrame();

    usleep(16666);
  }

  free(mat);

  closeLib();

  return 0;
}
