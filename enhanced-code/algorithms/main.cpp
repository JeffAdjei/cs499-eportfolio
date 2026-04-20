#include <iostream>
#include <cstdlib>

#include <GL/glew.h>
#include "GLFW/glfw3.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SceneManager.h"
#include "ViewManager.h"
#include "ShapeMeshes.h"
#include "ShaderManager.h"

namespace
{
    const char* const WINDOW_TITLE = "7-1 FinalProject and Milestones";
    GLFWwindow* g_Window = nullptr;

    SceneManager* g_SceneManager = nullptr;
    ShaderManager* g_ShaderManager = nullptr;
    ViewManager* g_ViewManager = nullptr;
}

// Function declarations
bool InitializeGLFW();
bool InitializeGLEW();
bool InitializeApplication();
void LoadSceneResources();
void RunRenderLoop();
void CleanupApplication();

/*
 * Main entry point for the graphics application.
 *
 * CS 499 Milestone Three enhancements:
 * 1. Refactored startup, render loop, and cleanup into dedicated functions
 * 2. Reduced repeated logic inside the frame loop
 * 3. Improved maintainability and scalability of render flow
 * 4. Structured program flow to better support algorithmic enhancements
 */
int main(int argc, char* argv[])
{
    if (!InitializeApplication())
    {
        CleanupApplication();
        return EXIT_FAILURE;
    }

    RunRenderLoop();
    CleanupApplication();

    return EXIT_SUCCESS;
}

/*
 * Initializes GLFW, shaders, window, view manager, scene manager,
 * and all required graphics resources.
 */
bool InitializeApplication()
{
    if (!InitializeGLFW())
    {
        std::cerr << "ERROR: Failed to initialize GLFW." << std::endl;
        return false;
    }

    g_ShaderManager = new ShaderManager();
    g_ViewManager = new ViewManager(g_ShaderManager);
    g_Window = g_ViewManager->CreateDisplayWindow(WINDOW_TITLE);

    if (g_Window == nullptr)
    {
        std::cerr << "ERROR: Failed to create display window." << std::endl;
        return false;
    }

    if (!InitializeGLEW())
    {
        std::cerr << "ERROR: Failed to initialize GLEW." << std::endl;
        return false;
    }

    g_ShaderManager->LoadShaders(
        "../../Utilities/shaders/vertexShader.glsl",
        "../../Utilities/shaders/fragmentShader.glsl");
    g_ShaderManager->use();

    g_SceneManager = new SceneManager(g_ShaderManager);

    LoadSceneResources();

    // Enable depth test once during initialization instead of every frame
    glEnable(GL_DEPTH_TEST);

    return true;
}

/*
 * Loads all scene textures and prepares the scene.
 * Grouping this logic improves readability and supports future scaling.
 */
void LoadSceneResources()
{
    g_SceneManager->CreateGLTexture("../../Utilities/textures/brick_texture.jpeg", "brick");
    g_SceneManager->CreateGLTexture("../../Utilities/textures/wood_texture.jpg", "wood");

    g_SceneManager->PrepareScene();
}

/*
 * Runs the main render loop.
 *
 * Algorithmic improvement:
 * Static OpenGL state setup is moved outside the loop where possible,
 * leaving only frame-dependent work inside the loop.
 */
void RunRenderLoop()
{
    while (!glfwWindowShouldClose(g_Window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        g_ViewManager->PrepareSceneView();
        g_SceneManager->RenderScene();

        glfwSwapBuffers(g_Window);
        glfwPollEvents();
    }
}

/*
 * Safely releases allocated resources.
 */
void CleanupApplication()
{
    if (g_SceneManager != nullptr)
    {
        delete g_SceneManager;
        g_SceneManager = nullptr;
    }

    if (g_ViewManager != nullptr)
    {
        delete g_ViewManager;
        g_ViewManager = nullptr;
    }

    if (g_ShaderManager != nullptr)
    {
        delete g_ShaderManager;
        g_ShaderManager = nullptr;
    }

    if (g_Window != nullptr)
    {
        glfwDestroyWindow(g_Window);
        g_Window = nullptr;
    }

    glfwTerminate();
}

/*
 * Initializes GLFW and sets platform-specific OpenGL hints.
 */
bool InitializeGLFW()
{
    if (!glfwInit())
    {
        return false;
    }

#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    return true;
}

/*
 * Initializes GLEW and prints OpenGL information.
 */
bool InitializeGLEW()
{
    GLenum glewInitResult = glewInit();
    if (GLEW_OK != glewInitResult)
    {
        std::cerr << glewGetErrorString(glewInitResult) << std::endl;
        return false;
    }

    std::cout << "INFO: OpenGL Successfully Initialized\n";
    std::cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << "\n" << std::endl;

    return true;
}