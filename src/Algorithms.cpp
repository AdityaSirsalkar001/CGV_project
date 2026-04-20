#include "Algorithms.h"
#include <cmath>
#include <queue>
#include <algorithm>
#include <utility>

// ---------------------------------------------------------
// Helper for Wu's Line Algorithm
// ---------------------------------------------------------
inline float fractionalPart(float x) { return x - std::floor(x); }
inline float reverseFractionalPart(float x) { return 1.0f - fractionalPart(x); }

static void DrawPixelAntiAliased(Canvas& canvas, int x, int y, const Color& color, float intensity) {
    if (x < 0 || x >= canvas.GetWidth() || y < 0 || y >= canvas.GetHeight()) return;

    Color bg = canvas.GetPixel(x, y);

    // Alpha blending
    uint8_t r = static_cast<uint8_t>(color.r * intensity + bg.r * (1.0f - intensity));
    uint8_t g = static_cast<uint8_t>(color.g * intensity + bg.g * (1.0f - intensity));
    uint8_t b = static_cast<uint8_t>(color.b * intensity + bg.b * (1.0f - intensity));

    canvas.SetPixel(x, y, Color(r, g, b, 255));
}

// ---------------------------------------------------------
// Bresenham's Line Algorithm
// ---------------------------------------------------------
void Algorithms::DrawLineBresenham(Canvas& canvas, int x0, int y0, int x1, int y1, const Color& color) {
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    
    // Determine the direction of step
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    
    // Initial error term
    int err = dx - dy;

    while (true) {
        canvas.SetPixel(x0, y0, color);
        
        // Check if we have reached the end point
        if (x0 == x1 && y0 == y1) break;
        
        // Calculate error for the next pixel
        int e2 = 2 * err;
        
        // Adjust error and x-coordinate if we step horizontally
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        
        // Adjust error and y-coordinate if we step vertically
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

// ---------------------------------------------------------
// Xiaolin Wu's algorithm for Anti-aliased line drawing
// ---------------------------------------------------------
void Algorithms::DrawLineWu(Canvas& canvas, int x0, int y0, int x1, int y1, const Color& color) {
    bool steep = std::abs(y1 - y0) > std::abs(x1 - x0);

    // Swap coordinates if line is steep to simplify calculation
    if (steep) {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }
    // Always draw from lower x to higher x
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = y1 - y0;
    float gradient = (dx == 0) ? 1.0f : static_cast<float>(dy) / dx;

    // Handle the first endpoint
    int xend = x0;
    int yend = y0 + gradient * (xend - x0);
    float xgap = reverseFractionalPart(x0 + 0.5f);
    int xpxl1 = xend; // will be used in the main loop
    int ypxl1 = yend;

    if (steep) {
        DrawPixelAntiAliased(canvas, ypxl1, xpxl1, color, reverseFractionalPart(yend) * xgap);
        DrawPixelAntiAliased(canvas, ypxl1 + 1, xpxl1, color, fractionalPart(yend) * xgap);
    } else {
        DrawPixelAntiAliased(canvas, xpxl1, ypxl1, color, reverseFractionalPart(yend) * xgap);
        DrawPixelAntiAliased(canvas, xpxl1, ypxl1 + 1, color, fractionalPart(yend) * xgap);
    }
    float intery = yend + gradient; // first y-intersection for the main loop

    // Handle the second endpoint
    xend = x1;
    yend = y1 + gradient * (xend - x1);
    xgap = fractionalPart(x1 + 0.5f);
    int xpxl2 = xend;
    int ypxl2 = yend;

    if (steep) {
        DrawPixelAntiAliased(canvas, ypxl2, xpxl2, color, reverseFractionalPart(yend) * xgap);
        DrawPixelAntiAliased(canvas, ypxl2 + 1, xpxl2, color, fractionalPart(yend) * xgap);
    } else {
        DrawPixelAntiAliased(canvas, xpxl2, ypxl2, color, reverseFractionalPart(yend) * xgap);
        DrawPixelAntiAliased(canvas, xpxl2, ypxl2 + 1, color, fractionalPart(yend) * xgap);
    }

    // Main loop
    if (steep) {
        for (int x = xpxl1 + 1; x < xpxl2; ++x) {
            DrawPixelAntiAliased(canvas, static_cast<int>(intery), x, color, reverseFractionalPart(intery));
            DrawPixelAntiAliased(canvas, static_cast<int>(intery) + 1, x, color, fractionalPart(intery));
            intery += gradient;
        }
    } else {
        for (int x = xpxl1 + 1; x < xpxl2; ++x) {
            DrawPixelAntiAliased(canvas, x, static_cast<int>(intery), color, reverseFractionalPart(intery));
            DrawPixelAntiAliased(canvas, x, static_cast<int>(intery) + 1, color, fractionalPart(intery));
            intery += gradient;
        }
    }
}

// ---------------------------------------------------------
// Midpoint Circle Algorithm
// ---------------------------------------------------------
void Algorithms::DrawCircleMidpoint(Canvas& canvas, int xc, int yc, int radius, const Color& color) {
    if (radius <= 0) return;
    
    int x = 0;
    int y = radius;
    // Decision parameter to decide which pixel is closer to the true circle
    int d = 1 - radius; // Equivalent to 5/4 - r for integer math

    // Inner lambda to plot 8 symmetrical points
    auto plot8 = [&](int cx, int cy, int px, int py) {
        canvas.SetPixel(cx + px, cy + py, color);
        canvas.SetPixel(cx - px, cy + py, color);
        canvas.SetPixel(cx + px, cy - py, color);
        canvas.SetPixel(cx - px, cy - py, color);
        canvas.SetPixel(cx + py, cy + px, color);
        canvas.SetPixel(cx - py, cy + px, color);
        canvas.SetPixel(cx + py, cy - px, color);
        canvas.SetPixel(cx - py, cy - px, color);
    };

    plot8(xc, yc, x, y);

    while (x < y) {
        if (d < 0) {
            // Select E (East) pixel
            d += 2 * x + 3;
        } else {
            // Select SE (South-East) pixel
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
        plot8(xc, yc, x, y);
    }
}

// ---------------------------------------------------------
// Basic Flood Fill Algorithm
// ---------------------------------------------------------
void Algorithms::FloodFill(Canvas& canvas, int startX, int startY, const Color& newColor) {
    // Check out of bounds
    if (startX < 0 || startX >= canvas.GetWidth() || startY < 0 || startY >= canvas.GetHeight()) return;

    Color targetColor = canvas.GetPixel(startX, startY);

    // If target is the same as new color, do nothing
    if (targetColor.Pack() == newColor.Pack()) return;

    // Use a queue for iterative BFS flood fill (to avoid stack overflow recursion)
    std::queue<std::pair<int, int>> q;
    q.push({startX, startY});

    while (!q.empty()) {
        auto [x, y] = q.front();
        q.pop();

        if (x < 0 || x >= canvas.GetWidth() || y < 0 || y >= canvas.GetHeight()) continue;

        if (canvas.GetPixel(x, y).Pack() == targetColor.Pack()) {
            canvas.SetPixel(x, y, newColor);
            
            // Add neighboring pixels
            q.push({x + 1, y});
            q.push({x - 1, y});
            q.push({x, y + 1});
            q.push({x, y - 1});
        }
    }
}
