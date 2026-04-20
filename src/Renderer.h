#pragma once

#include "Canvas.h"

class Renderer {
public:
    Renderer();
    ~Renderer();

    // Initializes OpenGL buffers and shaders
    void Initialize();

    // Uploads canvas data to GPU texture and triggers draw call
    void Render(const Canvas& canvas);

private:
    unsigned int shaderProgram;
    unsigned int VBO, VAO, EBO;
    unsigned int texture;

    void compileShaders();
    void setupBuffers();
    void setupTexture();
};
