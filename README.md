# Argus

Argus is a high-performance, real-time **edge blending** and **geometry correction** software designed for seamless multi-projector setups on complex, non-flat surfaces.

## Key Features

- **Advanced Geometry Warping**: Utilizes a "Pillow" distortion model (with recursive grid refinement) to perfectly map projections onto curved screens or domes.
- **Seamless Edge Blending**: Configurable soft-edge blending with alpha gradients and gamma correction for invisible projector overlap.
- **Ultra-Low Latency**:
  - **Linux (Wayland)**: Native **DMABuf (Zero-Copy)** support for importing GPU buffers directly from compositors, minimizing latency.
  - **Windows**: High-performance Shared Memory IPC for desktop capture.
- **Modular Architecture**: Decoupled *Capturer* (input) and *Drawer* (render) processes for stability and flexibility.
- **Cross-Platform**: Runs on Linux (Wayland/X11) and Windows.

## Getting Started

### Prerequisites

- **CMake** 3.10 or higher
- **C++17** compatible compiler
- **OpenGL** & **GLEW**
- **Linux Dependencies**: `libwayland-dev`, `libegl1-mesa-dev`, `libdrm-dev`, `libmicrohttpd-dev`

### Installation

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Configuration

Argus is highly configurable via `config.ini`. You can adjust:

- **Geometry**: Control points for corners (NW, NE, SW, SE) and edges.
- **Blending**: Alpha values for each edge to control the blend curve.
- **Color Correction**: Global RGB and Gamma adjustments.
- **Input Source**: Select between shared memory, captured windows, or test patterns.

Example snippet:
```ini
[geometry]
# Top Edge control point
Nx=0.5
Ny=0.0
# Top-Left Corner
NWx=0.0
NWy=0.0

[blending]
# Alpha transparency for North edge
Na=0.0
```

## Architecture

- **`drawer/`**: The rendering engine. Handles OpenGL context, warping shaders/grid, and final output.
- **`capturer/`**: Handles screen or window capture.
- **`lib/`**: Shared core logic and IPC mechanisms.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
