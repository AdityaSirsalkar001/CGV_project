#pragma once

#include "Canvas.h"

// Math and drawing algorithms
class Algorithms {
public:
    // Bresenham's Line Drawing Algorithm (handles all 8 octants)
    static void DrawLineBresenham(Canvas& canvas, int x0, int y0, int x1, int y1, const Color& color);

    // Xiaolin Wu's anti-aliased line algorithm
    static void DrawLineWu(Canvas& canvas, int x0, int y0, int x1, int y1, const Color& color);

    // Midpoint Circle Algorithm
    static void DrawCircleMidpoint(Canvas& canvas, int xc, int yc, int radius, const Color& color);

    // Basic Flood Fill Algorithm
    static void FloodFill(Canvas& canvas, int x, int y, const Color& newColor);
};
