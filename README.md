# Сplus
Compiler for a toy imperative language

## Getting Started
Prerequisites:

* CMake (3.0+)
* Flex and Bison tools
* C++ compiler

### Quick Start: Windows Platform

1. Install [CMake](https://cmake.org/)
    * You can check installation by typing `cmake --version` in terminal.
2. Install any compatible C++ compiler suite (like [Visual Studio](https://visualstudio.microsoft.com/) or [MinGW](https://www.mingw-w64.org/) or [Clang](https://clang.llvm.org/))
    * By default, CMake will discover your available compiler tools. To check available generators type `cmake --help` and look for last section about Generators.
    * The default generator will be highlighted and used by CMake. If you want to change it add `-G <generator_name>` flag on configuration/generation stage.
3. Install [Flex and Bison](https://github.com/lexxmark/winflexbison/releases) for Windows
    * Extract the contents of the archive like during the normal installation (e. g. into `C:\Program Files\Flex Bison\`)
    * Add the folder where you put your Flex/Bison to the path ([guide](https://stackoverflow.com/questions/44272416/how-to-add-a-folder-to-path-environment-variable-in-windows-10-with-screensho))
    * Start/Restart your terminal anywhere and type `win_bison.exe`. If Bison tool starts and asks for operands you did everything right. CMake will automatically discover Flex/Bison.

4. Now you can proceed to the [configuration stage](#config-and-building)

---
### Quick Start: Unix Platform

1. Install CMake using package manager (e. g. `sudo apt-get -y install cmake`)
2. Install C++ compiler (GCC or Clang) using package manager
3. The same way install Flex and Bison
4. Check your installation: `cmake`, `g++`, `flex` and `bison` tools must be discoverable in your shell.
5. Now you can proceed to the [configuration stage](#config-and-building)

---
### Config and Building

1. Open your terminal and navigate to repository
2. Create build folder (e. g. `mkdir <build_folder>`)
3. Run CMake Configuration with `cmake -S . -B <build_folder>`
4. Project files are generated into build folder. To build the project:
    * You open them if this a IDE project file or buld them manually if this is a makefile.
    * You run command `cmake --build <build_folder>`

Example for Windows:
```ps
cd "C:\Cplus"
mkdir Build
cmake -S . -B Build
cmake --build Build
```
Example for Unix:
```bash
cd cplus
mkdir build
cmake -S . -B build
cmake --build build
```

Additional flags you can specify on configuration/generation stage:
* `-A x64` to specify "x64" architecture
* `-G "Visual Studio 17 2022 Win64"` to choose VS22 generator

Additional flags for build stage:
* `--config Release` to set "Release" configuration. Configuration must one of {Debug|Release|RelWithDebInfo,MinSizeRel}.