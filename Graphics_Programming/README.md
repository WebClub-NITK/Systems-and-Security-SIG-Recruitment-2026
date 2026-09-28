# Graphics Programming

## Introduction

Graphics programming is the art and science of creating and manipulating visual content using a computer. It forms the foundation of video games, scientific simulations, film special effects, user interfaces, and data visualization. At its core, it involves communicating with the Graphics Processing Unit (GPU), a specialized piece of hardware designed for parallel processing, to render complex scenes efficiently. This is typically done through graphics APIs (Application Programming Interfaces) like OpenGL or Vulkan, which provide a standard way for software to access the GPU's capabilities. This task will guide you through the fundamental steps of building a real-time 3D rendering application from the ground up.

## Problem Statement(s):
This project is designed to build your foundational skills in real-time 3D graphics. You will create a simple rendering engine capable of displaying 3D models and allowing for interactive navigation. Using a language like C++ and a graphics API such as OpenGL, you will implement the core components of a modern graphics application.  

Your program must be able to perform the following tasks:
- **Window & 2D Polygon Rendering:** Create a stable application window and an active graphics context (e.g., using a library like GLFW or SDL). Within this window, render a static, filled, 2D polygon with more than three sides (e.g., a hexagon or an octagon).
- **3D Model Rendering:** Load vertex data from a simple 3D model file (e.g., .obj format) and render it in a 3D perspective scene. You will need to manage vertex buffers and implement vertex and fragment shaders to process and display the model. You may find any .obj files online.
- **Interactive 3D Camera:** Implement a first-person camera system to navigate the scene. The camera should allow the user to:
    - Move forward, backward, left, and right using the W, S, A, and D keys.
    - Move up and down using the Space and Left Shift keys.
    - Look around the scene by moving the mouse.
    - Frame Capture (Screenshot): Implement a feature that allows the user to press a key (e.g., F12) to capture the current contents of the render window and save it as an image file (e.g., .png or .bmp) on the disk.
- **[Bonus] Dynamic Model Loading:** Extend your program to allow a user to load a 3D model file at runtime (e.g., through a file dialog or by specifying a path in the console) without needing to recompile the code.
- **[Bonus] Texture Mapping:** Implement texture loading and mapping. This involves reading an image file, creating a texture object, and modifying your fragment shader to apply the texture to the surface of your 3D model, including handling texture coordinates (UVs).

## Resources:
- Learn OpenGL: Learn OpenGL (An essential, comprehensive resource for modern OpenGL from basics to advanced topics.)
- The Cherno's YouTube Channel: The Cherno - YouTube (Contains an excellent series on OpenGL and game engine development.)
- C++ OpenGL Project Template: https://github.com/NishantAS/webclub-demo (A starter project template using CMake, pre-configured with essential dependencies like GLFW, GLAD, and GLM to get you started quickly.)
- OpenGL Tutorial: OpenGL tutorial (Provides another set of great tutorials for learning modern OpenGL.)
- Official OpenGL Documentation: OpenGL - The Industry's Foundation for High Performance Graphics (The official homepage for the OpenGL standard, with links to specifications and documentation.)
- Assimp (Open Asset Import Library): Assimp (A popular library for loading dozens of different 3D model formats into a common data structure.)
- stb_image.h: https://github.com/nothings/stb/blob/master/stb_image.h (A simple, single-header C library for loading common image formats, perfect for textures.)

## Submission:
- Create a private GitHub repository and add mentors as collaborators.
- Attach a README file explaining each of the steps taken to implement the solutions, along with screenshots and shell logs wherever necessary.
- Include 2 .obj files that you have used to verify your implementation of task 2.
- Add a screen recording of all the features implemented.
## Mentor name and contact details:
Nishant A S (+91 6360219728, github: @NishantAS)  
Tanneru Ranjit (+91 8123999357, github: @AmissDrake)

