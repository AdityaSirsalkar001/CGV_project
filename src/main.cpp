#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <iostream>
#include <vector>
#include <cmath>
#include <memory>

#include "Canvas.h"
#include "Algorithms.h"
#include "Renderer.h"

// Window dimensions
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// Application state
enum class Tool {
    PENCIL,
    BRUSH, // Anti-aliased line
    CIRCLE,
    FILL_BUCKET
};

Tool currentTool = Tool::PENCIL;
float currentColor[4] = {1.0f, 1.0f, 1.0f, 1.0f}; // RGBA Draw color
float backgroundColor[4] = {0.0f, 0.0f, 0.0f, 1.0f}; // RGBA Back color
bool isDrawing = false;
int lastMouseX = -1;
int lastMouseY = -1;
int startMouseX = -1;
int startMouseY = -1;

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    // ------------------------------------------------------------------
    // GLFW Init
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "2D Paint Application - Software Rasterizer", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // ------------------------------------------------------------------
    // GLEW Init
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // ------------------------------------------------------------------
    // ImGui Init
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // ------------------------------------------------------------------
    // App Components Init
    Canvas canvas(SCR_WIDTH, SCR_HEIGHT);
    canvas.Clear(Color(0, 0, 0, 255)); // Start with black background
    
    Renderer renderer;
    renderer.Initialize();

    std::unique_ptr<Canvas> backupCanvas = nullptr;

    // ------------------------------------------------------------------
    // Main Render Loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        int windowW, windowH;
        glfwGetWindowSize(window, &windowW, &windowH);

        // Process Mouse Input for Canvas
        if (!io.WantCaptureMouse) { // Don't draw if mouse is over ImGui windows
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                double mx, my;
                glfwGetCursorPos(window, &mx, &my);
                
                // Map screen coordinates to internal canvas constraints, accounting for arbitrary window sizing natively
                int mouseX = static_cast<int>((mx / windowW) * SCR_WIDTH);
                int mouseY = static_cast<int>((my / windowH) * SCR_HEIGHT);
                
                Color color(static_cast<uint8_t>(currentColor[0] * 255.0f),
                            static_cast<uint8_t>(currentColor[1] * 255.0f),
                            static_cast<uint8_t>(currentColor[2] * 255.0f),
                            static_cast<uint8_t>(currentColor[3] * 255.0f));

                if (!isDrawing) {
                    isDrawing = true;
                    lastMouseX = mouseX;
                    lastMouseY = mouseY;
                    startMouseX = mouseX;
                    startMouseY = mouseY;
                    
                    // Take snapshot for preview-based drawing (Circle Tool)
                    backupCanvas = std::make_unique<Canvas>(canvas);
                    
                    if (currentTool == Tool::FILL_BUCKET) {
                        Algorithms::FloodFill(canvas, mouseX, mouseY, color);
                    } else if (currentTool == Tool::PENCIL) {
                        Algorithms::DrawLineBresenham(canvas, mouseX, mouseY, mouseX, mouseY, color);
                    } else if (currentTool == Tool::BRUSH) {
                        Algorithms::DrawLineWu(canvas, mouseX, mouseY, mouseX, mouseY, color);
                    }
                } else {
                    // Dragging
                    if (currentTool == Tool::CIRCLE) {
                        // Restore from backup to prevent stamping duplicate preview circles over each other instantly
                        canvas = *backupCanvas; 
                        int radius = static_cast<int>(std::sqrt(std::pow(mouseX - startMouseX, 2) + std::pow(mouseY - startMouseY, 2)));
                        Algorithms::DrawCircleMidpoint(canvas, startMouseX, startMouseY, radius, color);
                    } else if (currentTool == Tool::PENCIL) {
                        Algorithms::DrawLineBresenham(canvas, lastMouseX, lastMouseY, mouseX, mouseY, color);
                    } else if (currentTool == Tool::BRUSH) {
                        Algorithms::DrawLineWu(canvas, lastMouseX, lastMouseY, mouseX, mouseY, color);
                    }
                    lastMouseX = mouseX;
                    lastMouseY = mouseY;
                }
            } else {
                if (isDrawing) {
                    isDrawing = false;
                    backupCanvas.reset(); // Free backup once finalized click
                }
            }
        } else {
            // Also cancel drawing if they dragged onto the toolbar
            isDrawing = false;
        }

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ----------------------------------------------------------
        // MS-Paint Style Floating Toolbar natively docked to Top
        // ----------------------------------------------------------
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(windowW, 50)); 
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;
        ImGui::Begin("Tools", nullptr, flags);
        
        Color bgColor(static_cast<uint8_t>(backgroundColor[0] * 255.0f),
                      static_cast<uint8_t>(backgroundColor[1] * 255.0f),
                      static_cast<uint8_t>(backgroundColor[2] * 255.0f),
                      255);

        if (ImGui::Button("Clear")) {
            canvas.Clear(bgColor);
        }
        
        ImGui::SameLine(0, 20);
        ImGui::Text("Tools:");
        ImGui::SameLine();
        if (ImGui::RadioButton("Pencil", currentTool == Tool::PENCIL)) currentTool = Tool::PENCIL;
        ImGui::SameLine();
        if (ImGui::RadioButton("Brush", currentTool == Tool::BRUSH)) currentTool = Tool::BRUSH;
        ImGui::SameLine();
        if (ImGui::RadioButton("Circle", currentTool == Tool::CIRCLE)) currentTool = Tool::CIRCLE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Fill Bucket", currentTool == Tool::FILL_BUCKET)) currentTool = Tool::FILL_BUCKET;
        
        ImGui::SameLine(0, 20);
        
        // Color Pickers
        ImGuiColorEditFlags colorFlags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoOptions;
        
        ImGui::Text("Draw Color");
        ImGui::SameLine();
        ImGui::ColorEdit4("##Draw", currentColor, colorFlags);
        
        ImGui::SameLine();
        ImGui::Text("BG Color");
        ImGui::SameLine();
        ImGui::ColorEdit4("##BG", backgroundColor, colorFlags);

        ImGui::End();

        // Render OpenGL Frame
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Upload our CPU canvas to texture and draw the quad
        renderer.Render(canvas);

        // Render ImGui over the canvas
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}
