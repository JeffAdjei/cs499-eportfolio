///////////////////////////////////////////////////////////////////////////////
// viewmanager.cpp
// ============
// Manage the viewing of 3D objects within the viewport
//
// AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
// Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
//
// Enhanced for CS 499 Milestone Three by Jeffrey Adjei:
// - Improved frame update organization
// - Reduced repeated keyboard-processing code
// - Refactored projection selection logic
// - Improved maintainability and input-processing flow
///////////////////////////////////////////////////////////////////////////////

#include "ViewManager.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

namespace
{
    const int WINDOW_WIDTH = 1000;
    const int WINDOW_HEIGHT = 800;
    const char* g_ViewName = "view";
    const char* g_ProjectionName = "projection";

    Camera* g_pCamera = nullptr;

    float gLastX = WINDOW_WIDTH / 2.0f;
    float gLastY = WINDOW_HEIGHT / 2.0f;
    bool gFirstMouse = true;

    float gDeltaTime = 0.0f;
    float gLastFrame = 0.0f;

    /*
     * Key binding structure used to reduce repeated keyboard checks
     * and provide a more scalable input-processing strategy.
     */
    struct KeyBinding
    {
        int key;
        Camera_Movement direction;
    };
}

ViewManager::ViewManager(ShaderManager* pShaderManager)
    : m_pShaderManager(pShaderManager), m_pWindow(nullptr), m_bOrthographicProjection(false)
{
    g_pCamera = new Camera();
    g_pCamera->Position = glm::vec3(0.5f, 5.5f, 10.0f);
    g_pCamera->Front = glm::vec3(0.0f, -0.5f, -2.0f);
    g_pCamera->Up = glm::vec3(0.0f, 1.0f, 0.0f);
    g_pCamera->Zoom = 80.0f;
}

ViewManager::~ViewManager()
{
    if (g_pCamera)
    {
        delete g_pCamera;
        g_pCamera = nullptr;
    }

    m_pShaderManager = nullptr;
    m_pWindow = nullptr;
}

GLFWwindow* ViewManager::CreateDisplayWindow(const char* windowTitle)
{
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, windowTitle, nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);

    glfwSetCursorPosCallback(window, &ViewManager::Mouse_Position_Callback);
    glfwSetScrollCallback(window, &ViewManager::Scroll_Callback);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_pWindow = window;
    return window;
}

void ViewManager::Mouse_Position_Callback(GLFWwindow* window, double xMousePos, double yMousePos)
{
    if (gFirstMouse)
    {
        gLastX = static_cast<float>(xMousePos);
        gLastY = static_cast<float>(yMousePos);
        gFirstMouse = false;
    }

    float xOffset = static_cast<float>(xMousePos) - gLastX;
    float yOffset = gLastY - static_cast<float>(yMousePos);

    gLastX = static_cast<float>(xMousePos);
    gLastY = static_cast<float>(yMousePos);

    if (g_pCamera)
    {
        g_pCamera->ProcessMouseMovement(xOffset, yOffset);
    }
}

void ViewManager::Scroll_Callback(GLFWwindow* window, double xOffset, double yOffset)
{
    if (g_pCamera)
    {
        g_pCamera->ProcessMouseScroll(static_cast<float>(yOffset));
    }
}

/*
 * Updates frame timing values once per frame.
 */
void UpdateFrameTiming()
{
    float currentFrame = static_cast<float>(glfwGetTime());
    gDeltaTime = currentFrame - gLastFrame;
    gLastFrame = currentFrame;
}

/*
 * Returns the appropriate projection matrix based on the active mode.
 */
glm::mat4 BuildProjectionMatrix(bool orthographicProjectionEnabled, Camera* camera)
{
    if (orthographicProjectionEnabled)
    {
        float orthoSize = 10.0f;
        return glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, 0.1f, 100.0f);
    }

    return glm::perspective(
        glm::radians(camera->Zoom),
        static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT),
        0.1f,
        100.0f
    );
}

void ViewManager::ProcessKeyboardEvents()
{
    if (m_pWindow == nullptr)
    {
        return;
    }

    if (glfwGetKey(m_pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(m_pWindow, true);
    }

    if (g_pCamera == nullptr)
    {
        return;
    }

    /*
     * Data-structure enhancement:
     * Use a vector of key bindings instead of repeating nearly identical
     * conditionals for each movement direction.
     */
    std::vector<KeyBinding> movementKeys =
    {
        { GLFW_KEY_W, FORWARD },
        { GLFW_KEY_S, BACKWARD },
        { GLFW_KEY_A, LEFT },
        { GLFW_KEY_D, RIGHT },
        { GLFW_KEY_Q, UP },
        { GLFW_KEY_E, DOWN }
    };

    for (const KeyBinding& binding : movementKeys)
    {
        if (glfwGetKey(m_pWindow, binding.key) == GLFW_PRESS)
        {
            g_pCamera->ProcessKeyboard(binding.direction, gDeltaTime);
        }
    }

    if (glfwGetKey(m_pWindow, GLFW_KEY_O) == GLFW_PRESS)
    {
        m_bOrthographicProjection = true;
    }

    if (glfwGetKey(m_pWindow, GLFW_KEY_P) == GLFW_PRESS)
    {
        m_bOrthographicProjection = false;
    }
}

void ViewManager::PrepareSceneView()
{
    if (g_pCamera == nullptr || m_pShaderManager == nullptr)
    {
        return;
    }

    UpdateFrameTiming();
    ProcessKeyboardEvents();

    glm::mat4 view = g_pCamera->GetViewMatrix();
    glm::mat4 projection = BuildProjectionMatrix(m_bOrthographicProjection, g_pCamera);

    m_pShaderManager->setMat4Value(g_ViewName, view);
    m_pShaderManager->setMat4Value(g_ProjectionName, projection);
    m_pShaderManager->setVec3Value("viewPosition", g_pCamera->Position);
}