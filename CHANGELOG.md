# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [Unreleased][Unreleased]
### Added
- Handling of fullscreen window
- Loading models with Assimp
- Loading default settings from a `.toml` file
- Ability to toggle resizeability and borderless state of the GLFW window
- Basic ECS implementation
- Rudimentary input handling
- Gamepad `connect` and `disconnect` events
- Test of loding custom gamepad mappings supported by GLFW library
- Basic camera handling
- Rudimentary movement of objects
- Basic rendering system based on the ECS design pattern
- Wireframe render option in the debug interface
- Test of file drop operation
- Library for handling the Steam Controller input without the Steam SDK
- Billboarding

### Changed
- Improved ImGui interface
- Default CMake flags for the project

### Removed
- Unnecessary code from CMakeLists.txt files

### Fixed
- Partly fixed issue with a `0` sign appearing in names of installed executable files
- Accidental generation of documentation of the thirdparty code

### Security

## [0.1.0-alpha][0.1.0-alpha] - 2020-04-07
### Added
- CMake capable of generating Windows installers
- Most of the thirdpart libraries used in the project linked to appropriate targets
- Demo of audio support with OpenAL-soft
- Basic ImGui debug interface
- Initial test of lua bindings
- Initial test of logging and window handling in separate threads
- Generation of MSI and EXE installers for Windows
- Option to compile the game with or without the command line window visible

[Unreleased]: https://github.com/mkotowski/WARdrobe/compare/v0.1.0-alpha...HEAD
[0.1.0-alpha]: https://github.com/mkotowski/WARdrobe/releases/tag/v0.1.0-alpha