# Final Project: Finite Element Analysis (FEA) Program

## Project Overview

For the final project, I will use OpenGL and C++ to simulate a Finite Element Analysis (FEA) program. The program will visualize a simple 3D model of a structure, such as a beam, under different loads. The structure will be represented in 3D space, and the FEA results (e.g., deformations, stress distributions) will be shown through colors. 

The project will incorporate the following elements:
- **Geometric Objects:** Three 3D geometric objects representing the structure and supports.
- **Model File:** A simple model (e.g., a beam) will be loaded and rendered.
- **Animations:** The model will undergo transformations using the mouse to rotate and move the object
- **Lighting and Texture Mapping:** I will use lighting to illuminate the scene, and if possible, apply textures to the model to enhance realism.

## Project Schedule
- **Week 1** - Set up the basic OpenGL framework and display a simple 3D model.  
- **Week 2 and 3** - Implement the Finite Element Analysis calculations and apply them to the structure.  
- **Week 4** - Polish the application and create project presentation.

## Project Technical Requirements

- **Programming Language:** C++
- **Graphics API:** OpenGL
- **Libraries:** GLM, GLEW, and GLFW for OpenGL in Visual Studio, stb for importing images, assimp for importing 3D models and gathering necessary data for calculations
- **3D Model Format:** .blend for the structure model

The application will be written entirely in C++ using OpenGL to handle the rendering and visualization aspects. The 3D model will be loaded into the application using libraries such as Assimp to handle .blend file formats. The program will calculate and visualize the deformation of the structure based on simulated loads and stresses.