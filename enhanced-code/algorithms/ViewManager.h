///////////////////////////////////////////////////////////////////////////////
// ViewManager.h
// =============
// Manages the viewing and interaction of 3D objects within the viewport.
//
// AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
// Created for CS-330-Computational Graphics and Visualization
//
// Enhanced for CS 499 Milestone Three by Jeffrey Adjei:
// - Improved documentation and structure
// - Aligned with refactored input and projection logic
// - Supports scalable input handling and rendering workflow
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ShaderManager.h"
#include "camera.h"
#include "GLFW/glfw3.h"

/***********************************************************
 * ViewManager
 *
 * This class is responsible for:
 * - creating the display window
 * - handling camera movement and user input
 * - preparing view and projection matrices
 *
 * Enhancements support:
 * - cleaner frame update flow
 * - structured input processing
 * - improved maintainability
 ***********************************************************/
class ViewManager
{
public:
    // Constructor
    ViewManager(ShaderManager* pShaderManager);

    // Destructor
    ~ViewManager();

    // Creates the OpenGL display window
    GLFWwindow* CreateDisplayWindow(const char* windowTitle);

    // Prepares view and projection matrices for rendering
    void PrepareSceneView();

    // Mouse movement callback
    static void Mouse_Position_Callback(GLFWwindow* window, double xMousePos, double yMousePos);

    // Mouse scroll callback (zoom control)
    static void Scroll_Callback(GLFWwindow* window, double xOffset, double yOffset);

private:
    // Pointer to shader manager
    ShaderManager* m_pShaderManager;

    // Active window
    GLFWwindow* m_pWindow;

    // Handles keyboard input
    void ProcessKeyboardEvents();

    // Projection mode toggle
    bool m_bOrthographicProjection;
};