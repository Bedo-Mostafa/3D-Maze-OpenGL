# 3D Maze Explorer - Ultimate Edition

A first-person 3D maze game built with modern C++ and OpenGL, featuring atmospheric lighting, collectibles, and survival mechanics.

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![C++](https://img.shields.io/badge/C%2B%2B-17-00599C.svg)
![OpenGL](https://img.shields.io/badge/OpenGL-3.3-green.svg)

## Features

### Core Gameplay
- **Procedurally Generated Mazes**: Each playthrough offers a unique maze layout using recursive backtracker algorithm
- **First-Person Navigation**: Smooth camera controls with realistic head bobbing
- **Collectible System**: Gather glowing orbs scattered throughout the maze
- **Victory Condition**: Find the teleport portal to escape the maze

### Visual Features
- **Dynamic Lighting System**
  - Battery-powered flashlight with drain mechanics
  - Atmospheric fog rendering
  - Glowing portal light source
  - Phong lighting model with ambient, diffuse, and specular components
- **Advanced Effects**
  - Screen flash feedback on collectible pickup
  - Animated collectibles with hover and rotation
  - Particle-like portal effects
  - Real-time minimap with fog of war

### Audio
- Ambient background music
- Footstep sounds synchronized with movement
- Collection sound effects
- Victory fanfare
- 3D positional audio support

## System Requirements

### Minimum Requirements
- **OS**: Windows 7/8/10/11 (64-bit)
- **CPU**: Dual-core processor, 2.0 GHz
- **RAM**: 2 GB
- **GPU**: OpenGL 3.3 compatible graphics card
- **Storage**: 50 MB available space

### Recommended Requirements
- **OS**: Windows 10/11 (64-bit)
- **CPU**: Quad-core processor, 3.0 GHz
- **RAM**: 4 GB
- **GPU**: Dedicated graphics card with 1GB VRAM
- **Storage**: 100 MB available space

## Dependencies

### Required Libraries
- **SFML 2.5+**: Window management, input handling, and audio
- **GLEW 2.1+**: OpenGL extension loading
- **OpenGL 3.3+**: Graphics rendering

### Asset Files
- `textures/wall.png`: Wall texture
- `textures/floor.png`: Floor texture
- `textures/ceiling.png`: Ceiling texture (optional, uses floor as fallback)
- `shaders/vertex.glsl`: Main vertex shader
- `shaders/fragment.glsl`: Main fragment shader
- `shaders/teleport_vertex.glsl`: Portal vertex shader
- `shaders/teleport_fragment.glsl`: Portal fragment shader
- `shaders/collectible_vertex.glsl`: Collectible vertex shader
- `shaders/collectible_fragment.glsl`: Collectible fragment shader
- `audio/ambient.wav`: Background music
- `audio/collect.wav`: Collection sound effect
- `audio/footstep.wav`: Footstep sound
- `audio/win.wav`: Victory sound
- `RubikMaze-Regular.ttf`: UI font (falls back to Arial if unavailable)

## Building from Source

### Prerequisites
1. Install Visual Studio 2019 or later with C++17 support
2. Download and install SFML 2.5+ for Visual C++
3. Download and install GLEW 2.1+

### Compilation Steps

1. Clone the repository:
```bash
git clone https://github.com/yourusername/3d-maze-explorer.git
cd 3d-maze-explorer
```

2. Configure include directories in your IDE:
   - Add SFML include path
   - Add GLEW include path

3. Configure library directories:
   - Add SFML lib path
   - Add GLEW lib path

4. Link required libraries:
   - `sfml-graphics.lib`
   - `sfml-window.lib`
   - `sfml-system.lib`
   - `sfml-audio.lib`
   - `glew32.lib`
   - `opengl32.lib`

5. Build the project in Release mode

6. Copy required DLLs to output directory:
   - `sfml-graphics-2.dll`
   - `sfml-window-2.dll`
   - `sfml-system-2.dll`
   - `sfml-audio-2.dll`
   - `glew32.dll`
   - `openal32.dll`

## Controls

### Movement
- **W / ↑**: Move forward
- **S / ↓**: Move backward
- **A / ←**: Strafe left
- **D / →**: Strafe right
- **Mouse**: Look around

### Actions
- **F**: Toggle flashlight
- **ESC**: Exit game

### UI Interactions (Win Screen)
- **Left Click**: Interact with buttons
- **Restart Button**: Start a new maze
- **Exit Button**: Close the game

## Gameplay Tips

1. **Manage Your Battery**: The flashlight drains over time. Use it wisely in dark areas.
2. **Follow the Light**: The portal emits a cyan glow visible from a distance.
3. **Collect Orbs**: Gathering collectibles adds to your score and provides visual feedback.
4. **Use the Minimap**: The top-right minimap shows your position, walls, collectibles, and the exit.
5. **Listen Carefully**: Audio cues can help you navigate and track your progress.

## Architecture Overview

### Core Components

#### Game Systems
- **Game**: Main game loop and state management
- **Renderer**: OpenGL rendering pipeline with shader management
- **AudioManager**: SFML-based 3D audio system
- **InputHandler**: Keyboard and mouse input processing

#### World & Navigation
- **Maze**: Procedural maze generation and collision detection
- **Player**: First-person controller with physics
- **Camera**: View and projection matrix calculations

#### Visual Systems
- **Mesh**: Vertex buffer and index buffer management
- **Texture**: OpenGL texture loading and binding
- **ShaderProgram**: GLSL shader compilation and uniform management
- **Lighting**: Multi-source lighting with flashlight, portal, and ambient

#### Gameplay Features
- **CollectibleManager**: Collectible spawning, animation, and collision
- **FlashlightSystem**: Battery simulation and intensity calculation
- **ScreenFlashEffect**: Visual feedback overlay system
- **GameUI**: HUD, minimap, and win screen rendering

### Design Patterns
- **Component-Based Architecture**: Separation of concerns with modular systems
- **Resource Management**: RAII principles for OpenGL resources
- **State Machine**: Game state transitions (Playing, Won)

## Performance Optimization

- **Frustum Culling**: Off-screen geometry is not rendered
- **Batch Rendering**: Walls, floors, and ceilings rendered in single draw calls
- **Efficient Collision Detection**: 3x3 grid checks with AABB intersection
- **Mipmapping**: Texture LOD for distant surfaces
- **VSync**: Prevents screen tearing and limits frame rate

## Known Issues

- Ceiling texture defaults to floor texture if not found
- Audio may have initialization warnings on some systems
- Fullscreen mode required (no windowed mode option)

## Future Enhancements

- [ ] Multiple difficulty levels with larger mazes
- [ ] Enemy AI with pathfinding
- [ ] Power-ups and battery pickups
- [ ] Leaderboard system
- [ ] Additional maze generation algorithms
- [ ] Save/load game state
- [ ] Graphics settings menu
- [ ] Multiplayer support

## Contributing

Contributions are welcome! Please follow these guidelines:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Credits

### Development
- Core engine and gameplay systems
- Procedural maze generation algorithm
- Custom shader effects

### Third-Party Libraries
- **SFML**: Simple and Fast Multimedia Library
- **GLEW**: OpenGL Extension Wrangler Library
- **OpenGL**: Graphics rendering API
---

**Enjoy exploring the maze! Can you find your way out before your flashlight dies?**
