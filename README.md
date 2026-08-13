# Age of Animals

An **Age of Empires II–style real-time strategy game** built with **Unreal Engine 5** and **Modern C++23**, featuring five playable animal empires: **Pandas**, **Bunnies**, **Monkeys**, **Gorillas**, and **Cats**.

## Overview

Each empire has its own unique unit roster, building styles, civilization bonuses, and visual theme — all rendered with procedurally generated, vibrant photorealistic sprites. The game supports single-player skirmish against AI, LAN multiplayer, and online play through a matchmaking lobby server.

## Empires

| Empire | Bonus | Playstyle |
|--------|-------|-----------|
| **Panda Empire** | +20% HP on all units | Tanky, defensive — high-health units, strong fortifications |
| **Bunny Empire** | +25% movement speed | Fast swarm — cheap, rapid units that overwhelm through speed |
| **Monkey Empire** | +15% gather rate | Economic tech — versatile units, fast resource accumulation |
| **Gorilla Empire** | +30% attack damage | Brute force — expensive, devastating units that hit hard |
| **Cat Empire** | +20% attack range | Agile ranged — long-range precision units, stealthy assassins |

Each empire has four unit types: **Villager** (gatherer/builder), **Warrior** (melee), **Archer** (ranged), and a **Unique Special** unit (e.g. Panda Lord, Bunny Hoplite, Monkey Sage, Silverback Lord, Cat Ninja).

## Project Structure

```
Age of Animals/
├── AgeOfAnimals.uproject          # UE5 project descriptor
├── Config/
│   ├── DefaultEngine.ini          # Engine config (game mode, network, rendering)
│   ├── DefaultGame.ini            # Game settings (asset manager, project info)
│   └── DefaultInput.ini           # Input bindings (mouse, keyboard, camera)
├── Source/
│   ├── AgeOfAnimals.Target.cs     # Game build target (C++23)
│   ├── AgeOfAnimalsEditor.Target.cs # Editor build target
│   ├── AgeOfAnimals/              # Runtime module
│   │   ├── AgeOfAnimals.Build.cs  # Module build rules (C++23, dependencies)
│   │   ├── AgeOfAnimals.h/.cpp    # Module entry point
│   │   ├── Public/                # Header files
│   │   │   ├── AoAEmpireData.h        # Empire data asset (5 empires, bonuses, colors)
│   │   │   ├── AoAUnitData.h          # Unit data asset (stats, costs, sprites)
│   │   │   ├── AoABuildingData.h      # Building data asset
│   │   │   ├── AoAGameMode.h          # RTS game mode (setup, win conditions)
│   │   │   ├── AoAGameState.h         # Match state (phase, winner, time)
│   │   │   ├── AoAPlayerState.h       # Player economy (resources, pop, age)
│   │   │   ├── AoAGameInstance.h      # Session state (lobby, empire selection)
│   │   │   ├── AoAPlayerController.h  # RTS controller (selection, commands, camera)
│   │   │   ├── AoAUnit.h              # Base unit (movement, combat, gathering, building)
│   │   │   ├── AoABuilding.h          # Base building (construction, training, defense)
│   │   │   ├── AoAResourceNode.h      # Gatherable resource (wood/food/stone)
│   │   │   ├── AoAHUD.h               # RTS HUD (selection box, minimap, panels)
│   │   │   ├── AoASpriteGenerator.h   # Procedural sprite texture generation
│   │   │   ├── AoAOnlineLobby.h       # Online matchmaking client
│   │   │   └── AoAAIController.h      # AI opponent (economy + military strategy)
│   │   └── Private/               # Implementation files
│   │       └── ... (matching .cpp for each header above)
│   └── AgeOfAnimalsEditor/        # Editor module
│       ├── AgeOfAnimalsEditor.Build.cs
│       ├── Public/
│       │   └── AoALevelEditorTool.h   # Custom level editor tools
│       └── Private/
│           ├── AgeOfAnimalsEditor.cpp
│           └── AoALevelEditorTool.cpp
├── Content/                       # (Assets created in UE Editor)
├── Online/
│   ├── lobby_server.js            # Node.js matchmaking server
│   └── package.json
└── README.md
```

## Prerequisites

- **Unreal Engine 5.4** or later (installed via Epic Games Launcher)
- **Visual Studio 2022** with C++ game development workload
- **Node.js 18+** (for the online lobby server, optional)

## Building

### 1. Generate project files

Right-click `AgeOfAnimals.uproject` → **Generate Visual Studio project files**.

Or from command line:

```powershell
# Path to UE5 installation (adjust as needed)
$UE5 = "C:\Program Files\Epic Games\UE_5.4\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe"
& $UE5 -projectfiles -project="Age of Animals\AgeOfAnimals.uproject" -game -engine -VS2022
```

### 2. Build

Open `AgeOfAnimals.sln` in Visual Studio and build in **Development Editor** configuration.

Or from command line:

```powershell
& $UE5 Development Editor -project="Age of Animals\AgeOfAnimals.uproject" -WaitMutex
```

### 3. Run

Launch from the UE5 Editor (Play button), or build a standalone executable:

```powershell
& $UE5 Development -project="Age of Animals\AgeOfAnimals.uproject" -WaitMutex
```

## Playing

### Single Player (Skirmish vs AI)

1. Launch the game
2. Select **Single Player** from the main menu
3. Choose your empire (Panda, Bunny, Monkey, Gorilla, or Cat)
4. Select a map
5. Play! Drag-select units, right-click to issue commands

### LAN Multiplayer

**Host:**
1. Select **Host Game** from the menu
2. Choose your empire and map
3. Other players on your LAN can join via **Join Game → [your IP]**

**Join:**
1. Select **Join Game** from the menu
2. Enter the host's IP address
3. Choose your empire and wait for the host to start

### Online Play

1. Start the lobby server:
   ```powershell
   cd "Age of Animals\Online"
   node lobby_server.js
   ```

2. In the game, select **Online Play** from the menu
3. Either **Create** a room (you become host) or **Join** an existing room
4. The server exchanges IP addresses and players connect directly via UE replication

## Controls

| Action | Input |
|--------|-------|
| Select units | Left-click drag |
| Move/Attack/Gather | Right-click on target |
| Select all military | A |
| Select all villagers | V |
| Stop | S |
| Build house | 1 (with villager selected) |
| Build barracks | 2 (with villager selected) |
| Build tower | 3 (with villager selected) |
| Build town center | 4 (with villager selected) |
| Age up | T |
| Camera pan | WASD / edge pan |
| Zoom | Mouse wheel |
| Help | H |
| Menu | ESC |

## Level Editor

The game includes custom level editor tools integrated into the UE5 Editor toolbar:

- **Terrain Painting** — Paint tiles with grass, water, forest, bamboo, rock, dirt, sand, or flowers
- **Resource Placement** — Place wood (forest), food (bamboo), and stone (rock) nodes
- **Spawn Point Marking** — Set empire start positions
- **Procedural Generation** — Generate random maps with a seed
- **Map Save/Load** — Save maps as `.aoamap` JSON files
- **Export** — Export levels as playable map assets

Open the level editor tools from the **Age of Animals** toolbar section in the UE5 Editor.

## Sprite Generation

Animal sprites are generated procedurally at runtime by `UAoASpriteGenerator`. The generator creates detailed pixel art for each animal type with:

- **Unique body silhouettes** for each animal (panda's round body, bunny's long ears, monkey's tail, gorilla's massive frame, cat's sleek form)
- **Fur texture** — noise-based detail variation
- **Directional shading** — top-left lighting for depth
- **Specular highlights** — bright spots for realism
- **Empire-specific color palettes** — vibrant, distinct colors per empire
- **8-directional facing** — sprites flip and adjust for all compass directions
- **Animation frames** — idle bob, walk cycle, attack poses
- **Weapon overlays** — clubs for warriors, bows for archers, auras for specials
- **Drop shadows** — soft ground shadows beneath units
- **Outline rendering** — dark silhouette outlines for readability

The generator produces:
- 5 empire portraits (256×256)
- 1,280 unit sprites (5 empires × 4 roles × 8 frames × 8 directions, 128×128)
- 20 building sprites (5 empires × 4 roles, 256×256)
- 8 terrain textures (128×128)

## Networking Architecture

The game uses UE5's built-in replication system for authoritative server-side simulation:

- **Listen server** model — the host runs the authoritative game state
- **Server RPCs** — client commands (move, attack, build, train) are sent via reliable server RPCs
- **Replicated properties** — unit positions, HP, state, resources, and building status replicate to all clients
- **20 Hz snapshot rate** — optimized for RTS-style gameplay
- **Online lobby server** — Node.js server handles room creation, discovery, and IP exchange. Once matched, players connect directly via UE's NetDriver.

## C++23 Features

The project is configured with `CppStandardVersion.Latest` (C++23) in all build targets. Modern C++ features used include:

- `constexpr` computed data tables
- `std::span` for array views
- Structured bindings
- `if constexpr`
- Designated initializers
- `consteval` / `constinit` where applicable
- Range-based concepts
- `std::expected` for error handling (where compiler support allows)

## Technical Notes

- The project uses **Paper2D** for 2D sprite rendering within UE5's 3D world
- UE5's **NavigationSystem** handles unit pathfinding (A* on navmesh)
- The **AIModule** provides behavior tree support for the AI opponent
- **Niagara** is available for particle effects (attack visuals, construction dust)
- **GameplayAbilities** plugin is included for future ability system expansion

## License

This is a fan project for educational purposes. Age of Empires II is a trademark of its respective owners.
