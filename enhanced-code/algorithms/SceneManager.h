///////////////////////////////////////////////////////////////////////////////
// SceneManager.h
// ==============
// Manages the preparation and rendering of 3D scenes.
//
// AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
// Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
//
// Enhanced for CS 499 Milestone Three by Jeffrey Adjei:
// - Improved organization and documentation
// - Better alignment with vector-based scene processing
// - Clearer structure for scalable rendering workflows
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ShaderManager.h"
#include "ShapeMeshes.h"

#include <string>
#include <vector>

/***********************************************************
 * SceneManager
 *
 * This class manages the scene lifecycle, including:
 * - loading textures
 * - preparing mesh resources
 * - configuring shader values
 * - rendering scene objects
 *
 * In the CS 499 enhancement, this class supports a more
 * scalable rendering strategy by working with structured
 * scene data and reducing duplicated render logic.
 ***********************************************************/
class SceneManager
{
public:
    // Constructor
    SceneManager(ShaderManager* pShaderManager);

    // Destructor
    ~SceneManager();

    /***********************************************************
     * TEXTURE_INFO
     *
     * Stores a texture tag and its associated OpenGL ID.
     ***********************************************************/
    struct TEXTURE_INFO
    {
        std::string tag;
        uint32_t ID;
    };

    /***********************************************************
     * OBJECT_MATERIAL
     *
     * Stores material properties used by shaders.
     ***********************************************************/
    struct OBJECT_MATERIAL
    {
        float ambientStrength;
        glm::vec3 ambientColor;
        glm::vec3 diffuseColor;
        glm::vec3 specularColor;
        float shininess;
        std::string tag;
    };

private:
    // Pointer to shader manager object
    ShaderManager* m_pShaderManager;

    // Pointer to basic mesh collection
    ShapeMeshes* m_basicMeshes;

    // Total number of loaded textures
    int m_loadedTextures;

    // Fixed-size texture storage
    TEXTURE_INFO m_textureIDs[16];

    // Collection of defined object materials
    std::vector<OBJECT_MATERIAL> m_objectMaterials;

    // Loads texture images and converts them to OpenGL texture data
    bool CreateGLTexture(const char* filename, std::string tag);

    // Binds loaded OpenGL textures to texture slots
    void BindGLTextures();

    // Frees loaded OpenGL textures
    void DestroyGLTextures();

    // Finds a loaded texture by tag and returns its OpenGL ID
    int FindTextureID(std::string tag);

    // Finds a loaded texture by tag and returns its slot index
    int FindTextureSlot(std::string tag);

    // Finds a material by tag
    bool FindMaterial(std::string tag, OBJECT_MATERIAL& material);

    // Sets transformation values in the shader
    void SetTransformations(
        glm::vec3 scaleXYZ,
        float XrotationDegrees,
        float YrotationDegrees,
        float ZrotationDegrees,
        glm::vec3 positionXYZ
    );

    // Sets solid color values in the shader
    void SetShaderColor(
        float redColorValue,
        float greenColorValue,
        float blueColorValue,
        float alphaValue
    );

    // Sets texture data in the shader
    void SetShaderTexture(std::string textureTag);

    // Sets UV scaling values for texture mapping
    void SetTextureUVScale(float u, float v);

    // Sets material values in the shader
    void SetShaderMaterial(std::string materialTag);

public:
    // Prepares mesh resources and scene data
    void PrepareScene();

    // Renders the complete 3D scene
    void RenderScene();
};