/**
 *  @filename   :   epdpaint.h
 *  @brief      :   Header file for epdpaint.cpp
 *  @author     :   Yehui from Waveshare
 *  
 *  Copyright (C) Waveshare     July 28 2017
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef EPDPAINT_H
#define EPDPAINT_H

// Display orientation
#define ROTATE_0            0
#define ROTATE_90           1
#define ROTATE_180          2
#define ROTATE_270          3

// Color inverse. 1 or 0 = set or reset a bit if set a colored pixel
#define IF_INVERT_COLOR     0

#include <stdlib.h>
#include "Fonts/fonts.h"

typedef struct paint_t {
    uint8_t* frame_buffer;
    int width;
    int height;
    int rotate;
} paint_t;

paint_t* paint_new(int width, int height);
void paint_init(paint_t* paint, int width, int height);
void Paint_Clear(paint_t* paint, int colored);
int  Paint_GetWidth(paint_t* paint);
void Paint_SetWidth(paint_t* paint, int width);
int  Paint_GetHeight(paint_t* paint);
void Paint_SetHeight(paint_t* paint, int height);
int  Paint_GetRotate(paint_t* paint);
void Paint_SetRotate(paint_t* paint, int rotate);
unsigned char* Paint_GetImage(paint_t* paint);
void Paint_DrawAbsolutePixel(paint_t* paint, int x, int y, int colored);
void Paint_DrawPixel(paint_t* paint, int x, int y, int colored);
void Paint_DrawCharAt(paint_t* paint, int x, int y, char ascii_char, sFONT* font, int colored);
void Paint_DrawStringAt(paint_t* paint, int x, int y, const char* text, sFONT* font, int colored);
void Paint_DrawLine(paint_t* paint, int x0, int y0, int x1, int y1, int colored);
void Paint_DrawHorizontalLine(paint_t* paint, int x, int y, int width, int colored);
void Paint_DrawVerticalLine(paint_t* paint, int x, int y, int height, int colored);
void Paint_DrawRectangle(paint_t* paint, int x0, int y0, int x1, int y1, int colored);
void Paint_DrawFilledRectangle(paint_t* paint, int x0, int y0, int x1, int y1, int colored);
void Paint_DrawCircle(paint_t* paint, int x, int y, int radius, int colored);
void Paint_DrawFilledCircle(paint_t* paint, int x, int y, int radius, int colored);

#endif

/* END OF FILE */

