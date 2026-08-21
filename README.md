# Pathfinding Sandbox

This is the combination of two playground scene to experiment with a dynamic pathfinding algorithm.
Create agents and give them objectives & patrols while you modify their environment, 
then witness the efficiency of the A* algorithm.

![screenshot or gif placeholder](res/screenshot.png)

## Features

- 2 Sandbox in 2/3 dimensions
- Agent & path creation system
- Map Editor
- Dynamic path changes
- Patrol mode
- Path preview

## Tech Stack

- **Language:** C++
- **Rendering:** SFML / Custom Dx12 Engine
- **Build system:** Custom
- **Platform:** Windows

## Project Structure

```
src/         → Engine and Sandbox source code
vendor/      → Third-party dependencies (SFML & GCRender)
res/         → Runtime resources (fonts, maps, etc.)
```

## Download & Run

The easiest way to try this out — no build tools required:

1. Go to the [**Releases**](https://github.com/vivienSINGIER/PathFinding/releases) page.
2. Download the latest `PathFinding-Sandbox-Demo.zip` under **Assets**.
3. Extract the zip anywhere.
4. Double-click **`2DSandbox.exe`** to launch.

> **Note:** if Windows shows a SmartScreen warning on first launch, click *More info → Run anyway* — this is expected for unsigned indie/portfolio builds.

## Building from Source

If you'd rather build it yourself:

```bash
git clone https://github.com/vivienSINGIER/PathFinding.git
cd PathFinding
```

1. Run `make.bat` in the bin folder.
2. Open the generated solution in Visual Studio.
3. Build the **2DSandbox** or **3DSandbox** project.
4. Run via the ide or the `2DSandbox.exe` or `3DSandbox.exe` inside the ide folder.

**Requirements:**
- Visual Studio 2022 (or compatible MSVC toolset)
- Windows 10/11

## Inputs 2D

| Key / Input | Action |
|---|---|
| `B` | Toggle walkable state of selected cell |
| `Middle Click` | Select cell |
| `A + Left Click` | Agent creation |
| `Left Click` | Select agent |
| `Right Click` | Add target |
| `LControl + Right Click` | Add secondary target |
| `Tab` | Preview path |
| `P` | Toggle patrol mode |
| `Add` | Increase agent speed |
| `Subtract` | Decrease agent speed |
| `Suppr / Del` | Delete agent |
| `F1 / F2` | Change map preset |
| `Control + S` | Save map preset |

## Inputs 3D

| Key / Input | Action |
|---|---|
| `Up / Down / Right / Left` | Move Cursor |
| `B` | Toggle walkable state of selected cell |
| `RShift` | Increase height of selected cell |
| `RControl` | Decrease height of selected cell |
| `A` | Agent creation |
| `Page_Up / Page_Down` | Change selected agent |
| `Enter` | Set destination |
| `Enter + LShift` | Set next destination |
| `Tab` | Preview path |
| `P` | Toggle patrol mode |
| `Add` | Increase agent speed |
| `Subtract` | Decrease agent speed |
| `BackSpace` | Delete agent |
| `Numpad 4 / 6` | Rotate camera |
| `Numpad 5 / 8` | Change camera height |
| `Numpad 1 / 3` | Set zoom |
| `Space` | Reset camera |
