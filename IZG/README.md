# Custom GPU

## May 2024

### Usage details

Files `gpu.cpp` and `prepareModel.cpp` are the only files I modified. Whole project was wrapped in a template made by Ph.D Tomáš Milet and it is too big for this repository, so the project you see here cannot be compiled :)

### Results

15.8 / 20 points

### Description

Your task is to implement a simple graphics card (GPU) and a function to render models. In the file `gpu.cpp` implement the function `izg_enqueue` - the functionality of the graphics card you have implemented. In the `prepareModel.cpp` file, implement the `prepareModel`, `drawModel_vertexShader`, and `drawModel_fragmentShader` functions. These functions are used to process the loaded model file into the graphics card memory and command buffer. In addition, there is a `fwd.hpp` file - it contains declarations of structures and constants.
