//
//  MyGeometries.cpp - Fall 2022
//
//   Sets up and renders 
//     - the ground plane, and
//     - the surface of rotation
//   for the Math 155A project #6.
//


// Use the static library (so glew32.dll is not needed):
#define GLEW_STATIC
#include <GL/glew.h> 
#include <GLFW/glfw3.h>

#include "LinearR3.h"		// Adjust path as needed.
#include "LinearR4.h"		// Adjust path as needed.
#include "MathMisc.h"       // Adjust path as needed

#include "MyGeometries.h"
#include "TextureProj.h"
#include "PhongData.h"
#include "RgbImage.h"
#include "GlGeomCylinder.h"
#include "GlGeomSphere.h"
#include "GlGeomTorus.h"

// **********************************
// Material to underlie a texture map.
// YOU MAY DEFINE A SECOND ONE OF THESE IF YOU WISH
// **********************************
phMaterial materialUnderTexture;

// **************************
// Information for loading textures
// **************************
const int NumTextures = 7;
unsigned int TextureNames[NumTextures];     // Texture names generated by OpenGL
const char* TextureFiles[NumTextures] = {
    "darkred.bmp",
    "MARBLES.BMP",
    "stars.bmp",
    "monalisa.bmp",
    "bamboo.bmp",
    "eye.bmp",
    "RoughWood.bmp"
};

// *******************************
// For spheres and a cylinder and a torus (Torus is currently not used.)
// *******************************
GlGeomSphere texSphere(5, 5);
GlGeomCylinder texCylinder(5, 5, 5);
GlGeomTorus texTorus(5, 5, 0.75);   


// ************************
// General data helping with setting up VAO (Vertex Array Objects)
//    and Vertex Buffer Objects.
// ***********************
const int NumObjects = 6;
const int myFloor = 0;
const int myBackWall = 1;
const int myLeftWall = 2;
const int myRightWall = 3;
const int myPaint = 4;
const int myTop = 5;


unsigned int myVBO[NumObjects];  // a Vertex Buffer Object holds an array of data
unsigned int myVAO[NumObjects];  // a Vertex Array Object - holds info about an array of vertex data;
unsigned int myEBO[NumObjects];  // a Element Array Buffer Object - holds an array of elements (vertex indices)

// ********************************************
// This sets up for texture maps. It is called only once
// ********************************************
void SetupForTextures()
{
    // This material goes under the textures.
    // IF YOU WISH, YOU MAY DEFINE MORE THAN ONE OF THESE FOR DIFFERENT GEOMETRIES
    materialUnderTexture.SpecularColor.Set(0.9, 0.9, 0.9);
    materialUnderTexture.AmbientColor.Set(0.3, 0.3, 0.3);
    materialUnderTexture.DiffuseColor.Set(0.7, 0.7, 0.7);       // Increase or decrease to adjust brightness
    materialUnderTexture.SpecularExponent = 40.0;

    // ***********************************************
    // Load texture maps
	// ***********************************************
    RgbImage texMap;

    glUseProgram(shaderProgramBitmap);
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(NumTextures, TextureNames);
    for (int i = 0; i < NumTextures; i++) {
        texMap.LoadBmpFile(TextureFiles[i]);            // Read i-th texture from the i-th file.
        glBindTexture(GL_TEXTURE_2D, TextureNames[i]);  // Bind (select) the i-th OpenGL texture

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Set best quality filtering.   Also see below for disabling mipmaps.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);  // Requires that mipmaps be generated (see below)
        // You may also try GL_LINEAR_MIPMAP_NEAREST -- try looking at the wall from a 30 degree angle, and look for sweeping transitions.

        // Store the texture into the OpenGL texture named TextureNames[i]
        int textureWidth = texMap.GetNumCols();
        int textureHeight = texMap.GetNumRows();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texMap.ImageData());
 #if 1
        // Use mipmaps  (Best!)
        glGenerateMipmap(GL_TEXTURE_2D);
#else
        // Don't use mipmaps.  Try moving away from the brick wall a great distance
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#endif

    }

    // Make sure that the shaderProgramBitmap uses the GL_TEXTURE_0 texture.
    glUseProgram(shaderProgramBitmap);
    glUniform1i(glGetUniformLocation(shaderProgramBitmap, "theTextureMap"), 0);
    glActiveTexture(GL_TEXTURE0);


}

// **********************
// This sets up geometries needed for 
//   (a) the floor (ground plane)
//   (b) the back wall
//   (d) two spheres
//   (e) one cylinder
//  It is called only once.
// **********************
void MySetupSurfaces() {

    texSphere.InitializeAttribLocations(vertPos_loc, vertNormal_loc, vertTexCoords_loc);
    texCylinder.InitializeAttribLocations(vertPos_loc, vertNormal_loc, vertTexCoords_loc);
    texTorus.InitializeAttribLocations(vertPos_loc, vertNormal_loc, vertTexCoords_loc);

    // Initialize the VAO's, VBO's and EBO's for the ground plane, the back wall
    // and the surface of rotation. Gives them the "vertPos" location,
    // and the "vertNormal"  and the "vertTexCoords" locations in the shader program.
    //   routines is called

    glGenVertexArrays(NumObjects, &myVAO[0]);
    glGenBuffers(NumObjects, &myVBO[0]);
    glGenBuffers(NumObjects, &myEBO[0]);

    float floorVerts[] = {
        // Position              // Normal                  // Texture coordinates
        -6.0f, 0.0f, -6.0f,      0.0f, 1.0f, 0.0f,          0.0f, 1.0f,         // Back left
         6.0f, 0.0f, -6.0f,      0.0f, 1.0f, 0.0f,          1.0f, 1.0f,         // Back right
         6.0f, 0.0f,  6.0f,      0.0f, 1.0f, 0.0f,          1.0f, 0.0f,         // Front right
        -6.0f, 0.0f,  6.0f,      0.0f, 1.0f, 0.0f,          0.0f, 0.0f,         // Front left
    };
    unsigned int floorElts[] = { 0, 3, 1, 2 };
    glBindBuffer(GL_ARRAY_BUFFER, myVBO[myFloor]);
    glBindVertexArray(myVAO[myFloor]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVerts), floorVerts, GL_STATIC_DRAW);
    glVertexAttribPointer(vertPos_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);	   // Vertex positions in the VBO
    glEnableVertexAttribArray(vertPos_loc);									// Enable the stored vertices
    glVertexAttribPointer(vertNormal_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));	// Vertex normals in the VBO
    glEnableVertexAttribArray(vertNormal_loc);									// Enable the stored vertices
    glVertexAttribPointer(vertTexCoords_loc, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));	// Vertex texture coordinates in the VBO
    glEnableVertexAttribArray(vertTexCoords_loc);									// Enable the stored vertices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myEBO[myFloor]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floorElts), floorElts, GL_STATIC_DRAW);

    float backWallVerts[] = {
        // Position              // Normal                  // Texture coordinates
        -6.0f, 0.0f, -6.0f,      0.0f, 1.0f, 0.0f,          0.0f, 1.0f,         // A
         6.0f, 0.0f, -6.0f,      0.0f, 1.0f, 0.0f,          1.0f, 1.0f,         // B
         6.0f, 6.0f, -6.0f,      0.0f, 1.0f, 0.0f,          1.0f, 0.0f,         // C
        -6.0f, 6.0f, -6.0f,      0.0f, 1.0f, 0.0f,          0.0f, 0.0f,         // D
    };
    unsigned int backWallElts[] = { 0, 1, 3, 2 };
    glBindBuffer(GL_ARRAY_BUFFER, myVBO[myBackWall]);
    glBindVertexArray(myVAO[myBackWall]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(backWallVerts), backWallVerts, GL_STATIC_DRAW);
    glVertexAttribPointer(vertPos_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);	   // Vertex positions in the VBO
    glEnableVertexAttribArray(vertPos_loc);									// Enable the stored vertices
    glVertexAttribPointer(vertNormal_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));	// Vertex normals in the VBO
    glEnableVertexAttribArray(vertNormal_loc);									// Enable the stored vertices
    glVertexAttribPointer(vertTexCoords_loc, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));	// Vertex texture coordinates in the VBO
    glEnableVertexAttribArray(vertTexCoords_loc);									// Enable the stored vertices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myEBO[myBackWall]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(backWallElts), backWallElts, GL_STATIC_DRAW);


    float leftWallVerts[] = {
        // Position              // Normal                  // Texture coordinates
        -6.0f, 0.0f,  6.0f,      0.0f, 1.0f, 0.0f,          0.0f, 1.0f,         // E
        -6.0f, 0.0f, -6.0f,      0.0f, 1.0f, 0.0f,          1.0f, 1.0f,         // A
        -6.0f, 6.0f, -6.0f,      0.0f, 1.0f, 0.0f,          1.0f, 0.0f,         // D
        -6.0f, 6.0f,  6.0f,      0.0f, 1.0f, 0.0f,          0.0f, 0.0f,         // F
    };
    unsigned int leftWallElts[] = { 0, 1, 3, 2 };
    glBindBuffer(GL_ARRAY_BUFFER, myVBO[myLeftWall]);
    glBindVertexArray(myVAO[myLeftWall]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(leftWallVerts), leftWallVerts, GL_STATIC_DRAW);
    glVertexAttribPointer(vertPos_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);	   // Vertex positions in the VBO
    glEnableVertexAttribArray(vertPos_loc);									// Enable the stored vertices
    glVertexAttribPointer(vertNormal_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));	// Vertex normals in the VBO
    glEnableVertexAttribArray(vertNormal_loc);									// Enable the stored vertices
    glVertexAttribPointer(vertTexCoords_loc, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));	// Vertex texture coordinates in the VBO
    glEnableVertexAttribArray(vertTexCoords_loc);									// Enable the stored vertices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myEBO[myLeftWall]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(leftWallElts), leftWallElts, GL_STATIC_DRAW);


    float rightWallVerts[] = {
        // Position              // Normal                  // Texture coordinates
         6.0f, 0.0f, -6.0f,      0.0f, 1.0f, 0.0f,          1.0f, 1.0f,         // B
         6.0f, 0.0f,  6.0f,      0.0f, 1.0f, 0.0f,          0.0f, 1.0f,         // G
         6.0f, 6.0f,  6.0f,      0.0f, 1.0f, 0.0f,          0.0f, 0.0f,         // H
         6.0f, 6.0f, -6.0f,      0.0f, 1.0f, 0.0f,          1.0f, 0.0f,         // C
    };
    unsigned int rightWallElts[] = { 0, 1, 3, 2 };
    glBindBuffer(GL_ARRAY_BUFFER, myVBO[myRightWall]);
    glBindVertexArray(myVAO[myRightWall]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rightWallVerts), rightWallVerts, GL_STATIC_DRAW);
    glVertexAttribPointer(vertPos_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);	   // Vertex positions in the VBO
    glEnableVertexAttribArray(vertPos_loc);									// Enable the stored vertices
    glVertexAttribPointer(vertNormal_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));	// Vertex normals in the VBO
    glEnableVertexAttribArray(vertNormal_loc);									// Enable the stored vertices
    glVertexAttribPointer(vertTexCoords_loc, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));	// Vertex texture coordinates in the VBO
    glEnableVertexAttribArray(vertTexCoords_loc);									// Enable the stored vertices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myEBO[myRightWall]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rightWallElts), rightWallElts, GL_STATIC_DRAW);


    float paintVerts[] = {
        -1.0f, 1.2f, -5.9f,      0.0f, 1.0f, 0.0f,          0.0f, 1.0f,         // A
         1.0f, 1.2f, -5.9f,      0.0f, 1.0f, 0.0f,          1.0f, 1.0f,         // B
         1.0f, 4.2f, -5.9f,      0.0f, 1.0f, 0.0f,          1.0f, 0.0f,         // C
        -1.0f, 4.2f, -5.9f,      0.0f, 1.0f, 0.0f,          0.0f, 0.0f,         // D
    };
    unsigned int paintElts[] = { 0, 1, 3, 2 };
    glBindBuffer(GL_ARRAY_BUFFER, myVBO[myPaint]);
    glBindVertexArray(myVAO[myPaint]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(paintVerts), paintVerts, GL_STATIC_DRAW);
    glVertexAttribPointer(vertPos_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);	   // Vertex positions in the VBO
    glEnableVertexAttribArray(vertPos_loc);									// Enable the stored vertices
    glVertexAttribPointer(vertNormal_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));	// Vertex normals in the VBO
    glEnableVertexAttribArray(vertNormal_loc);									// Enable the stored vertices
    glVertexAttribPointer(vertTexCoords_loc, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));	// Vertex texture coordinates in the VBO
    glEnableVertexAttribArray(vertTexCoords_loc);									// Enable the stored vertices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myEBO[myPaint]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(paintElts), paintElts, GL_STATIC_DRAW);

    float topVerts[] = {
        // Position              // Normal                  // Texture coordinates
        -6.0f, 6.0f, -6.0f,      0.0f, 1.0f, 0.0f,          0.0f, 1.0f,         // Back left
         6.0f, 6.0f, -6.0f,      0.0f, 1.0f, 0.0f,          1.0f, 1.0f,         // Back right
         6.0f, 6.0f,  6.0f,      0.0f, 1.0f, 0.0f,          1.0f, 0.0f,         // Front right
        -6.0f, 6.0f,  6.0f,      0.0f, 1.0f, 0.0f,          0.0f, 0.0f,         // Front left
    };
    unsigned int topElts[] = { 0, 1, 3, 2 };
    glBindBuffer(GL_ARRAY_BUFFER, myVBO[myTop]);
    glBindVertexArray(myVAO[myTop]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(topVerts), topVerts, GL_STATIC_DRAW);
    glVertexAttribPointer(vertPos_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);	   // Vertex positions in the VBO
    glEnableVertexAttribArray(vertPos_loc);									// Enable the stored vertices
    glVertexAttribPointer(vertNormal_loc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));	// Vertex normals in the VBO
    glEnableVertexAttribArray(vertNormal_loc);									// Enable the stored vertices
    glVertexAttribPointer(vertTexCoords_loc, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));	// Vertex texture coordinates in the VBO
    glEnableVertexAttribArray(vertTexCoords_loc);									// Enable the stored vertices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myEBO[myTop]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(topElts), topElts, GL_STATIC_DRAW);

    check_for_opengl_errors();      // Watch the console window for error messages!
}

void MyRemeshGeometries() 
{

    texSphere.Remesh(meshRes, meshRes);
    texCylinder.Remesh(meshRes, meshRes, meshRes);
    texTorus.Remesh(meshRes, meshRes );

    check_for_opengl_errors();
}

void MyRenderGeometries() {

    float matEntries[16];       // Temporary storage for floats
    // ******
    // Render the Floor - using a procedural texture map
    glBindTexture(GL_TEXTURE_2D, TextureNames[4]);
    selectShaderProgram(shaderProgramBitmap);
    glBindVertexArray(myVAO[myFloor]);                // Select the floor VAO (Vertex Array Object)
    materialUnderTexture.LoadIntoShaders();         // Use the bright underlying color
    viewMatrix.DumpByColumns(matEntries);           // Apply the model view matrix
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    glUniform1i(applyTextureLocation, true);           // Enable applying the texture!
    // Draw the floor as a single triangle strip
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, (void*)0);    
    glUniform1i(applyTextureLocation, false);           // Turn off applying texture!
    check_for_opengl_errors();

    // ******
    // Render the Top - using a procedural texture map
    // ******
    selectShaderProgram(shaderProgramProc);
    glBindVertexArray(myVAO[myTop]);                // Select the floor VAO (Vertex Array Object)
    materialUnderTexture.LoadIntoShaders();         // Use the bright underlying color
    viewMatrix.DumpByColumns(matEntries);           // Apply the model view matrix
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    glUniform1i(applyTextureLocation, true);           // Enable applying the texture!
    // Draw the floor as a single triangle strip
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, (void*)0);
    glUniform1i(applyTextureLocation, false);           // Turn off applying texture!

    if (renderFloorOnly) {
        return;
    }

    // ************ 
    // Render the wall

    glBindTexture(GL_TEXTURE_2D, TextureNames[0]);
    selectShaderProgram(shaderProgramBitmap);
    glBindVertexArray(myVAO[myBackWall]);
    materialUnderTexture.LoadIntoShaders();
    viewMatrix.DumpByColumns(matEntries);
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    glUniform1i(applyTextureLocation, true);
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, (void*)0);
    glUniform1i(applyTextureLocation, false);
    check_for_opengl_errors();

    selectShaderProgram(shaderProgramBitmap);
    glBindVertexArray(myVAO[myLeftWall]);
    materialUnderTexture.LoadIntoShaders();
    viewMatrix.DumpByColumns(matEntries);
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    glUniform1i(applyTextureLocation, true);
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, (void*)0);
    glUniform1i(applyTextureLocation, false);
    check_for_opengl_errors();

    selectShaderProgram(shaderProgramBitmap);
    glBindVertexArray(myVAO[myRightWall]);
    materialUnderTexture.LoadIntoShaders();
    viewMatrix.DumpByColumns(matEntries);
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    glUniform1i(applyTextureLocation, true);
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, (void*)0);
    glUniform1i(applyTextureLocation, false);
    check_for_opengl_errors();

    // Render a paint
    glBindTexture(GL_TEXTURE_2D, TextureNames[3]);
    selectShaderProgram(shaderProgramBitmap);
    glBindVertexArray(myVAO[myPaint]);
    materialUnderTexture.LoadIntoShaders();
    viewMatrix.DumpByColumns(matEntries);
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    glUniform1i(applyTextureLocation, true);
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, (void*)0);
    glUniform1i(applyTextureLocation, false);
    check_for_opengl_errors();

    // *************
    // Render a sphere
    LinearMapR4 matDemo = viewMatrix;
    matDemo.Mult_glTranslate(-5.9, 3, 0.0);
    matDemo.Mult_glScale(0.2, 1.5 + cosf(currentTime * PI2), 1.5 - cosf(currentTime * PI2));
    matDemo.DumpByColumns(matEntries);           // Apply the model view matrix
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    glBindTexture(GL_TEXTURE_2D, TextureNames[2]);     // Choose Earth image texture
    glUniform1i(applyTextureLocation, true);           // Enable applying the texture!
    texSphere.Render();                                 // Render the sphere
    glUniform1i(applyTextureLocation, false);           // Turn off applying texture!

    // Renders a torus
    matDemo = viewMatrix;
    matDemo.Mult_glTranslate(0.0, 0.3, 0.0);
    matDemo.Mult_glScale(0.8, 0.2, 0.8);
    matDemo.Mult_glRotate(currentTime * PI2, 0.0, 1.0, 0.0);
    matDemo.DumpByColumns(matEntries);           // Apply the model view matrix
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    glBindTexture(GL_TEXTURE_2D, TextureNames[1]);     // Choose texture
    glUniform1i(applyTextureLocation, true);            // Enable applying the texture!
    texTorus.Render();                                  // Render the torus
    glUniform1i(applyTextureLocation, false);           // Turn off applying texture!
        


    // Render a paint 
    matDemo = viewMatrix;
    matDemo.Mult_glTranslate(5.8, 3, 0.0);
    matDemo.Mult_glRotate(-currentTime * PI2, 1, 0, 0);
    matDemo.Mult_glScale(0.05, 1.5, 1.5);
    matDemo.Mult_glRotate(PI / 2, 0, 0, 1);
    matDemo.DumpByColumns(matEntries);           // Apply the model view matrix
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    glUniform1i(applyTextureLocation, true);           // Enable applying the texture!
    glBindTexture(GL_TEXTURE_2D, TextureNames[5]);
    texCylinder.RenderTop();                              // RENDER THIS WITH A TEXTURE MAP
    texCylinder.RenderBase();                             // RENDER THIS WITH A TEXTURE MAP
    glUniform1i(applyTextureLocation, false);             // Turn off applying texture!

    // Render chairs
    matDemo = viewMatrix;
    matDemo.Mult_glTranslate(4, 0.1, -4);
    matDemo.Mult_glRotate(PI / 4, 0, 1, 0);
    matDemo.Mult_glScale(0.5, 0.2, 1);
    matDemo.DumpByColumns(matEntries);
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    glUniform1i(applyTextureLocation, true);           // Enable applying the texture!
    glBindTexture(GL_TEXTURE_2D, TextureNames[6]);
    texCylinder.RenderSide();
    texCylinder.RenderTop();                              // RENDER THIS WITH A TEXTURE MAP
    texCylinder.RenderBase();                             // RENDER THIS WITH A TEXTURE MAP
    glUniform1i(applyTextureLocation, false);             // Turn off applying texture!

    matDemo = viewMatrix;
    matDemo.Mult_glTranslate(-4, 0.1, -4);
    matDemo.Mult_glRotate(PI /-4, 0, 1, 0);
    matDemo.Mult_glScale(0.5, 0.2, 1);
    matDemo.DumpByColumns(matEntries);
    glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
    glUniform1i(applyTextureLocation, true);           // Enable applying the texture!
    texCylinder.RenderSide();
    texCylinder.RenderTop();                              // RENDER THIS WITH A TEXTURE MAP
    texCylinder.RenderBase();                             // RENDER THIS WITH A TEXTURE MAP
    glUniform1i(applyTextureLocation, false);             // Turn off applying texture!




    check_for_opengl_errors();      // Watch the console window for error messages!
}