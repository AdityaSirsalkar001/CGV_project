#pragma once

#include <vector>
#include <cstdint>

struct Color {
    uint8_t r, g, b, a;
    
    // Default constructor (black, fully opaque)
    Color() : r(0), g(0), b(0), a(255) {}
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : r(r), g(g), b(b), a(a) {}
    
    // Pack into a single 32-bit unsigned int (ABGR order for typical little-endian OpenGL RGBA)
    // Adjust if needed depending on GL_RGBA format.
    uint32_t Pack() const {
        return (uint32_t(a) << 24) | (uint32_t(b) << 16) | (uint32_t(g) << 8) | uint32_t(r);
    }
};

class Canvas {
public:
    Canvas(int width, int height);
    ~Canvas() = default;

    void SetPixel(int x, int y, const Color& color);
    Color GetPixel(int x, int y) const;
    void Clear(const Color& color);

    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    const std::vector<uint32_t>& GetBuffer() const { return buffer; }

private:
    int width;
    int height;
    std::vector<uint32_t> buffer;
};
