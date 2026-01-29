# Defold DAABBCC3D - Copilot Instructions

## Repository Overview

This is a **native C++ extension** for the Defold game engine that implements a Dynamic AABB (Axis-Aligned Bounding Box) Tree for 3D collision detection. The extension is built on Box2D's Dynamic Tree algorithm, providing fast overlap testing for moving objects. This is NOT a physics engine - it only provides broad-phase collision detection.

**Repository Size**: ~750KB  
**Lines of Code**: ~5,300 (C++, Lua)  
**Languages**: C++11, Lua  
**Framework**: Defold Engine (native extension SDK)  
**Target Platforms**: Cross-platform (Windows, macOS, Linux, iOS, Android, HTML5)

## Project Structure

### Directory Layout

```
/
├── .github/                    # GitHub metadata (funding, images)
├── daabbcc3d/                  # Main native extension
│   ├── ext.manifest           # Extension manifest (C++ flags: -std=c++11)
│   ├── annotation.lua         # Lua API annotations for IDE support
│   ├── include/daabbcc3d/     # Public C++ headers
│   │   ├── daabbcc.h         # Main API header
│   │   ├── collision.h       # Collision structures (raycast, tree nodes)
│   │   ├── aabb.h            # AABB structures
│   │   ├── math_functions.h  # 3D math utilities
│   │   ├── core.h            # Core definitions, assertions
│   │   ├── constants.h       # Constants, flags, default values
│   │   └── base.h            # Base allocator and assert prototypes
│   └── src/
│       ├── extension.cpp      # Lua C API bindings (main entry point)
│       └── daabbcc3d/        # Implementation
│           ├── daabbcc.cpp   # Core DAABBCC functionality
│           ├── dynamic_tree.cpp  # Box2D Dynamic Tree implementation
│           ├── aabb.cpp      # AABB operations
│           ├── core.cpp      # Memory allocation, assertions
│           └── math_functions.cpp  # 3D math implementations
├── example/                    # Demo/test application
│   ├── assets/                # Images, sprites
│   ├── components/            # Defold game objects and collections
│   │   ├── collections/      # Scene files (.collection)
│   │   └── gameobjects/      # Game object files (.go)
│   └── scripts/lib/          # Lua helper scripts
│       ├── manager.lua       # Example manager
│       ├── collision.lua     # Collision utilities
│       ├── camera.lua        # 3D camera utilities
│       └── utils.lua         # General utilities
├── input/                      # Input bindings
├── game.project               # Defold project configuration
├── README.md                  # Basic project info
└── LICENSE.md                 # Dual MIT license (Box2D + this extension)
```

### Key Files

- **daabbcc3d/src/extension.cpp**: Lua C API entry point. Contains all Lua-to-C++ bindings.
- **daabbcc3d/include/daabbcc3d/daabbcc.h**: Main API header defining all public functions.
- **daabbcc3d/src/daabbcc3d/dynamic_tree.cpp**: Core Box2D Dynamic Tree implementation (~1,800 LOC).
- **daabbcc3d/ext.manifest**: Extension manifest defining the C++ symbol name "DAABBCC3D" and build flags.
- **game.project**: Defold project file with extension configuration values.

## Architecture

### Extension Architecture

This is a **Defold native extension** written in C++11 that exposes Lua APIs to the game engine:

1. **Lua API Layer** (`extension.cpp`):
   - Exposes ~20 Lua functions (new_group, insert_aabb, query_aabb, raycast, etc.)
   - Uses Defold SDK macros: `DM_LUA_STACK_CHECK`, `DM_LUA_ERROR`, `DM_DECLARE_EXTENSION`
   - Registers module as "daabbcc3d" in Lua

2. **Core Implementation** (`daabbcc3d/daabbcc.cpp`):
   - Manages multiple Dynamic Tree groups
   - Handles game object position updates
   - Query and raycast operations
   - Configuration: max_group_count, max_gameobject_count, max_query_result_count

3. **Dynamic Tree** (`dynamic_tree.cpp`):
   - Box2D's Dynamic Tree ported to 3D
   - Self-balancing binary tree for efficient AABB queries
   - Supports incremental, partial, and full rebuilds
   - Copyright: Erin Catto (Box2D author)

4. **Math & Utilities**:
   - Custom 3D vector math (b2Vec3)
   - AABB operations and manifold calculations
   - Custom memory allocation through Defold SDK

### Configuration (game.project)

The extension reads configuration from `game.project`:

```ini
[daabbcc]
max_group_count = 1              # Max number of tree groups
max_gameobject_count = 10        # Max tracked game objects
max_query_result_count = 10      # Max query results

[display]
update_frequency = 0             # Game update frequency (0 = vsync)

[library]
include_dirs = daabbcc3d        # Include extension in build

[native_extension]
app_manifest = /test.appmanifest  # App manifest (optional)
```

### API Categories

1. **Group Operations**: new_group, remove_group
2. **Proxy Operations**: insert_aabb, insert_gameobject, update_aabb, remove
3. **Query Operations**: query_aabb, query_id, query_aabb_sort, query_id_sort
4. **Raycast Operations**: raycast, raycast_sort
5. **Tree Operations**: rebuild, rebuild_all
6. **Gameobject Updates**: run, update_frequency
7. **Utilities**: reset

## Build Process

### Building with Defold

**IMPORTANT**: This is a Defold native extension. It **cannot be built standalone** using Make, CMake, or local C++ compilers. The extension must be built through the Defold build server or Defold Editor.

### Build Methods

#### Method 1: Defold Editor (Recommended for Development)
1. Open the project in Defold Editor (https://defold.com/download/)
2. The editor automatically fetches dependencies and builds native extensions
3. Press Ctrl+B (Cmd+B on Mac) to build
4. Press F5 to run the example application
5. **No local C++ toolchain required** - building happens on Defold's build servers

#### Method 2: Bob Command Line Tool (for CI/CD)
1. Download bob.jar from https://github.com/defold/defold/releases
2. Build command: `java -jar bob.jar --archive --platform x86_64-linux build`
3. Platforms: x86_64-linux, x86_64-macos, arm64-ios, armv7-android, js-web, etc.
4. Bob compiles native extensions on Defold's build servers

#### Method 3: As a Dependency in Another Project
Add to another project's `game.project`:
```ini
[project]
dependencies = https://github.com/selimanac/defold-daabbcc3d/archive/refs/heads/main.zip
```
The extension builds automatically when you build the parent project.

### Important Build Notes

- **No Makefiles**: This project has no Makefiles or CMakeLists.txt
- **No local compilation**: Cannot compile with g++, clang, or MSVC directly
- **Cloud builds**: Native code compiles on Defold's build infrastructure
- **Cross-compilation**: Single source builds for all platforms automatically
- **C++11 standard**: Set in ext.manifest (`flags: ["-std=c++11"]`)

### Testing

**There are no automated unit tests.** The `example/` directory contains a visual demo application:

1. Build and run the project in Defold Editor
2. The example demonstrates:
   - Query AABB: Overlap detection with AABBs
   - Query ID: Query by AABB ID
   - Raycast: Ray intersection tests
   - Manifold: Collision manifolds with contact points
3. Interactive controls (see example/scripts/lib/manager.lua)

**To verify changes**:
1. Make your C++ or Lua changes
2. Build in Defold Editor (Ctrl+B / Cmd+B)
3. Run (F5) and test the example scenes
4. Check console for errors (use `dmLogInfo`, `dmLogError` in C++)

## Code Conventions

### C++ Code Style

- **Naming**: Snake_case for functions, CamelCase for types
- **Namespaces**: All code in `daabbcc3d` namespace
- **Prefixes**: Box2D types use `b2` prefix (b2AABB, b2Vec3, b2TreeNode)
- **Macros**: Capital letters with underscores (B2_NULL_INDEX, B2_HUGE)
- **Comments**: Minimal - only for TODOs and complex algorithms
- **Headers**: SPDX license headers (Erin Catto/MIT for Box2D code)

### Lua Code Style

- **Naming**: snake_case for all Lua functions and variables
- **Module**: All APIs exposed as `daabbcc3d.function_name()`
- **Documentation**: annotation.lua provides LSP/IDE type hints
- **Error handling**: C++ errors propagate to Lua via `DM_LUA_ERROR`

### Important Patterns

1. **Group validation**: Always check `SetTreeGroup(groupID)` before operations
2. **Stack checks**: Use `DM_LUA_STACK_CHECK(L, return_count)` in all Lua bindings
3. **Memory management**: Use Defold SDK allocators, not malloc/free
4. **Assertions**: Use `B2_ASSERT()` for debug checks (Box2D style)
5. **Logging**: Use `dmLogInfo()`, `dmLogWarning()`, `dmLogError()` (Defold SDK)

## Known Issues & TODOs

The codebase contains several TODOs (preserved from Box2D):

1. `daabbcc.cpp`: "TODO Find a better way" - DT calculation
2. `dynamic_tree.cpp`: Multiple optimization TODOs
   - "todo avoid building freelist?"
   - "TODO_ERIN optimize"
   - "todo use a node growth metric instead of simply enlarged"

**Do not modify these TODOs** unless specifically addressing the optimization issue.

## Common Gotchas

1. **No standalone builds**: Don't try to compile with g++/clang directly
2. **Extension name**: C++ symbol "DAABBCC3D" must match ext.manifest
3. **Lua module name**: "daabbcc3d" (lowercase) is the Lua module
4. **3D vs 2D**: This is the 3D version; there's a separate 2D version
5. **Not a physics engine**: No narrow-phase collision, no physics simulation
6. **Group IDs**: uint8_t (0-255), allocated sequentially, can overflow
7. **Memory limits**: Hardcoded max counts in game.project
8. **Update loop**: Game objects auto-update positions if registered

## Making Changes

### When editing C++ code:

1. Modify source files in `daabbcc3d/src/` or headers in `daabbcc3d/include/`
2. **Do not** add Makefiles, CMakeLists.txt, or build scripts
3. If adding new files, they're automatically included by Defold's build system
4. Build in Defold Editor - any compilation errors will appear in editor console
5. Check Defold Editor build output for errors (typically native extension errors are clear)

### When editing Lua APIs:

1. Modify `daabbcc3d/src/extension.cpp` for C bindings
2. Update `daabbcc3d/annotation.lua` for IDE support
3. Lua function signature must match annotations exactly
4. Remember to register new functions in `Module_methods` array

### When testing:

1. Run example application in Defold Editor
2. Use dmLogInfo() liberally for debugging
3. Check console output for errors and logs
4. No automated test framework - manual verification only

## Documentation

- **Wiki**: https://github.com/selimanac/defold-daabbcc3d/wiki (detailed API documentation)
- **Discussions**: https://github.com/selimanac/defold-daabbcc3d/discussions
- **Box2D reference**: https://box2d.org/documentation/
- **Defold SDK**: https://defold.com/ref/stable/dmExtension/
- **Defold native extensions**: https://defold.com/manuals/extensions/

## Trust These Instructions

These instructions are comprehensive and verified. Only search for additional information if you find errors or encounter situations not covered here. The Defold build process is unique - do not attempt to apply standard C++ build practices.
