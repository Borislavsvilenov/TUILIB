#ifndef TUILIB_H
#define TUILIB_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <signal.h>
#include <termios.h>

#include "Vec2.h"

#ifdef __cplusplus
extern "C" {
#endif

  // ============================================================================
  // Key Codes (Returned by readKey)
  // ============================================================================
#define KEY_NONE        0
#define KEY_ESC        27
#define KEY_ARROW_UP   1001
#define KEY_ARROW_DOWN 1002
#define KEY_ARROW_RIGHT 1003
#define KEY_ARROW_LEFT 1004
#define KEY_HOME       1005
#define KEY_END        1006
#define KEY_DELETE     1007

  // ============================================================================
  // Data Structures
  // ============================================================================
  typedef struct {
    uint8_t fg_r, fg_g, fg_b;
    uint8_t bg_r, bg_g, bg_b;
  } Color;

  typedef struct {
    char ch[4];
    Color col;
  } Tile;

  typedef struct {
    Tile* front;
    Tile* back;
  } DoubleBuffer;

  typedef struct {
    Vec2i size;
    uint8_t* data;
  } Matrix;

  typedef struct {
    Vec2i pos;
    Vec2i size;
    char* text;
    Color col;
  } TextBox;

  typedef struct {
    TextBox* TB;
    size_t TB_COUNT;
  } ElementBuffer;

  typedef struct {
    char data[128 * 4096];
    size_t len;
  } HelperBuf;

  // ============================================================================
  // External Global Variables (Defined in TUILIB.c)
  // ============================================================================
  extern int WIDTH;
  extern int HEIGHT;
  extern size_t FRAMECOUNT;
  extern volatile sig_atomic_t g_resized;
  extern struct termios orig_termios;
  extern struct sigaction sa;
  extern DoubleBuffer buf;
  extern ElementBuffer elementBuf;

  // ============================================================================
  // System & Initialization Functions
  // ============================================================================
  void handle_sigwinch(int sig);
  void getTerminalSize(void);
  void disableRawMode(void);
  void enableRawMode(void);
  int readKey(void);
  void enableMouse(void);
  void disableMouse(void);
  void initLib(void);
  void closeLib(void);
  void updateFrame(void);

  // ============================================================================
  // Buffer & Rendering Operations
  // ============================================================================
  void gridClear(void);
  void appendToBuf(HelperBuf* HB, const char* data);
  void gridFlush(void);

  // ============================================================================
  // Primitive Drawing Functions
  // ============================================================================
  void drawChar(Vec2i pos, const char* ch, const Color col);
  void drawString(Vec2i pos, const char* str, const Color col);

  // Block/Shape Drawing
  void drawFullBlock(Vec2i pos, const Color col);
  void drawTopBlock(Vec2i pos, const Color col);
  void drawBottomBlock(Vec2i pos, const Color col);
  void drawLeftBlock(Vec2i pos, const Color col);
  void drawRightBlock(Vec2i pos, const Color col);
  void drawSquare(Vec2i pos, const Color col);
  void drawCircle(Vec2i pos, const Color col);
  void drawTriangleUp(Vec2i pos, const Color col);
  void drawTriangleDown(Vec2i pos, const Color col);
  void drawDiamond(Vec2i pos, const Color col);

  // Box Drawing
  void drawBox(Vec2i opos, Vec2i epos, const Color col);
  void drawBoxR(Vec2i opos, Vec2i epos, const Color col);
  void drawBoxD(Vec2i opos, Vec2i epos, const Color col);
  void drawBoxTitle(Vec2i opos, Vec2i epos, const Color col, const char* str, const unsigned char type);
  void drawBoxText(Vec2i opos, Vec2i epos, const Color col, const char* str, const unsigned char type);

  // ============================================================================
  // Braille & Matrix Functions
  // ============================================================================
  Matrix* initMatrix(Vec2i size);
  void setBit(const Matrix* mat, Vec2i pos, bool val);
  bool getBit(const Matrix* mat, Vec2i pos);
  uint8_t getCode(const Matrix* mat, size_t idx);
  void brailCode(uint8_t code, char out[4]);
  void drawBrail(const Matrix* mat, Vec2i pos, const Color col);

  // ============================================================================
  // UI Component Creation
  // ============================================================================
  void createTB(Vec2i pos, Vec2i size, const char* str, const Color col);

#ifdef __cplusplus
}
#endif

#endif // TUILIB_H
