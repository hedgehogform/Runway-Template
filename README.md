# Runway Template - Neodash Mod Template

A mod template for [Neodash](https://store.steampowered.com/app/1514950/Neodash/) using Unreal Engine 4.26.

## Overview

This template provides a basic Unreal Engine 4 project structure for creating Neodash mods. Use this as a starting point to develop custom content and modifications for Neodash.

## Prerequisites

- **Unreal Engine 4.26** installed from Epic Games Launcher
- **Visual Studio 2019** (Community edition or higher)
- **Windows 10/11** (64-bit)
- **.NET Framework 4.7.2** or later

## Getting Started

### 1. Clone the Repository

```bash
git clone <your-repository-url>
cd Runway-Template
```

### 2. Generate Project Files

Right-click on `Runway.uproject` and select **"Generate Visual Studio project files"**

### 3. Build the Project

Open `Runway.sln` in Visual Studio and build the solution:
- Set configuration to **Development Editor**
- Set platform to **Win64**
- Build Solution (Ctrl+Shift+B)

### 4. Launch the Editor

- Double-click `Runway.uproject` to launch the Unreal Editor
- Or press F5 in Visual Studio to launch with debugging

## Project Structure

```
Runway-Template/
├── Content/              # Your mod content and assets go here
├── Source/
│   ├── Runway/          # C++ source code
│   ├── RunwayEditor.Target.cs
│   └── RunwayGame.Target.cs
├── Runway.uproject      # Unreal project file
└── Runway.sln          # Visual Studio solution
```

## Development

### Adding New Content

1. Launch the editor by double-clicking `Runway.uproject`
2. Create your mod content in the **Content/** folder
3. Add Blueprints, materials, meshes, and other assets as needed

### Adding C++ Code (Optional)

1. In Unreal Editor: **Tools → New C++ Class**
2. Choose parent class and name your class
3. Rebuild the project in Visual Studio
4. Implement your functionality

### Referencing Neodash Game Classes

Need to reference Neodash's game classes, functions, or structs? Check out the [Runway-CXX-Headers](https://github.com/hedgehogform/Runway-CXX-Headers) repository, which contains dumped C++ headers from Neodash. These headers can help you understand the game's structure and create mods that interact with existing game systems.

## Troubleshooting

### Build Errors
If you encounter build errors:
1. Clean and rebuild the solution in Visual Studio
2. Regenerate project files (right-click `Runway.uproject`)
3. Check that UE 4.26 is properly installed

### Editor Crashes
- Verify Unreal Engine 4.26 installation
- Check logs in `Saved/Logs/`
- Rebuild in Development Editor configuration

## Related Links

- [Neodash on Steam](https://store.steampowered.com/app/1514950/Neodash/)
- [Runway-CXX-Headers](https://github.com/hedgehogform/Runway-CXX-Headers) - Dumped C++ headers for Neodash game classes
- [Unreal Engine 4.26 Documentation](https://docs.unrealengine.com/4.26/)
- [UE4SS - UHT Compatible Headers Guide](https://docs.ue4ss.com/guides/generating-uht-compatible-headers.html)
