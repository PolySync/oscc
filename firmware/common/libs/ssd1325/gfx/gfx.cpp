/*
This is the core graphics library for all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).  It needs to be
paired with a hardware-specific library for each display device we carry
(to handle the lower-level functions).

Adafruit invests time and resources providing this open source code, please
support Adafruit & open-source hardware by purchasing products from Adafruit!

Copyright (c) 2013 Adafruit Industries.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
 */

#include "gfx.h"
#include "glcdfont.c"


#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif


GFX::GFX(int16_t w, int16_t h):
WIDTH(w), HEIGHT(h)
{
    _width    = WIDTH;
    _height   = HEIGHT;
    rotation  = 0;
    cursor_y  = cursor_x    = 0;
    textsize  = 1;
    textcolor = textbgcolor = 0xFFFF;
    wrap      = false;
}


void GFX::startWrite(){
    // Overwrite in subclasses if desired!
}

void GFX::writePixel(int16_t x, int16_t y, uint16_t color){
    // Overwrite in subclasses if startWrite is defined!
    drawPixel(x, y, color);
}

void GFX::endWrite(){
    // Overwrite in subclasses if startWrite is defined!
}

// TEXT- AND CHARACTER-HANDLING FUNCTIONS ----------------------------------

// Draw a character
void GFX::drawChar(int16_t x, int16_t y, unsigned char c,
  uint16_t color, uint16_t bg, uint8_t size) {
    if((x >= _width)            || // Clip right
        (y >= _height)           || // Clip bottom
        ((x + 6 * size - 1) < 0) || // Clip left
        ((y + 8 * size - 1) < 0))   // Clip top
        return;

    startWrite();
    for(int8_t i=0; i<5; i++ ) { // Char bitmap = 5 columns
        uint8_t line = pgm_read_byte(&font[c * 5 + i]);
        for(int8_t j=0; j<8; j++, line >>= 1) {
            if(line & 1) {
                if(size == 1)
                    writePixel(x+i, y+j, color);
            } else if(bg != color) {
                if(size == 1)
                    writePixel(x+i, y+j, bg);
            }
        }
    }
    // If opaque, draw vertical line for last column
    if(bg != color) {
        writeLine(x+5, y, x+5, y+7, bg);
    }
    endWrite();
}

#if ARDUINO >= 100
size_t GFX::write(uint8_t c) {
#else
void GFX::write(uint8_t c) {
#endif
    if(c == '\n') {                        // Newline?
        cursor_x  = 0;                     // Reset x to zero,
        cursor_y += textsize * 8;          // advance y one line
    } else if(c != '\r') {                 // Ignore carriage returns
        if(wrap && ((cursor_x + textsize * 6) > _width)) { // Off right?
            cursor_x  = 0;                 // Reset x to zero,
            cursor_y += textsize * 8;      // advance y one line
        }
        drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
        cursor_x += textsize * 6;          // Advance x one char
    }
#if ARDUINO >= 100
    return 1;
#endif
}

void GFX::setCursor(int16_t x, int16_t y) {
    cursor_x = x;
    cursor_y = y;
}

int16_t GFX::getCursorX(void) const {
    return cursor_x;
}

int16_t GFX::getCursorY(void) const {
    return cursor_y;
}

void GFX::setTextColor(uint16_t c) {
    // For 'transparent' background, we'll set the bg
    // to the same as fg instead of using a flag
    textcolor = textbgcolor = c;
}

void GFX::setTextColor(uint16_t c, uint16_t b) {
    textcolor   = c;
    textbgcolor = b;
}

uint8_t GFX::getRotation(void) const {
    return rotation;
}

void GFX::setRotation(uint8_t x) {
    rotation = (x & 3);
    switch(rotation) {
        case 0:
        case 2:
            _width  = WIDTH;
            _height = HEIGHT;
            break;
        case 1:
        case 3:
            _width  = HEIGHT;
            _height = WIDTH;
            break;
    }
}

// Return the size of the display (per current rotation)
int16_t GFX::width(void) const {
    return _width;
}

int16_t GFX::height(void) const {
    return _height;
}

// Bresenham's algorithm
void GFX::writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
        uint16_t color) {
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        _swap_int16_t(x0, y0);
        _swap_int16_t(x1, y1);
    }

    if (x0 > x1) {
        _swap_int16_t(x0, x1);
        _swap_int16_t(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0<=x1; x0++) {
        if (steep) {
            writePixel(y0, x0, color);
        } else {
            writePixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}
