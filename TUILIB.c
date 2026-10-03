#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <locale.h>
#include <stdbool.h>
#include <stdint.h>

#define WIDTH 80
#define HEIGHT 24

typedef struct {
  int x;
  int y;
} Vec2;

Vec2 add(Vec2 a, Vec2 b) {
  return (Vec2){a.x + b.x, a.y + b.y};
}

typedef struct {
  Vec2 size;
  uint8_t* data;
} Matrix;

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
} HelperBuf;

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

void drawChar(Vec2 pos, const char* ch, const Color col) {
  Tile* t = &buf.back[pos.x][pos.y];

  if(pos.x < 0 || pos.x >= WIDTH || pos.y < 0 || pos.y >= HEIGHT) return;

  strncpy(t->ch, ch, 4);
  t->ch[3] = '\0';
  t->col = col;
}

void drawFullBlock(Vec2 pos, const Color col) {
  drawChar(pos, "█", col);
}

void drawTopBlock(Vec2 pos, const Color col) {
  drawChar(pos, "▀", col);
}

void drawBottomBlock(Vec2 pos, const Color col) {
  drawChar(pos, "▄", col);
}

void drawLeftBlock(Vec2 pos, const Color col) {
  drawChar(pos, "▌", col);
}

void drawRightBlock(Vec2 pos, const Color col) {
  drawChar(pos, "▐", col);
}

void drawSquare(Vec2 pos, const Color col) {
  drawChar(pos, "■", col);
}

void drawCircle(Vec2 pos, const Color col) {
  drawChar(pos, "●", col);
}

void drawTriangleUp(Vec2 pos, const Color col) {
  drawChar(pos, "▲", col);
}

void drawTriangleDown(Vec2 pos, const Color col) {
  drawChar(pos, "▼", col);
}

void drawDiamond(Vec2 pos, const Color col) {
  drawChar(pos, "◆", col);
}

void drawString(Vec2 pos, const char* str, const Color col) {
  int curr_x = pos.x;
  while( *str && curr_x < WIDTH ) {
    char ch_buf[2] = { *str, '\0' };
    drawChar((Vec2){curr_x, pos.y}, ch_buf, col);
    curr_x++;
    str++;
  }
}

void drawBox(Vec2 opos, Vec2 epos, const Color col) {
  if(opos.x < 0) opos.x = 0;
  if(opos.y < 0) opos.y = 0;
  if(epos.x >= WIDTH) epos.x = WIDTH;
  if(epos.y >= HEIGHT) epos.y = HEIGHT;

  Tile* t;

  t = &buf.back[opos.x][opos.y];
  strcpy(t->ch, "┌");
  t->col = col;

  t = &buf.back[epos.x][opos.y];
  strcpy(t->ch, "┐");
  t->col = col;

  t = &buf.back[opos.x][epos.y];
  strcpy(t->ch, "└");
  t->col = col;
  
  t = &buf.back[epos.x][epos.y];
  strcpy(t->ch, "┘");
  t->col = col;

  for(int i = opos.x+1; i < epos.x; i++) {
    t = &buf.back[i][opos.y];
    strcpy(t->ch, "─");
    t->col = col;

    t = &buf.back[i][epos.y];
    strcpy(t->ch, "─");
    t->col = col;
  }

  for(int i = opos.y+1; i < epos.y; i++) {
    t = &buf.back[opos.x][i];
    strcpy(t->ch, "│");
    t->col = col;

    t = &buf.back[epos.x][i];
    strcpy(t->ch, "│");
    t->col = col;
  }
}

void drawBoxR(Vec2 opos, Vec2 epos, const Color col) {
  if(opos.x < 0) opos.x = 0;
  if(opos.y < 0) opos.y = 0;
  if(epos.x >= WIDTH) epos.x = WIDTH;
  if(epos.y >= HEIGHT) epos.y = HEIGHT;

  Tile* t;

  t = &buf.back[opos.x][opos.y];
  strcpy(t->ch, "╭");
  t->col = col;

  t = &buf.back[epos.x][opos.y];
  strcpy(t->ch, "╮");
  t->col = col;

  t = &buf.back[opos.x][epos.y];
  strcpy(t->ch, "╰");
  t->col = col;
  
  t = &buf.back[epos.x][epos.y];
  strcpy(t->ch, "╯");
  t->col = col;

  for(int i = opos.x+1; i < epos.x; i++) {
    t = &buf.back[i][opos.y];
    strcpy(t->ch, "─");
    t->col = col;

    t = &buf.back[i][epos.y];
    strcpy(t->ch, "─");
    t->col = col;
  }

  for(int i = opos.y+1; i < epos.y; i++) {
    t = &buf.back[opos.x][i];
    strcpy(t->ch, "│");
    t->col = col;

    t = &buf.back[epos.x][i];
    strcpy(t->ch, "│");
    t->col = col;
  }
}

void drawBoxD(Vec2 opos, Vec2 epos, const Color col) {
  if(opos.x < 0) opos.x = 0;
  if(opos.y < 0) opos.y = 0;
  if(epos.x >= WIDTH) epos.x = WIDTH;
  if(epos.y >= HEIGHT) epos.y = HEIGHT;

  Tile* t;

  t = &buf.back[opos.x][opos.y];
  strcpy(t->ch, "╔");
  t->col = col;

  t = &buf.back[epos.x][opos.y];
  strcpy(t->ch, "╗");
  t->col = col;

  t = &buf.back[opos.x][epos.y];
  strcpy(t->ch, "╚");
  t->col = col;
  
  t = &buf.back[epos.x][epos.y];
  strcpy(t->ch, "╝");
  t->col = col;

  for(int i = opos.x+1; i < epos.x; i++) {
    t = &buf.back[i][opos.y];
    strcpy(t->ch, "═");
    t->col = col;

    t = &buf.back[i][epos.y];
    strcpy(t->ch, "═");
    t->col = col;
  }

  for(int i = opos.y+1; i < epos.y; i++) {
    t = &buf.back[opos.x][i];
    strcpy(t->ch, "║");
    t->col = col;

    t = &buf.back[epos.x][i];
    strcpy(t->ch, "║");
    t->col = col;
  }
}

void drawBoxTitle(Vec2 opos, Vec2 epos, const Color col, const char* str, const unsigned char type) {
  if(opos.x < 0) opos.x = 0;
  if(opos.y < 0) opos.y = 0;
  if(epos.x >= WIDTH) epos.x = WIDTH;
  if(epos.y >= HEIGHT) epos.y = HEIGHT;

  switch (type) {
    case 's':
      drawBox(opos, epos, col);
      break;

    case 'r':
      drawBoxR(opos, epos, col);
      break;

    case 'd':
      drawBoxD(opos, epos, col);
      break;
  }

  int len = strlen(str);
  int mx = (opos.x + epos.x - len) / 2;

  drawString((Vec2){mx, opos.y}, str, col);
}

Matrix* initMatrix(Vec2 size) {
  Matrix* mat = malloc(sizeof(Matrix));
  mat->size = size;
  
  uint8_t bytes = (size.x / 2) * (size.y / 4);
  mat->data = calloc(bytes, sizeof(uint8_t));

  return mat;
}

void setBit(const Matrix* mat, Vec2 pos, bool val) {
  if(pos.x < 0 || pos.x >= mat->size.x || pos.y < 0 || pos.y >= mat->size.y) return;
  
  size_t idx = (pos.y / 4) * mat->size.x / 2 + (pos.x / 2);
  
  if(val) {
    mat->data[idx] |= (1 << ((pos.y % 4) * 2 + (pos.x % 2)));
  } else {
    mat->data[idx] &= ~(1 << ((pos.y % 4) * 2 + (pos.x % 2)));
  }
}

bool getBit(const Matrix* mat, Vec2 pos) {
  if(pos.x < 0 || pos.x >= mat->size.x || pos.y < 0 || pos.y >= mat->size.y) return 0;

  uint8_t cell = mat->data[(pos.y / 4) * mat->size.x + (pos.x / 2)];
  
  return cell >> ((pos.y % 4) * 2 + (pos.x % 2)) & 1;
}

uint8_t getCode(const Matrix* mat, size_t idx) {
  uint8_t code = 0;
  uint8_t pat = mat->data[idx];

  if(pat & 0x01) code |= 0x01;
  if(pat & 0x02) code |= 0x08;
  if(pat & 0x04) code |= 0x02;
  if(pat & 0x08) code |= 0x10;
  if(pat & 0x10) code |= 0x04;
  if(pat & 0x20) code |= 0x20;
  if(pat & 0x40) code |= 0x40;
  if(pat & 0x80) code |= 0x80;

  return code;
}

char* brailCode(const uint8_t code) {
  setlocale(LC_ALL, "");

  char* temp = malloc(8 * sizeof(char));
  snprintf(temp, sizeof(temp), "%lc", 0x2800 + code);

  return temp;
}

void drawBrail(const Matrix* mat, Vec2 pos, const Color col) {
  Vec2 gridPos = {0, 0};
  for(size_t i = 0; i < (mat->size.x / 2) * (mat->size.y / 4); i++) { 
    gridPos.x = i % (mat->size.x / 2);
    gridPos.y = i / (mat->size.x / 2);
 
    drawChar(add(pos, gridPos), brailCode(getCode(mat, i)), col);
  }
}

void appendToBuf(HelperBuf* HB, const char* data) {
  size_t cap = sizeof(HB->data) - HB->len;
  size_t len = strlen(data);

  if( len > cap ) len = cap;

  memcpy(HB->data + HB->len, data, len);
  HB->len += len;
}

void gridFlush(void) {
  HelperBuf HB = { .len = 0 };
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
    snprintf(temp, sizeof(temp), "\x1b[%d;%dH", HEIGHT+1, 0);
    appendToBuf(&HB, temp);

    write(STDOUT_FILENO, HB.data, HB.len);
  }
}


