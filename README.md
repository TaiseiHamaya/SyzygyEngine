# SyzygyEngine

[![BuildCheck](https://github.com/TaiseiHamaya/SyzygyEngine/actions/workflows/BuildCheck.yml/badge.svg)](https://github.com/TaiseiHamaya/SyzygyEngine/actions/workflows/BuildCheck.yml)

SyzygyEngine is a Windows-focused C++20 game engine with a DirectX 12 renderer, scene-based runtime framework, and integrated debug/editor tooling.

The repository is designed to be used with the included project generator, which creates a full game solution that references this engine as a submodule.

## Showcase

### Render Path Editor

![Render Path Editor](docs/media/render-path-editor.png)

### Asset Browser

![Asset Browser](docs/media/asset-browser.gif)

## Features

- DirectX 12 rendering pipeline with Shader Model 6.6
- Scene-driven runtime flow (initialize, setup, update, draw, finalize)
- Render DAG-based rendering path configuration
- Built-in asset systems for:
	- Polygon meshes
	- Textures
	- Shaders
	- Audio
	- Skeleton and node animation assets
	- Primitive geometry
- Background asset loading
- Debug/editor mode (ImGui-based) with:
	- Hierarchy and scene view tools
	- Asset browser and asset import helpers
	- Render DAG editor
	- In-editor log window
- Configurable project settings via JSON (window, graphics, frame settings)

## Requirements

- Windows 11 (with a recent SDK)
- Visual Studio 2022 (MSVC v143)
- Windows SDK 10.0.26100.0 (or compatible installed SDK)
- PowerShell (for solution generation script)
- Git (submodule workflow)

## External Libraries

The engine includes and/or references the following libraries inside the repository:

- asio
- assimp
- DirectXTex
- freetype
- imgui
- msdf-atlas-gen
- nlohmann/json

## Quick Start (Recommended)

This is the same flow used by CI.

1. Open a PowerShell terminal in a directory where you want to create your game project.
2. Download and run the generator script:

```powershell
curl -o CreateSolution.ps1 https://raw.githubusercontent.com/TaiseiHamaya/SyzygyEngine/refs/heads/master/ProjectGeneratorTool/CreateSolution.ps1
.\CreateSolution.ps1 MyGame
```

3. Open the generated solution:

```text
MyGame/project/MyGame.sln
```

4. Build with Visual Studio (x64, Debug / Develop / Release).

The generated solution includes:

- Game (your project, static library)
- SyzygyEngine (engine executable)
- DirectXTex
- imgui
- Asio

## Build Configurations

- `Debug`:
	- Debug runtime
	- Editor/debug features enabled (`DEBUG_FEATURES_ENABLE`)
- `Develop`:
	- Release runtime + debug features enabled
	- Useful for development performance checks with editor tools
- `Release`:
	- Release runtime
	- Debug/editor features disabled

## Running and Editor Notes

- Entry point is `WinMain` from the generated game script.
- The framework loop initializes engine systems, processes frames, and finalizes automatically.
- In debug-feature builds, the editor can be toggled during runtime (default shortcut includes `F6`).
- Command line option:
	- `--editor-disable`: start without editor UI even when debug features are compiled.

## Project Structure

```text
SyzygyEngine/
	Engine/
		Application/     # Framework, window app, logging, project settings
		Assets/          # Asset types and libraries (mesh, texture, shader, audio, animation)
		Debug/           # Editor, profiler, ImGui integration
		GraphicsAPI/     # Rendering API layer (DirectX)
		Loader/          # Scene/world and asset loading helpers
		Module/          # World, rendering, and management modules
		Runtime/         # Scene manager, input, clock
	EngineResources/   # Engine-side HLSL and built-in resources
	Library/
		Externals/       # Third-party dependencies
		Math/            # Math library
		Utility/         # Utility layer and templates
	ProjectGeneratorTool/
		CreateSolution.ps1
		CopyFolderRoot/  # Template files for generated game projects
```

## CI

GitHub Actions verifies builds for all configurations:

- Debug
- Develop
- Release

Workflow file: `.github/workflows/BuildCheck.yml`
