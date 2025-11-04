# Low Level OpenGL Practice (C++/GLFW/GLEW/GLM)

A small Windows project that explores modern OpenGL 3.3 core profile from the ground up. It uses raw VAOs, VBOs, EBOs, shader compilation and linking, uniforms, textures, orthographic projection, clipping, blending and basic input. The goal is hands-on learning with minimal abstractions.

## What it shows

- Window creation with GLFW and OpenGL 3.3 core profile
- Manual buffer management: VAO, VBO, EBO and indexed drawing
- Shader pipeline: GLSL vertex and fragment shaders, uniforms and program linking
- 2D orthographic rendering in pixel coordinates with GLM
- Textures via stb_image and texture sampling in shaders
- A pulsating circular UI button rendered as a triangle fan
- Multiple small scenes you can switch between
- Bitmap font text rendering (.fnt + .tga) and a scrolling ticker inside a clipped rectangle
- Simple input handling with GLFW and a fixed FPS main loop

## Scenes and controls

General
- ESC quit the app
- Click the circular button to toggle content on or off
- Press 1, 2, 3 or 4 to switch scenes

Scene 4 (default)
- Two characters with textures
- Controls: A and D move the left character. Left Arrow and Right Arrow move the right character
- Characters are constrained to their halves of the play area

Scene 1
- An analog clock rendered from primitives
- Clock color is configurable from the Settings scene

Scene 2
- A reporter sprite and a scrolling news ticker rendered with the bitmap font

Scene 3 — Settings
- Use Up and Down to select an option
- Press Enter to apply
- Options: Volume Up, Volume Down, Brightness Up, Brightness Down, Change Clock Color
- Brightness is implemented as a translucent overlay. Change Clock Color cycles predefined colors

On the top bar next to the button you will also see a sample text label rendered through the font system.

## Project structure

- `main.cpp` application entry point, setup, render loop and scene switching
- `ShaderUtils.*` helper for compiling and linking shaders from files
- `CircleUtils.*` circle vertex generation and mouse click detection
- `TextRenderer.*` bitmap font loading and batched text rendering, plus scrolling ticker
- `CartoonCharacter.*` textured quad with movement constraints
- `Clock.*` clock face and animated hand using simple transforms
- Shaders in the project root
  - `basic.vert`, `basic.frag` solid color pipeline for rectangles
  - `basic2.vert` variant with model matrix used by the clock
  - `pulsating.frag` time based pulsating effect for the button
  - `character.vert`, `character.frag` textured sprite pipeline
  - `text.vert`, `text.frag` bitmap font rendering with alpha
  - `overlay.vert`, `overlay.frag` fullscreen translucent overlay for brightness

## Dependencies

Managed by NuGet through `packages.config`
- GLFW 3.3.8 window and input
- GLEW 2.2.0 OpenGL function loader
- GLM 0.9.9.8 math library
- stb_image single header image loader bundled as `stb_image.h`

## Build and run (Windows, Visual Studio)

- Open `Sablon.sln` in Visual Studio 2019 or 2022
- Ensure configuration is x64 Debug or Release
- Visual Studio will restore NuGet packages on first build
- Press Build then Start Without Debugging

Runtime assets
- Place the following files next to the executable or run from the project folder so relative paths resolve
  - `myFont.fnt` and `myFont_0.tga`
  - Character textures for the scenes such as `fica.png`, `fedja.png`, `reporter.png`

If an image or font is missing you will see a message in the console and the app will continue running.

## Learning notes

- Uses OpenGL 3.3 core only and avoids deprecated fixed function calls
- Demonstrates scissor test for rectangular clipping and alpha blending for text
- Shows uniform updates for projection, model transforms and simple time based animation
- The code favors explicit state changes and minimal abstraction to expose how the pipeline works


