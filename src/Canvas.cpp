#include "Canvas.h"

Canvas::Canvas(int width, int height) : width(width), height(height) {
    buffer.resize(width * height, Color(0, 0, 0, 255).Pack()); // Initialize with black
}

void Canvas::SetPixel(int x, int y, const Color& color) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        buffer[y * width + x] = color.Pack();
    }
}

Color Canvas::GetPixel(int x, int y) const {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        uint32_t c = buffer[y * width + x];
        uint8_t r = c & 0xFF;
        uint8_t g = (c >> 8) & 0xFF;
        uint8_t b = (c >> 16) & 0xFF;
        uint8_t a = (c >> 24) & 0xFF;
        return Color(r, g, b, a);
    }
    return Color(0, 0, 0, 0); // Out of bounds
}

void Canvas::Clear(const Color& color) {
    uint32_t packedColor = color.Pack();
    std::fill(buffer.begin(), buffer.end(), packedColor);
}
