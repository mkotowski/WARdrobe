# WARdrobe
WARdrobe: Self-Assembly Required. ZTGK 2020

> We throw in extra parts just to mess with you. — IKEA, probably

## Project Structure

```
.
├───.git/                    -- git folder
├───assets/                  -- files used during game's runtime
│   ├───audio/               -- audio files used within the game
│   ├───models/              -- 3d models used within the game
│   ├───images/              -- raster graphics used within the game
│   ├───scripts/             -- Lua scripts used by the game's logic
│   └───textures/            -- audio files used within the game
├───build/                   -- default out-of-source build directory
│   ├───config/              -- output dir for configured headers
│   ├───documentation/       -- default Doxygen output directory
│   ├───installs/            -- default INSTALL target output
│   └───packages/            -- default PACKAGE target output
├───cmake/                   -- cmake-specific scripts
├───config/                  -- CMake C headers, filled by CMake
├───game/                    -- project of the game's executable
│   ├───include/             -- C++ headers
│   ├───src/                 -- C++ source files
│   └───CMakeLists.txt       -- subproject specific CMakeLists.txt
├───libs/                    -- internal code, separated for clarity
│   ├───audio/               -- audio manager library
│   │   ├───include/
│   │   ├───src/
│   │   └───CMakeLists.txt
│   ├───assetmanager/        -- asset manager library
│   │   ├───include/
│   │   ├───src/
│   │   └───CMakeLists.txt
│   ├───utils/               -- comman useful functions and structs
│   │   ├───include/
│   │   ├───src/
│   │   └───CMakeLists.txt
│   ├───ecs/                 -- entity component system implementation
│   │   ├───include/
│   │   ├───src/
│   │   └───CMakeLists.txt
│   ├───renderer/            -- entity component system implementation
│   │   ├───include/
│   │   ├───src/
│   │   └───CMakeLists.txt
│   └───CMakeLists.txt       -- include the internal libraries
├───resources/               -- unprocessed assets, and compilation-only
│   ├───fonts/               -- font files
│   ├───audio/               -- unprocessed audio files
│   ├───images/              -- unprocessed images
│   │   └───icons/           -- icons for the installers and main exe
│   └───shaders/             -- shaders used in the game
├───settings/                -- game settings accessible to the player
│   └───DefaultSettings.toml
├───tests/                   -- 
├───thirdparty/              -- third-party submodules and libraries
│   ├───glad/                -- generated <glad> code
│   └───CMakeLists.txt       -- handling of the external libraries
├───utilities/               -- scripts for managing the repository
├───.clang-format            -- code formatter's settings
├───.gitignore               -- files and folders ignored by git
├───.gitmodules              -- list of submodules to initialize
├───CMakeLists.txt           -- CMakeLists to build the project
├───LICENSE.txt              -- license information
└───README.md                -- about the project
```

## Preparation of the Workspace

The lowest OpenGL version supported by the game's source code is the **OpenGL 3.3**. Anything lower than that is too different to by supported without an additional effort.

Additionally if any change to the minimum required OpenGL version is to be required, a different version of `glad` library may be needed. In this case use [this generator][gladgen] and replace the files in the `thirdparty/glad` directory.

### Necessary Packages

- `git` and `git lfs`
- [`cmake`][cmakedownload] (minimum version required: **3.15.5**, tested with *3.17.0*)
- compiler with support for C++17

CMake's versions before 3.15.5 do not include all necessary `.dll` files when installing binaries compiled with *Microsoft Visual Studio 2019*.

### Optional Packages

#### Code Style

- `clang-format` — code formatter, bundled with [LLVM installer][llvmdownload] (version 10.0.0 tested)

#### Documentation Generation

- [Doxygen][doxygendownload] — documentation generation, version 1.8.17 used
- `dot` from [Graphviz][dotdownload] — generation of documentation's diagrams

#### Installer Building on Windows

- [WiX Toolset build tools][wixdownload] *V3.11.2*
- [NSIS (nullsoft scriptable install system)][nsisdownload] *3.05*

## Git Submodules' Initialization

The main `CMakeLists.txt` of the project checks whether `git` is installed. If so, it attempts to initialize or update recursively all submodules.

In the case this does not work use the following command:

```bash
git submodule update --init --recursive
```

## Building with CMake

The `CMakeLists.txt` used in this project are tested with an assumption that building is done in the folder `build` in the project's source code root directory, but building should still work if a different location is chosen.

The most basic command to build the project correctly for MSVC2019 is:

```bash
cmake <CMakeLists.txt location> -G "<generator>" -Wno-dev
```

`-Wno-dev` is added to suppress warnings from OpenAL-soft third-party library. In the next revision of the CMakeLists, it will be automatically added.

To build the project from within `build` directory using the generator for MSVC2019, use:

```bash
cmake .. -G "Visual Studio 16 2019" -Wno-dev
```

### CMake Options

Options to set, when building the project with CMake. Here shown as a command line arguments with their default values. They will also be shown while using the CMake GUI.

- `-DINCLUDE_DEBUG_UI=ON` — include ImGUI debug interface
- `-DHIDE_CRT_WIN32_MSVC=OFF` — when building with Visual Studio, toggle visibility of the game's command line
- `-DGIT_SUBMODULE=ON` — check if submodules are initialized when building the project
- `-DBUILD_WARDROBE_DOC=OFF` — generate the documentation
- `-DUSE_WIX_TOOLSET=ON` — use the WiX toolset for MSI Windows Installer, `OFF` means setting the default generator to NSIS EXE installer

Options of the third-party libraries are hidden and set to desired values by default.

### Configurations

Only Debug and Release are taken under consideration. The build should detect if the generator used is multi-config (as for example Microsoft Visual Studio) or not, and behave accordingly. In case of a single-config generator, to create build of different configuration, the rebuild of the whole project is necessary.

Release configuration by default do not include ImGUI and other debug-specific elements.

## Generating Installers

To generate an installer for the game, compile the automatically generated target `PACKAGE`. Generated file(s) should be then found inside `packages` folder in the build directory.

On Windows possible installers to generate are:

- ZIP archive
- MSI installer generated with WiX
- EXE installer generated with NSIS

By default, compiling the `PACKAGE` target will result in generation of a MSI installer with the installed WiX toolset. To generate using NSIS instead, toggle `USE_WIX_TOOLSET` to `OFF`.

## Used Third-party Libraries

### OpenGL

- glad — OpenGL loader
- GLFW — cross-platform OpenGL context and window handling
- Dear ImGUI — included only with *Debug* releases
- freetype

### Audio

- OpenAL-soft — software implementation of the OpenAL standard

**REMEMBER:** Due to both the licensing and the way the library was designed, `openal-soft` must be included as a shared library.

### Utilities

- glm — OpenGL Mathematics

### Parsers and Interpreters

- cpptoml
- json
- lua

### Asset Loaders

- Assimp

### Other

- [ltjax/steam_controller][steam_controller] — a library for accessing the Steam Controller without the Steam SDK or Steam.

## Code Conventions

### C++

#### Version

The project is assumed to use **C++17** standard.

#### Code Style

The required code style is included within the file `.clang-format` in the root directory of the project. When using Microsoft Visual Studio 2019, the IDE should detect the file on its own. Then to apply style to the code itself you need to use the following keyboard shortcut: `ctrl+k ctrl+d`.

Code whenever possible should be maximally 80 characters wide.

#### Modern Features

The only features used should be supported by all three of `GCC`, `Clang`, and `MSVC`. For precise information about support look check [C++ compiler support][compilersupport] on *cppreference.com*.

#### File extensions

- `*.hpp` — C++ header
- `*.cpp` — C++ source files
- `*.h.in` — CMake C headers, configured by CMake and copied to build directory
- `*.toml` — settings files (an extension of INI format)
- `*.json` — data files
- `*.md` — documentation files
- `*.lua` — Lua scripts

### Git

#### Large binary files

For binary files the `git-lfs` is used. Check `.gitattributes` for file formats managed this way.

#### Commits

1. Commit related changes.
2. Commit often.
3. **Don't commit half-done work.**
4. Test your code before you commit.

#### Formatting Rules for Commit Messages

1. Capitalized summary (**max. 50 chars**).
2. More detailed explanatory text, if necessary (wrap it to 72 characters).
3. Write commit messages in the imperative mode: "Fix bug."

#### Branching Workflow and Naming Conventions

1. All new code should be developed in its own branch if it takes more than one commit.
2. If the branch is more than 2 commits behind the `master`, the master should be merged into the feature branch to ensure compatibility with the `master`'s new codebase.

- `master` — this branch contains all finished features
  + releases are marked with `git tag`
  + ideally no unfinished and non-working code allowed
  + merging only with pull requests and review from at least 1 other team member
- `feature/*` — new functionalities
- `fix/*` — fixing bugs and vulnerabilities
- `refactor/*` — improvements to the existing code/features
- `style/*` — changes to code formatting or names only
- `doc/*` — changes to the inline documentation only
- `xxx/*` — very experimental features; testing very different functionalities

With the slash included, graphical git clients (such as SourceTree and GitKraken) can automatically place branches into folders.

Additionally:

1. Avoid non ASCII characters.
2. Use `-` as a replacement for spaces.

### Inline Documentation

The documentation is generated with Doxygen. Form more detailed explanation check the official Doxygen documentation.

#### Comment Blocks

To mark a comment block use the Javadoc style, which consist of a C-style comment block starting with two \*'s, like this:

```cpp
/**
 * ... text ...
 */
```
## Entity Component System

The game employs the Entity Component System design pattern. It means that instead of single `GameObject`, the existence of an object, it's properties (components) and the logic of working on that data (systems) are separated.

> TODO: describe the current ECS implementation

## Asset Pipeline

> TODO: automatic asset processing

Assets used during the runtime and without the need of further processing, are placed in the `assets` directory in the root of the project. During the building/installation/packaging the directory is copied to the necessary location. Assets in original formats and those used only during the building or compilation of the project should be located in `resources` directory.

[gladgen]: https://glad.dav1d.de/
[compilersupport]: https://en.cppreference.com/w/cpp/compiler_support
[cmakedownload]: https://cmake.org/download/
[wixdownload]: https://wixtoolset.org/releases/
[nsisdownload]: https://nsis.sourceforge.io/Download
[dotdownload]: https://www.graphviz.org/download/
[llvmdownload]: https://releases.llvm.org/download.html
[doxygendownload]: http://www.doxygen.nl/download.html#srcbin
[steam_controller]: https://github.com/ltjax/steam_controller