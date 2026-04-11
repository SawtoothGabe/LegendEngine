# LegendShaders
LegendShaders is the shader compiler for LegendEngine.
It takes slang shaders and compiles them into various formats like DXIL, SPIRV, GLSL, and WGSL.

LegendShaders is meant to make compiling and embedding shaders into a LegendEngine
project as easy as possible. The output file (specified by the `-o` option)
is a C file that projects include with the executable to include shaders.

## Usage
Here's a simple example:
```
lesh input.slang -o output.c
```
### Options
* -o *output*
    
    Specifies the name of the output `.c` file containing shader code and information.
* --dxil

    Enables DXIL output. Used for DX11 and DX12.

    *Note: DXIL compilation will only work if using MSVC on Windows*
* --spirv

    Enables SPIR-V output. Used for Vulkan
* --glsl

    Enables GLSL output. Used for OpenGL.
* --wgsl

    Enables WGSL output. Used for WebGPU.
* -I *directory*

    Specifies an include directory for the input shaders