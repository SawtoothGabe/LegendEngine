[![Build (Windows)](https://github.com/SawtoothGabe/LegendEngine/actions/workflows/windows.yml/badge.svg)](https://github.com/SawtoothGabe/LegendEngine/actions/workflows/windows.yml)
[![Build and test (Linux)](https://github.com/SawtoothGabe/LegendEngine/actions/workflows/tests.yml/badge.svg)](https://github.com/SawtoothGabe/LegendEngine/actions/workflows/tests.yml)
[![CodeQL Advanced](https://github.com/SawtoothGabe/LegendEngine/actions/workflows/codeql.yml/badge.svg)](https://github.com/SawtoothGabe/LegendEngine/actions/workflows/codeql.yml)

# Legend Engine

<img alt="l.png" src="Assets/l.png" width="200"/>

***

Legend Engine is a WIP game engine made using C++. <br>
*whose current logo currently looks a bit like an insult you'd hear in Call Of Duty*

The goal is for it to be a fast, versatile, and feature-rich game 
engine that runs on Windows and Linux.
An important goal for the engine is to be usable either through its GUI frontend
called Legendary, as well as its API in code!

All window management and input is done from scratch and handled by a library called [Tether](https://github.com/SawtoothGabe/Tether).

Here's a simple scene:
![demo.gif](Assets/demo.gif)

## How to build

Dependencies:
* **CMake**
* Some toolchain compatible with CMake like **Ninja**, **Makefile**, **Visual Studio**, etc.
* **Vulkan SDK** *(if building with `LE_VULKAN_API`, enabled by default)*

As well as:
* **googletest** *(if building tests)*
* **Tether**
* **Vulkan Memory Allocator (VMA)**, *if `LE_VULKAN_API` is enabled*

which the above should be fetched automatically by CMake.

After you have the necessary dependencies, simply clone the project and run `cmake .` in the root directory.
You may need to install the [Vulkan SDK](https://vulkan.lunarg.com/), or run this command if you're on a debian-based linux:
```
sudo apt install libvulkan-dev vulkan-validationlayers vulkan-tools glslc
```

## Technologies used

* Modern C++26 and design patterns
* Doxygen for generating a documentation page
* GitHub Actions for building and testing the project automatically
* Kanban-style planners for managing development internally.
* CodeQL static analysis
* Unit testing using googletest
* Flexible Archetype ECS framework

...and more as the project continues to grow!