# make_terrain
24-2 Graphics programming team 4 project

### Project Structure
```
make_terrain
├─ assets       : asset files (.obj, .png, etc.)
├─ includes     : header files (.h or .hpp)
├─ lib          : external files (not our implementations)
├─ shaders      : shader codes (.vs, .fs, etc.)
└─ src          : source files (.cpp)
```

### Build and Execution Guide
- In `Visual Studio Code`, install `CMake Tools` extention
- `ctrl + shift + p` → `Cmake: Configure`
- `ctrl + shift + p` → `Cmake: Build` (shortcut is `F7`)
- execute `./build/make_terrain`

### Build Troubleshooting
- install dependencies:
  - `sudo apt install libsoil-dev libglm-dev libassimp-dev libglew-dev libglfw3-dev libxinerama-dev libxcursor-dev libxi-dev libfreetype-dev libgl1-mesa-dev xorg-dev ninja-build libxkbcommon-x11-dev`
- compiler version
  - tested on gcc 9.4.0 

### Camera Control
- `W/S/A/D/Q/E`: camera movement
- `Mouse`: camera view direction **(right button should be pressed)**
