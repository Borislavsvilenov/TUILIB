#include "TUILIB.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>

int WIDTH = 80;
int HEIGHT = 24;
size_t FRAMECOUNT = 0;
volatile sig_atomic_t g_resized = 0;
struct termios orig_termios;
struct sigaction sa;
DoubleBuffer buf;
ElementBuffer elementBuf;

void handle_sigwinch(int sig) {
  (void)sig;
  g_resized = 1;
}

void getTerminalSize() {
  struct winsize ws;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
    WIDTH = ws.ws_col;
    HEIGHT = ws.ws_row;
  } else {
    WIDTH = 80;
    HEIGHT = 24;
  }
}

void disableRawMode(void) {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode(void) {
  tcgetattr(STDIN_FILENO, &orig_termios);
  atexit(disableRawMode);

  struct termios raw = orig_termios;

  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

  raw.c_cc[VMIN] = 0; 
  raw.c_cc[VTIME] = 1; 

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int readKey(void) {
  char c;
  ssize_t nread = read(STDIN_FILENO, &c, 1);
  if (nread <= 0) return KEY_NONE;

  if (c == '\x1b') {
    char seq[3];

    if (read(STDIN_FILENO, &seq[0], 1) <= 0) return KEY_ESC;
    if (read(STDIN_FILENO, &seq[1], 1) <= 0) return KEY_ESC;

    if (seq[0] == '[') {
      if (seq[1] >= '0' && seq[1] <= '9') {
        if (read(STDIN_FILENO, &seq[2], 1) <= 0) return KEY_ESC;
        if (seq[2] == '~') {
          switch (seq[1]) {
            case '3': return KEY_DELETE;
          }
        }
      } else {
        switch (seq[1]) {
          case 'A': return KEY_ARROW_UP;
          case 'B': return KEY_ARROW_DOWN;
          case 'C': return KEY_ARROW_RIGHT;
          case 'D': return KEY_ARROW_LEFT;
          case 'H': return KEY_HOME;
          case 'F': return KEY_END;
        }
      }
    }
    return KEY_ESC;
  }

  return (unsigned char)c;
}

void enableMouse(void) {
    printf("\x1b[?1000h\x1b[?1006h");
    fflush(stdout);
}

void disableMouse(void) {
    printf("\x1b[?1000l\x1b[?1006l");
    fflush(stdout);
}

void gridClear(void) {
  for (int i = 0; i < WIDTH; i++) {
    for (int j = 0; j < HEIGHT; j++) {
      Tile* t = &buf.back[j * WIDTH + i];

      strcpy(t->ch, " ");
      t->col.fg_r = 0; t->col.fg_g = 0; t->col.fg_b = 0;
      t->col.bg_r = 0; t->col.bg_g = 0; t->col.bg_b = 0;
    }
  }
}

void drawChar(Vec2i pos, const char* ch, const Color col) {
  Tile* t = &buf.back[pos.y * WIDTH + pos.x];

  if(pos.x < 0 || pos.x >= WIDTH || pos.y < 0 || pos.y >= HEIGHT) return;

  strncpy(t->ch, ch, 4);
  t->ch[3] = '\0';
  t->col = col;
}

inline void drawFullBlock(Vec2i pos, const Color col) {
  drawChar(pos, "█", col);
}

inline void drawTopBlock(Vec2i pos, const Color col) {
  drawChar(pos, "▀", col);
}

inline void drawBottomBlock(Vec2i pos, const Color col) {
  drawChar(pos, "▄", col);
}

inline void drawLeftBlock(Vec2i pos, const Color col) {
  drawChar(pos, "▌", col);
}

inline void drawRightBlock(Vec2i pos, const Color col) {
  drawChar(pos, "▐", col);
}

inline void drawSquare(Vec2i pos, const Color col) {
  drawChar(pos, "■", col);
}

inline void drawCircle(Vec2i pos, const Color col) {
  drawChar(pos, "●", col);
}

inline void drawTriangleUp(Vec2i pos, const Color col) {
  drawChar(pos, "▲", col);
}

inline void drawTriangleDown(Vec2i pos, const Color col) {
  drawChar(pos, "▼", col);
}

inline void drawDiamond(Vec2i pos, const Color col) {
  drawChar(pos, "◆", col);
}

void drawString(Vec2i pos, const char* str, const Color col) {
  int curr_x = pos.x;
  while( *str && curr_x < WIDTH ) {
    char ch_buf[2] = { *str, '\0' };
    drawChar((Vec2i){curr_x, pos.y}, ch_buf, col);
    curr_x++;
    str++;
  }
}

void drawBox(Vec2i opos, Vec2i epos, const Color col) {
  if(opos.x < 0) opos.x = 0;
  if(opos.y < 0) opos.y = 0;
  if(epos.x >= WIDTH) epos.x = WIDTH;
  if(epos.y >= HEIGHT) epos.y = HEIGHT;

  Tile* t;

  t = &buf.back[opos.y * WIDTH + opos.x];
  strcpy(t->ch, "┌");
  t->col = col;

  t = &buf.back[opos.y * WIDTH + epos.x];
  strcpy(t->ch, "┐");
  t->col = col;

  t = &buf.back[epos.y * WIDTH + opos.x];
  strcpy(t->ch, "└");
  t->col = col;

  t = &buf.back[epos.y * WIDTH + epos.x];
  strcpy(t->ch, "┘");
  t->col = col;

  for(int i = opos.x+1; i < epos.x; i++) {
    t = &buf.back[opos.y * WIDTH + i];
    strcpy(t->ch, "─");
    t->col = col;

    t = &buf.back[epos.y * WIDTH + i];
    strcpy(t->ch, "─");
    t->col = col;
  }

  for(int i = opos.y+1; i < epos.y; i++) {
    t = &buf.back[i * WIDTH + opos.x];
    strcpy(t->ch, "│");
    t->col = col;

    t = &buf.back[i * WIDTH + epos.x];
    strcpy(t->ch, "│");
    t->col = col;
  }
}

void drawBoxR(Vec2i opos, Vec2i epos, const Color col) {
  if(opos.x < 0) opos.x = 0;
  if(opos.y < 0) opos.y = 0;
  if(epos.x >= WIDTH) epos.x = WIDTH;
  if(epos.y >= HEIGHT) epos.y = HEIGHT;

  Tile* t;

  t = &buf.back[opos.y * WIDTH + opos.x];
  strcpy(t->ch, "╭");
  t->col = col;

  t = &buf.back[opos.y * WIDTH + epos.x];
  strcpy(t->ch, "╮");
  t->col = col;

  t = &buf.back[epos.y * WIDTH + opos.x];
  strcpy(t->ch, "╰");
  t->col = col;

  t = &buf.back[epos.y * WIDTH + epos.x];
  strcpy(t->ch, "╯");
  t->col = col;

  for(int i = opos.x+1; i < epos.x; i++) {
    t = &buf.back[opos.y * WIDTH + i];
    strcpy(t->ch, "─");
    t->col = col;

    t = &buf.back[epos.y * WIDTH + i];
    strcpy(t->ch, "─");
    t->col = col;
  }

  for(int i = opos.y+1; i < epos.y; i++) {
    t = &buf.back[i * WIDTH + opos.x];
    strcpy(t->ch, "│");
    t->col = col;

    t = &buf.back[i * WIDTH + epos.x];
    strcpy(t->ch, "│");
    t->col = col;
  }
}

void drawBoxD(Vec2i opos, Vec2i epos, const Color col) {
  if(opos.x < 0) opos.x = 0;
  if(opos.y < 0) opos.y = 0;
  if(epos.x >= WIDTH) epos.x = WIDTH;
  if(epos.y >= HEIGHT) epos.y = HEIGHT;

  Tile* t;

  t = &buf.back[opos.y * WIDTH + opos.x];
  strcpy(t->ch, "╔");
  t->col = col;

  t = &buf.back[opos.y * WIDTH + epos.x];
  strcpy(t->ch, "╗");
  t->col = col;

  t = &buf.back[epos.y * WIDTH + opos.x];
  strcpy(t->ch, "╚");
  t->col = col;

  t = &buf.back[epos.y * WIDTH + epos.x];
  strcpy(t->ch, "╝");
  t->col = col;

  for(int i = opos.x+1; i < epos.x; i++) {
    t = &buf.back[opos.y * WIDTH + i];
    strcpy(t->ch, "═");
    t->col = col;

    t = &buf.back[epos.y * WIDTH + i];
    strcpy(t->ch, "═");
    t->col = col;
  }

  for(int i = opos.y+1; i < epos.y; i++) {
    t = &buf.back[i * WIDTH + opos.x];
    strcpy(t->ch, "║");
    t->col = col;

    t = &buf.back[i * WIDTH + epos.x];
    strcpy(t->ch, "║");
    t->col = col;
  }
}

void drawBoxTitle(Vec2i opos, Vec2i epos, const Color col, const char* str, const unsigned char type) {
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
  int mx = (opos.x + epos.x - len + 1) / 2;

  drawString((Vec2i){mx, opos.y}, str, col);
}

void drawBoxText(Vec2i opos, Vec2i epos, const Color col, const char* str, const unsigned char type) {
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
  int mx = (opos.x + epos.x - len + 1) / 2;
  int my = (opos.y + epos.y) / 2;

  drawString((Vec2i){mx, my}, str, col);
}

Matrix* initMatrix(Vec2i size) {
  Matrix* mat = malloc(sizeof(Matrix));
  mat->size = size;

  uint8_t bytes = (size.x / 2) * (size.y / 4);
  mat->data = calloc(bytes, sizeof(uint8_t));

  return mat;
}

void setBit(const Matrix* mat, Vec2i pos, bool val) {
  if(pos.x < 0 || pos.x >= mat->size.x || pos.y < 0 || pos.y >= mat->size.y) return;

  size_t idx = (pos.y / 4) * mat->size.x / 2 + (pos.x / 2);

  if(val) {
    mat->data[idx] |= (1 << ((pos.y % 4) * 2 + (pos.x % 2)));
  } else {
    mat->data[idx] &= ~(1 << ((pos.y % 4) * 2 + (pos.x % 2)));
  }
}

bool getBit(const Matrix* mat, Vec2i pos) {
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

void brailCode(uint8_t code, char out[4]) {
  uint32_t codepoint = 0x2800 + code;
  out[0] = (char)(0xE0 | ((codepoint >> 12) & 0x0F));
  out[1] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
  out[2] = (char)(0x80 | (codepoint & 0x3F));
  out[3] = '\0';
}

void drawBrail(const Matrix* mat, Vec2i pos, const Color col) {
  Vec2i gridPos = {0, 0};
  char ch[4];
  for(size_t i = 0; i < (mat->size.x / 2) * (mat->size.y / 4); i++) { 
    gridPos.x = i % (mat->size.x / 2);
    gridPos.y = i / (mat->size.x / 2);

    brailCode(getCode(mat, i), ch);
    drawChar(vec2i_add(pos, gridPos), ch, col);
  }
}

void createTB(Vec2i pos, Vec2i size, const char* str, const Color col) {
  if(elementBuf.TB_COUNT == 0) {

  }
}

void appendToBuf(HelperBuf* HB, const char* data) {
  size_t cap = sizeof(HB->data) - HB->len;
  size_t len = strlen(data);

  if( len > cap ) len = cap;

  memcpy(HB->data + HB->len, data, len);
  HB->len += len;
}

void gridFlush() {
  HelperBuf HB = { .len = 0 };
  char temp[128];

  for(int j = 0; j < HEIGHT; j++) {
    for(int i = 0; i < WIDTH; i++) {
      Tile* front = &buf.front[j * WIDTH + i];
      Tile* back = &buf.back[j * WIDTH + i];

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

void initLib(void) {
  sa.sa_handler = handle_sigwinch;
  sa.sa_flags = SA_RESTART;

  sigemptyset(&sa.sa_mask);
  sigaction(SIGWINCH, &sa, NULL);

  getTerminalSize();

  buf.front = malloc(sizeof(Tile) * (WIDTH * HEIGHT));
  buf.back = malloc(sizeof(Tile) * (WIDTH * HEIGHT));

  elementBuf.TB_COUNT = 0;

  memset(buf.front, 0, sizeof(Tile) * (WIDTH * HEIGHT));
  gridClear();
}

void closeLib(void) {
  free(buf.front);
  free(buf.back);

  if(elementBuf.TB_COUNT>0) {
    free(elementBuf.TB);
  }
}

void updateFrame(void) {
  if (g_resized) {
    g_resized = 0;

    getTerminalSize();

    buf.front = realloc(buf.front, sizeof(Tile) * (WIDTH * HEIGHT));
    buf.back = realloc(buf.back, sizeof(Tile) * (WIDTH * HEIGHT));

    memset(buf.front, 0, sizeof(Tile) * (WIDTH * HEIGHT));
    gridClear();

    printf("\033[2J\033[H"); 
  }

  gridFlush();

  FRAMECOUNT++;
}
