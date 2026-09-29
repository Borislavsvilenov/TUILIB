#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#define WIDTH 80
#define HEIGHT 24

typedef struct {
  unsigned char fg_r, fg_g, fg_b;
  unsigned char bg_r, bg_g, bg_b;
} Color;

typedef struct {
  char ch[4];
  Color col;
} Tile;

typedef struct {
  Tile front[WIDTH][HEIGHT];
  Tile back[WIDTH][HEIGHT];
} ScreenBuffer;

typedef struct {
  char data[128 * 1024];
  size_t len;
} helperBuf;

static ScreenBuffer buf = {0};

void gridClear(void) {
  for (int i = 0; i < WIDTH; i++) {
    for (int j = 0; j < HEIGHT; j++) {
      Tile* t = &buf.back[i][j];

      strcpy(t->ch, " ");
      t->col.fg_r = 255; t->col.fg_g = 255; t->col.fg_b = 255;
      t->col.bg_r = 0; t->col.bg_g = 0; t->col.bg_b = 0;
    }
  }
}

void gridSetChar(int x, int y, const char* ch, const Color col) {
  Tile* t = &buf.back[x][y];

  if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;

  strncpy(t->ch, ch, 4);
  t->ch[3] = '\0';
  t->col = col;
}

void gridSetString(int x, int y, const char* str, const Color col) {
  int curr_x = x;
  while( *str && curr_x < WIDTH ) {
    char ch_buf[2] = { *str, '\0' };
    gridSetChar(curr_x, y, ch_buf, col);
    curr_x++;
    str++;
  }
}

void drawBoxR(int ox, int oy, int ex, int ey, const Color col) {
  if(ox < 0) ox = 0;
  if(oy < 0) oy = 0;
  if(ex >= WIDTH) ex = WIDTH;
  if(ey >= HEIGHT) ey = HEIGHT;

  Tile* t;

  t = &buf.back[ox][oy];
  strcpy(t->ch, "╭");
  t->col = col;

  t = &buf.back[ex][oy];
  strcpy(t->ch, "╮");
  t->col = col;

  t = &buf.back[ox][ey];
  strcpy(t->ch, "╰");
  t->col = col;
  
  t = &buf.back[ex][ey];
  strcpy(t->ch, "╯");
  t->col = col;

  for(int i = ox+1; i < ex; i++) {
    t = &buf.back[i][oy];
    strcpy(t->ch, "─");
    t->col = col;

    t = &buf.back[i][ey];
    strcpy(t->ch, "─");
    t->col = col;
  }

  for(int i = oy+1; i < ey; i++) {
    t = &buf.back[ox][i];
    strcpy(t->ch, "│");
    t->col = col;

    t = &buf.back[ex][i];
    strcpy(t->ch, "│");
    t->col = col;
  }
}

void appendToBuf(helperBuf* HB, const char* data) {
  size_t cap = sizeof(HB->data) - HB->len;
  size_t len = strlen(data);

  if( len > cap ) len = cap;

  memcpy(HB->data + HB->len, data, len);
  HB->len += len;
}

void gridFlush(void) {
  helperBuf HB = { .len = 0 };
  char temp[128];

  for(int j = 0; j < HEIGHT; j++) {
    for(int i = 0; i < WIDTH; i++) {
      Tile* front = &buf.front[i][j];
      Tile* back = &buf.back[i][j];

      if (memcmp(back, front, sizeof(Tile)) != 0) {
        snprintf(temp, sizeof(temp), "\x1b[%d;%dH", j+1, i+1);
        appendToBuf(&HB, temp);

        snprintf(temp, sizeof(temp), "\x1b[38;2;%d;%d;%dm\x1b[48;2;%d;%d;%dm", 
            back->col.fg_r, back->col.fg_g, back->col.fg_b,
            back->col.bg_r, back->col.bg_g, back->col.bg_b);
        appendToBuf(&HB, temp);

        appendToBuf(&HB, back->ch);

        *front = *back;
      }
    }
  }

  if (HB.len > 0) {
    write(STDOUT_FILENO, HB.data, HB.len);
  }
}
