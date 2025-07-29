# The Complete Guide to C++ Development in VSCode on Linux

## Table of Contents
1. [Why VSCode is Not an IDE and What That Means](#vscode-ide)
2. [Understanding C++ Compilation: The Complete Process](#compilation-process)
3. [Compilers: GCC vs Clang - Making the Right Choice](#compilers-gcc-clang)
4. [The Four-Stage Compilation Pipeline](#compilation-pipeline)
5. [Library Linking: Static vs Dynamic Explained](#library-linking)
6. [VSCode Configuration Files: Purpose and Deep Dive](#vscode-config-files)
7. [Compilation Flags: Every Flag Has a Purpose](#compilation-flags)
8. [Build Systems: From Manual to Make to CMake](#build-systems)
9. [Complete Real-World Project Setup](#complete-project-setup)
10. [Advanced Debugging and Development Workflow](#debugging-workflow)

---

## Why VSCode is Not an IDE (and What That Means for You) {#vscode-ide}

### The Fundamental Difference

**Traditional IDEs (like Code::Blocks, Dev-C++):**
- Come with built-in compilers
- Automatically detect project structure
- Provide pre-configured build systems
- Include integrated debuggers out-of-the-box
- Work immediately after installation

**VSCode:**
- Is a **text editor with extension capabilities**
- Requires you to install and configure compilers separately
- Needs manual configuration for each project
- Depends on external tools (GCC, GDB, Make, CMake)
- Gives you complete control over every aspect

### Why Choose This Complexity?

**Professional Development Reality**: In real software development, you need to understand:
- How compilation actually works
- How to configure build systems for different platforms
- How to integrate with various compilers and tools
- How to debug complex build issues

**Learning Benefits:**
- **Deep Understanding**: You learn what actually happens when code compiles
- **Flexibility**: Can adapt to any project structure or build system
- **Problem Solving**: When things break, you know how to fix them
- **Professional Skills**: Industry-standard development practices

### What You Need to Install Separately

```bash
# Essential toolchain
sudo apt update
sudo apt install build-essential  # GCC compiler + essential tools
sudo apt install gdb             # GNU Debugger
sudo apt install make            # Build automation
sudo apt install cmake           # Modern build system generator

# Optional but recommended
sudo apt install clang clangd    # Alternative compiler + language server
sudo apt install valgrind        # Memory debugging
sudo apt install git             # Version control
```

**Why each tool matters:**
- **build-essential**: Contains GCC, standard library headers, and linker
- **gdb**: Required for step-by-step debugging in VSCode
- **make**: Traditional build system, still widely used
- **cmake**: Modern build system generator, industry standard
- **clangd**: Provides faster, more accurate IntelliSense than Microsoft's extension

---

## Understanding C++ Compilation: The Complete Process {#compilation-process}

### The Reality of C++ Compilation

C++ code cannot run directly. It must be **transformed** into machine code through a complex multi-stage process.

### Why Understanding This Matters for VSCode

VSCode needs to replicate parts of this compilation process to provide:
- **IntelliSense**: Code completion and error detection
- **Error Highlighting**: Real-time syntax and semantic analysis
- **Go to Definition**: Navigate between headers and implementations
- **Debugging**: Map source code to executable instructions

**The Key Insight**: Every VSCode configuration file exists to provide information that the compilation process already knows, but VSCode needs to discover independently.

### The Source-to-Executable Journey

```cpp
// main.cpp - Your source code
#include <iostream>
#include "calculator.h"

int main() {
    Calculator calc;
    std::cout << calc.add(5, 3) << std::endl;
    return 0;
}
```

**What happens when you compile:**

1. **Preprocessing** → **Compilation** → **Assembly** → **Linking** → **Executable**

Each stage has specific responsibilities and potential failure points that VSCode needs to understand.

---

## Compilers: GCC vs Clang - Making the Right Choice {#compilers-gcc-clang}

### GCC (GNU Compiler Collection)

**What it is**: The traditional, most widely used C++ compiler on Linux systems.

**Strengths:**
- **Mature and Stable**: Decades of development and testing
- **Excellent Optimization**: Often produces faster executables
- **Wide Platform Support**: Available on virtually every Linux distribution
- **Standard Compliance**: Excellent C++ standard library implementation
- **Debugging Integration**: Deep integration with GDB debugger

**When to use GCC:**
- Default choice for most Linux development
- Production software where performance matters
- Working with legacy codebases
- Learning C++ (most tutorials assume GCC)

### Clang (LLVM-based Compiler)

**What it is**: Modern compiler with focus on fast compilation and excellent diagnostics.

**Strengths:**
- **Superior Error Messages**: More helpful, precise error descriptions
- **Faster Compilation**: Often compiles code faster than GCC
- **Better Static Analysis**: Catches more potential bugs during compilation
- **Modular Design**: Better integration with development tools
- **Cross-Platform**: Same compiler behavior across Linux/macOS/Windows

**When to use Clang:**
- Development phase (faster iteration due to better errors)
- Large codebases (faster compilation)
- Modern C++ features (often implements standards faster)
- Cross-platform development

### Practical Compiler Selection

**Installation:**
```bash
# GCC (usually pre-installed)
sudo apt install build-essential

# Clang
sudo apt install clang clang-tools clangd
```

**Setting Default Compiler:**
```bash
# Check available compilers
which gcc g++ clang clang++
gcc --version
clang --version

# Use update-alternatives to manage defaults
sudo update-alternatives --install /usr/bin/cc cc /usr/bin/gcc 100
sudo update-alternatives --install /usr/bin/cc cc /usr/bin/clang 200
sudo update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++ 100
sudo update-alternatives --install /usr/bin/c++ c++ /usr/bin/clang++ 200
```

**In VSCode Configuration:**
```json
// c_cpp_properties.json - for Microsoft C++ extension
{
    "compilerPath": "/usr/bin/g++",     // or "/usr/bin/clang++"
    "intelliSenseMode": "linux-gcc-x64" // or "linux-clang-x64"
}
```

```txt
# compile_flags.txt - for clangd extension
-xc++
-std=c++17
-Wall
-Wextra
```

---

## The Four-Stage Compilation Pipeline {#compilation-pipeline}

### Stage 1: Preprocessing - Header File Resolution

**What happens:** The preprocessor processes all `#include`, `#define`, and `#ifdef` directives.

```cpp
// Before preprocessing
#include <iostream>
#include "calculator.h"
#define PI 3.14159

int main() {
    std::cout << "Pi = " << PI << std::endl;
    return 0;
}
```

**After preprocessing (simplified):**
```cpp
// Thousands of lines from <iostream> are inserted here
// Contents of calculator.h are inserted here

int main() {
    std::cout << "Pi = " << 3.14159 << std::endl;
    return 0;
}
```

**Header Search Process:**

For `#include <iostream>` (system headers):
1. `/usr/include/c++/11/iostream`
2. `/usr/include/c++/11/x86_64-linux-gnu/iostream`
3. Other system directories

For `#include "calculator.h"` (local headers):
1. Current directory
2. Directories specified with `-I` flags
3. System directories (as fallback)

**Why VSCode Needs This Information:**
- **IntelliSense** must find headers to provide code completion
- **Error detection** requires parsing the same headers the compiler uses
- **Go to definition** needs to know where declarations are located

**VSCode Configuration Impact:**
```json
// c_cpp_properties.json
{
    "includePath": [
        "${workspaceFolder}/include",     // For #include "myheader.h"
        "/usr/include/c++/**",           // For #include <iostream>
        "/usr/local/include"             // For third-party libraries
    ]
}
```

### Stage 2: Compilation - Source to Assembly

**What happens:** C++ source code is translated to assembly language (human-readable machine instructions).

```cpp
// C++ source
int add(int a, int b) {
    return a + b;
}
```

**Generated assembly (x86-64):**
```assembly
add(int, int):
    push    rbp
    mov     rbp, rsp
    mov     DWORD PTR [rbp-4], edi
    mov     DWORD PTR [rbp-8], esi
    mov     edx, DWORD PTR [rbp-4]
    mov     eax, DWORD PTR [rbp-8]
    add     eax, edx
    pop     rbp
    ret
```

**Compiler Flags That Affect This Stage:**
- `-std=c++17`: Language standard (affects what syntax is accepted)
- `-O2`: Optimization level (affects generated assembly)
- `-g`: Debug information (adds mapping from assembly back to source)
- `-Wall`: Warnings (helps catch potential issues)

### Stage 3: Assembly - Assembly to Object Files

**What happens:** Assembly code is converted to machine code and stored in object files (`.o`).

```bash
# Manual compilation stages
g++ -E main.cpp -o main.i          # Preprocessing only
g++ -S main.cpp -o main.s          # Compilation to assembly
g++ -c main.cpp -o main.o          # Assembly to object file
```

**Object File Contents:**
- **Machine code**: Actual CPU instructions
- **Symbol table**: Names of functions and variables
- **Relocation information**: Addresses that need to be filled in during linking
- **Debug information**: (if compiled with `-g`) mapping back to source code

**Key Insight**: Object files contain **unresolved references**. If `main.cpp` calls a function defined in `calculator.cpp`, the object file `main.o` contains a "placeholder" that says "call the function named `Calculator::add`, but I don't know where it is yet."

### Stage 4: Linking - Combining Everything

**What happens:** The linker combines object files and resolves all references.

```bash
# Separate compilation
g++ -c main.cpp -o main.o           # Contains call to Calculator::add
g++ -c calculator.cpp -o calculator.o  # Contains definition of Calculator::add

# Linking
g++ main.o calculator.o -o program  # Connects call to definition
```

**What the Linker Does:**
1. **Symbol Resolution**: Finds the definition of `Calculator::add` in `calculator.o`
2. **Address Assignment**: Assigns final memory addresses to all functions
3. **Reference Patching**: Replaces placeholders with actual addresses
4. **Library Inclusion**: Links standard library functions (like `std::cout`)

**Library Linking Types:**

**Static Linking (`.a` files):**
```bash
# Create static library
ar rcs libcalculator.a calculator.o

# Link with static library
g++ main.o -L. -lcalculator -o program
```
- Library code becomes part of your executable
- Larger executable size
- No runtime dependencies
- Faster program startup

**Dynamic Linking (`.so` files):**
```bash
# Create shared library
g++ -shared -fPIC calculator.cpp -o libcalculator.so

# Link with shared library
g++ main.cpp -L. -lcalculator -o program

# Run (library must be available at runtime)
LD_LIBRARY_PATH=. ./program
```
- Library loaded at runtime
- Smaller executable size
- Multiple programs can share the same library
- Library updates don't require recompiling programs

---

## Library Linking: Static vs Dynamic Explained {#library-linking}

### Understanding the Linking Problem

When your program uses functions from libraries (standard library, third-party libraries), the linker must:
1. **Find** the library files
2. **Extract** the needed functions
3. **Include** them in your executable (static) or **reference** them (dynamic)

### Static Linking Deep Dive

**Process:**
```bash
# Create object files
g++ -c calculator.cpp -o calculator.o
g++ -c math_utils.cpp -o math_utils.o

# Create static library
ar rcs libmath.a calculator.o math_utils.o

# Link statically
g++ main.cpp -L. -lmath -static -o program_static
```

**Advantages:**
- **Self-contained**: Executable runs on any system (no dependencies)
- **Performance**: No runtime library loading overhead
- **Predictable**: Always uses the exact library version you linked with

**Disadvantages:**
- **Size**: Large executables (each program includes full library copy)
- **Updates**: Security fixes require recompiling all programs
- **Memory**: Multiple running programs duplicate library code in RAM

### Dynamic Linking Deep Dive

**Process:**
```bash
# Create shared library with Position Independent Code
g++ -shared -fPIC calculator.cpp math_utils.cpp -o libmath.so

# Link dynamically (default behavior)
g++ main.cpp -L. -lmath -o program_dynamic

# Runtime library path
export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH
./program_dynamic
```

**The `-fPIC` Flag Explained:**
- **Position Independent Code**: Code that works regardless of where it's loaded in memory
- **Required for shared libraries**: Multiple programs loading the same library at different addresses
- **Performance cost**: Slightly slower than position-dependent code

**Advantages:**
- **Size**: Small executables
- **Updates**: Library updates benefit all programs automatically
- **Memory**: Single library copy shared by all programs

**Disadvantages:**
- **Dependencies**: Programs fail if libraries are missing or incompatible
- **Complexity**: Managing library versions and paths
- **Performance**: Runtime loading and symbol resolution overhead

### VSCode Configuration for Libraries

**In tasks.json:**
```json
{
    "args": [
        "-std=c++17",
        "-Iinclude",                    // Header search path
        "-L/usr/local/lib",             // Library search path
        "-lboost_system",               // Link with libboost_system.so
        "-lboost_filesystem",           // Link with libboost_filesystem.so
        "-pthread",                     // Link with pthread library
        "${workspaceFolder}/src/*.cpp",
        "-o", "${workspaceFolder}/build/program"
    ]
}
```

**Finding System Libraries:**
```bash
# Find library files
find /usr/lib /usr/local/lib -name "libboost*.so" 2>/dev/null

# Check library dependencies
ldd program_dynamic

# Library search paths
ldconfig -p | grep boost
```

---

## VSCode Configuration Files: Purpose and Deep Dive {#vscode-config-files}

### The Configuration Ecosystem

Each configuration file serves a specific purpose in the VSCode C++ development workflow:

```
.vscode/
├── tasks.json              # How to build your project
├── launch.json             # How to debug your project
├── c_cpp_properties.json   # How IntelliSense understands your code
├── settings.json           # Editor behavior and extension settings
└── compile_flags.txt       # Alternative IntelliSense configuration
```

### tasks.json: Teaching VSCode How to Build

**Purpose**: Defines what happens when you press `Ctrl+Shift+B` or run "Tasks: Run Build Task".

**Core Concepts:**
- **Task**: A command that VSCode can execute
- **Group**: Categories like "build", "test", "clean"
- **Problem Matcher**: How to parse compiler output for errors
- **Dependencies**: Tasks that must run before this task

**Complete Example:**
```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build Debug",
            "type": "shell",
            "command": "g++",
            "args": [
                "-std=c++17",              // Language standard
                "-g", "-O0",               // Debug flags
                "-Wall", "-Wextra",        // Warning flags
                "-Iinclude",               // Include directories
                "-DDEBUG=1",               // Preprocessor defines
                "${workspaceFolder}/src/*.cpp",  // Source files
                "-o", "${workspaceFolder}/build/debug/program"
            ],
            "group": {
                "kind": "build",
                "isDefault": true          // Default build task (Ctrl+Shift+B)
            },
            "problemMatcher": ["$gcc"],    // Parse errors for Problems panel
            "dependsOrder": "sequence",
            "dependsOn": ["Create Build Directory"]
        },
        {
            "label": "Create Build Directory",
            "type": "shell",
            "command": "mkdir",
            "args": ["-p", "build/debug"],
            "group": "build"
        },
        {
            "label": "Clean Build",
            "type": "shell",
            "command": "rm",
            "args": ["-rf", "build"],
            "group": "build"
        }
    ]
}
```

**Key Elements Explained:**

**`problemMatcher`**: 
- `"$gcc"`: Built-in pattern that parses GCC error messages
- Enables clicking on errors to jump to source code
- Shows errors in Problems panel with file/line information

**`dependsOn`**: 
- Ensures build directory exists before compilation
- Can specify multiple dependencies: `["task1", "task2"]`
- `dependsOrder`: "sequence" (run dependencies in order) or "parallel"

**VSCode Variables:**
- `${workspaceFolder}`: Project root directory
- `${file}`: Currently open file
- `${fileBasename}`: Filename without path
- `${fileDirname}`: Directory of current file

### launch.json: Teaching VSCode How to Debug

**Purpose**: Configures the debugger - what program to run, how to run it, and how to connect GDB.

**Complete Example:**
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug C++ Program",
            "type": "cppdbg",              // Use C++ debugger
            "request": "launch",           // Start new process (vs "attach")
            "program": "${workspaceFolder}/build/debug/program",
            "args": ["--verbose", "input.txt"],  // Command line arguments
            "stopAtEntry": false,          // Don't pause at main()
            "cwd": "${workspaceFolder}",   // Working directory
            "environment": [               // Environment variables
                {
                    "name": "DEBUG_LEVEL",
                    "value": "2"
                },
                {
                    "name": "LD_LIBRARY_PATH",
                    "value": "/usr/local/lib:${env:LD_LIBRARY_PATH}"
                }
            ],
            "externalConsole": false,      // Use VSCode's integrated terminal
            "MIMode": "gdb",              // Debugger backend
            "miDebuggerPath": "/usr/bin/gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for STL",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                },
                {
                    "description": "Show all array elements",
                    "text": "-gdb-set print elements 0"
                }
            ],
            "preLaunchTask": "Build Debug",  // Build before debugging
            "internalConsoleOptions": "openOnSessionStart"
        }
    ]
}
```

**Advanced Configuration Options:**

**`setupCommands`**: GDB commands executed when debugger starts
- Pretty-printing: Makes STL containers readable (`std::vector<int>` shows as `{1, 2, 3}`)
- Print elements: Show full arrays instead of truncating
- Custom breakpoints: Set breakpoints in libraries

**`sourceFileMap`**: Map build paths to source paths (useful for Docker builds)
```json
"sourceFileMap": {
    "/build/src": "${workspaceFolder}/src"
}
```

**`symbolSearchPath`**: Additional paths to search for debug symbols
```json
"symbolSearchPath": "/usr/lib/debug:/usr/local/lib/debug"
```

### c_cpp_properties.json: Teaching IntelliSense About Your Code

**Purpose**: Configures the Microsoft C++ extension's IntelliSense engine to understand your project structure.

**Why This File Exists**: IntelliSense needs to simulate compilation to provide accurate code completion and error detection. It needs the same information the compiler has.

**Complete Example:**
```json
{
    "configurations": [
        {
            "name": "Linux Debug",
            "includePath": [
                "${workspaceFolder}/include",      // Your headers
                "${workspaceFolder}/src",          // Source directory headers
                "${workspaceFolder}/external/**",  // Third-party libraries
                "/usr/include/c++/**",            // Standard library
                "/usr/local/include",             // System libraries
                "/opt/boost/include"              // Specific library paths
            ],
            "defines": [
                "DEBUG=1",                        // Debug build defines
                "_GNU_SOURCE",                    // GNU extensions
                "BOOST_ALL_NO_LIB"               // Library-specific defines
            ],
            "compilerPath": "/usr/bin/g++",       // Which compiler to simulate
            "cStandard": "c17",
            "cppStandard": "c++17",              // Language standard
            "intelliSenseMode": "linux-gcc-x64", // IntelliSense behavior mode
            "compileCommands": "${workspaceFolder}/build/compile_commands.json",
            "configurationProvider": "ms-vscode.cmake-tools",
            "browse": {
                "path": [
                    "${workspaceFolder}/**"
                ],
                "limitSymbolsToIncludedHeaders": true
            }
        }
    ],
    "version": 4
}
```

**Key Concepts:**

**`includePath` vs `browse.path`**:
- `includePath`: Used for IntelliSense analysis (what #include can find)
- `browse.path`: Used for symbol database (what Go to Definition searches)

**`compileCommands`**: Path to `compile_commands.json` (generated by CMake)
- When present, overrides `includePath` and `defines`
- Provides exact compilation flags used for each file
- Most accurate IntelliSense configuration

**`configurationProvider`**: Extension that provides compilation information
- `ms-vscode.cmake-tools`: Use CMake extension's configuration
- Automatically configures IntelliSense based on CMake settings

### compile_flags.txt: Alternative IntelliSense Configuration

**Purpose**: Provides compilation flags for the clangd language server (alternative to Microsoft's C++ extension).

**Why Use This**: clangd uses the actual Clang compiler for analysis, providing more accurate results than simulated IntelliSense.

**Example:**
```txt
-xc++
-std=c++17
-Wall
-Wextra
-Wno-unused-parameter
-Iinclude
-Isrc
-I/usr/include/c++/11
-I/usr/local/include
-DDEBUG=1
-D_GNU_SOURCE
```

**Flag Explanations:**
- `-xc++`: Treat input as C++ (even files without .cpp extension)
- `-Wall -Wextra`: Enable comprehensive warnings
- `-Wno-unused-parameter`: Disable specific warnings
- `-I`: Include directory paths
- `-D`: Define preprocessor macros

**When to Use clangd vs Microsoft C++ Extension:**
- **clangd**: Faster, more accurate, better for large projects
- **Microsoft**: Better VSCode integration, more beginner-friendly

### settings.json: Editor and Extension Configuration

**Purpose**: Configures VSCode editor behavior and extension settings for the workspace.

**C++ Specific Settings:**
```json
{
    "C_Cpp.default.cppStandard": "c++17",
    "C_Cpp.default.compilerPath": "/usr/bin/g++",
    "C_Cpp.default.intelliSenseMode": "linux-gcc-x64",
    "C_Cpp.clang_format_style": "{ BasedOnStyle: Google, IndentWidth: 4 }",
    "C_Cpp.errorSquiggles": "enabled",
    "C_Cpp.autocomplete": "default",
    "C_Cpp.suggestSnippets": true,
    
    "cmake.buildDirectory": "${workspaceFolder}/build",
    "cmake.generator": "Unix Makefiles",
    "cmake.configureOnOpen": true,
    
    "files.associations": {
        "*.h": "cpp",
        "*.tpp": "cpp"
    },
    
    "editor.formatOnSave": true,
    "editor.rulers": [80, 120],
    "editor.tabSize": 4,
    "editor.insertSpaces": true
}
```

---

## Compilation Flags: Every Flag Has a Purpose {#compilation-flags}

### Standard and Language Flags

**`-std=c++17`**: Language Standard
- **Purpose**: Tells compiler which C++ standard to use
- **Impact**: Affects available language features and library functions
- **Options**: `c++11`, `c++14`, `c++17`, `c++20`, `c++23`
- **Why it matters**: Code using C++17 features won't compile with `-std=c++11`

```cpp
// C++17 features that require -std=c++17
#include <filesystem>              // C++17 library
auto [x, y] = std::make_pair(1, 2); // Structured bindings
if constexpr (sizeof(int) == 4) {   // Constexpr if
    // ...
}
```

### Warning Flags: Catching Bugs Early

**`-Wall`**: Enable Common Warnings
```cpp
// Warning: unused variable
int main() {
    int unused_var = 42;  // Warning with -Wall
    return 0;
}
```

**`-Wextra`**: Enable Extra Warnings
```cpp
// Warning: comparison between signed and unsigned
int main() {
    int signed_val = -1;
    unsigned int unsigned_val = 10;
    if (signed_val < unsigned_val) {  // Warning with -Wextra
        // Potential bug: -1 becomes large positive number when compared
    }
    return 0;
}
```

**`-Werror`**: Treat Warnings as Errors
- **Purpose**: Forces fixing all warnings before compilation succeeds
- **When to use**: Production code, continuous integration
- **Caution**: Can be too strict for development

**Specific Warning Flags:**
```bash
-Wunused-variable          # Warn about unused variables
-Wunused-function          # Warn about unused functions
-Wshadow                   # Warn when variables shadow each other
-Wconversion               # Warn about implicit type conversions
-Wsign-conversion          # Warn about signed/unsigned conversions
-Wno-unused-parameter      # Disable unused parameter warnings
```

### Optimization Flags

**`-O0`**: No Optimization (Debug Default)
- **Purpose**: Fastest compilation, easiest debugging
- **Result**: Direct translation of source to machine code
- **When to use**: Development and debugging

**`-O1`**: Basic Optimization
- **Purpose**: Minor optimizations that don't affect debugging much
- **Result**: Slightly faster code, still debuggable

**`-O2`**: Standard Optimization (Release Default)
- **Purpose**: Good balance of speed and compilation time
- **Result**: Significantly faster code, harder to debug
- **What it does**: Function inlining, loop optimization, dead code elimination

**`-O3`**: Aggressive Optimization
- **Purpose**: Maximum performance
- **Result**: Fastest code, difficult to debug, longer compilation
- **Caution**: Can sometimes make code slower due to increased size

**`-Os`**: Optimize for Size
- **Purpose**: Minimize executable size
- **When to use**: Embedded systems, memory-constrained environments

**`-Ofast`**: Aggressive + Non-Standard Optimizations
- **Purpose**: Fastest possible code
- **Caution**: May break IEEE floating-point compliance
- **Only use if**: You understand the trade-offs

### Debug Information Flags

**`-g`**: Include Debug Information
- **Purpose**: Enables source-level debugging
- **What it adds**: Line number mapping, variable names, function boundaries
- **Cost**: Larger executable size, no performance impact on optimized code

**`-g0`**: No Debug Information
**`-g1`**: Minimal Debug Information
**`-g2`**: Default Debug Information (same as `-g`)
**`-g3`**: Maximum Debug Information (includes macro definitions)

**Debug + Optimization Combinations:**
```bash
# Development: Easy debugging, slow execution
g++ -g -O0 -DDEBUG main.cpp -o debug_program

# Release with debug info: Fast execution, some debugging capability
g++ -g -O2 -DNDEBUG main.cpp -o release_debug_program

# Pure release: Fast execution, no debugging
g++ -O3 -DNDEBUG main.cpp -o release_program
```

### Include and Library Flags

**`-I<directory>`**: Add Include Directory
```bash
g++ -Iinclude -I/usr/local/include -I../common/headers main.cpp
```
- **Purpose**: Tell preprocessor where to find header files
- **Search order**: Specified directories searched before system directories
- **Multiple `-I`**: Searched in order specified

**`-L<directory>`**: Add Library Directory
```bash
g++ main.cpp -L/usr/local/lib -L../lib -lmylib
```
- **Purpose**: Tell linker where to find library files
- **Search order**: Specified directories searched before system directories

**`-l<library>`**: Link with Library
```bash
g++ main.cpp -lboost_system -lboost_filesystem -lpthread
```
- **Purpose**: Link with specific libraries
- **Name transformation**: `-lboost_system` looks for `libboost_system.so` or `libboost_system.a`
- **Order matters**: Libraries should be listed in dependency order

### Preprocessor Definition Flags

**`-D<macro>=<value>`**: Define Preprocessor Macro
```bash
g++ -DDEBUG=1 -DVERSION=\"1.0\" -DLINUX main.cpp
```

```cpp
// Conditional compilation based on defines
#ifdef DEBUG
    #define DBG_PRINT(x) std::cout << "Debug: " << x << std::endl
#else
    #define DBG_PRINT(x)
#endif

#if VERSION >= 2
    // New features
#endif

#ifdef LINUX
    #include <sys/utsname.h>
#elif defined(WINDOWS)
    #include <windows.h>
#endif
```

### Platform and Architecture Flags

**`-march=native`**: Optimize for Current CPU
- **Purpose**: Use all available CPU instructions
- **Benefit**: Maximum performance on build machine
- **Drawback**: Binary might not run on different CPUs

**`-m64` / `-m32`**: Target Architecture
- **Purpose**: Generate 64-bit or 32-bit code
- **Default**: Usually matches build system architecture

**`-fPIC`**: Position Independent Code
- **Purpose**: Required for shared libraries
- **What it does**: Generates code that works at any memory address
- **Cost**: Slight performance overhead

### Security and Hardening Flags

**`-fstack-protector-strong`**: Stack Protection
- **Purpose**: Detect buffer overflows
- **How**: Adds canary values to detect stack corruption

**`-D_FORTIFY_SOURCE=2`**: Runtime Checks
- **Purpose**: Add bounds checking to library functions
- **Requirement**: Must be used with optimization (`-O1` or higher)

**`-Wformat-security`**: Format String Security
- **Purpose**: Warn about potentially unsafe format strings
- **Catches**: `printf(user_input)` instead of `printf("%s", user_input)`

### Linking Flags

**`-static`**: Static Linking
- **Purpose**: Include all libraries in the executable
- **Result**: Large, self-contained executable
- **Use case**: Deployment to systems without required libraries

**`-shared`**: Create Shared Library
```bash
g++ -shared -fPIC calculator.cpp -o libcalculator.so
```

**`-Wl,<option>`**: Pass Options to Linker
```bash
g++ main.cpp -Wl,-rpath,/usr/local/lib -lmylib
```
- **`-rpath`**: Set runtime library search path
- **`-soname`**: Set shared library name
- **`--as-needed`**: Only link libraries that are actually used

---

## Build Systems: From Manual to Make to CMake {#build-systems}

### The Evolution of Build Complexity

**Single File (Manual Compilation):**
```bash
g++ -std=c++17 -Wall -O2 main.cpp -o program
```
- **Works for**: Learning, simple utilities
- **Breaks down**: Multiple files, libraries, different configurations

**Multiple Files (Manual Getting Complex):**
```bash
g++ -std=c++17 -Wall -O2 -Iinclude src/main.cpp src/calculator.cpp src/utils.cpp -lm -o program
```
- **Problems**: Long command lines, no dependency tracking, rebuild everything always

**Real Projects Need:**
- **Incremental compilation**: Only rebuild changed files
- **Dependency tracking**: Rebuild when headers change
- **Configuration management**: Debug vs Release builds
- **Library management**: Finding and linking external dependencies
- **Cross-platform support**: Different commands for different systems

### Make: The Traditional Solution

**Core Concept**: Make uses **rules** to describe how to build **targets** from **dependencies**.

```makefile
# target: dependencies
#     command (must be indented with TAB, not spaces)

program: main.o calculator.o
	g++ main.o calculator.o -o program

main.o: src/main.cpp include/calculator.h
	g++ -c -Iinclude src/main.cpp -o main.o

calculator.o: src/calculator.cpp include/calculator.h
	g++ -c -Iinclude src/calculator.cpp -o calculator.o
```

**How Make Decides What to Build:**
1. **Target exists?** If `program` doesn't exist, build it
2. **Dependencies newer?** If `main.o` is newer than `program`, rebuild `program`
3. **Recursive checking**: Check if `main.o` needs rebuilding from `src/main.cpp`

**Why This is Efficient:**
- Change only `src/main.cpp` → Rebuild only `main.o` and `program`
- Change only `include/calculator.h` → Rebuild both `.o` files and `program`
- No changes → Build nothing (instant)

### Advanced Makefile Techniques

**Variables and Pattern Rules:**
```makefile
# Configuration variables
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude
SRCDIR := src
BUILDDIR := build
TARGET := $(BUILDDIR)/program

# Automatic file discovery
SOURCES := $(wildcard $(SRCDIR)/*.cpp)
OBJECTS := $(SOURCES:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)

# Default target
all: $(TARGET)

# Debug and Release configurations
debug: CXXFLAGS += -g -O0 -DDEBUG
debug: $(TARGET)

release: CXXFLAGS += -O3 -DNDEBUG  
release: $(TARGET)

# Link executable
$(TARGET): $(OBJECTS) | $(BUILDDIR)
	$(CXX) $(OBJECTS) -o $@

# Pattern rule: compile any .cpp to .o
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create build directory if it doesn't exist
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# Clean build artifacts
clean:
	rm -rf $(BUILDDIR)

# Generate dependencies automatically
depend: $(SOURCES)
	$(CXX) $(CXXFLAGS) -MM $^ > .depend

-include .depend

.PHONY: all debug release clean depend
```

**Key Advanced Features:**

**Automatic Variables:**
- `$@`: The target name
- `$<`: First dependency
- `$^`: All dependencies
- `$?`: Dependencies newer than target

**Pattern Rules:** `$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp`
- Matches any `.cpp` file in `src/` to corresponding `.o` in `build/`
- Eliminates need to write rule for each source file

**Order-Only Prerequisites:** `| $(BUILDDIR)`
- Ensures directory exists but doesn't trigger rebuilds based on directory timestamp
- Prevents unnecessary rebuilds when directory is touched

**Dependency Generation:** `-MM` flag
- Compiler generates list of header dependencies
- Automatically rebuilds when any included header changes

### VSCode Integration with Make

**tasks.json for Make:**
```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Make Debug",
            "type": "shell",
            "command": "make",
            "args": ["debug"],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "problemMatcher": ["$gcc"],
            "presentation": {
                "echo": true,
                "reveal": "always",
                "focus": false,
                "panel": "shared"
            }
        },
        {
            "label": "Make Release", 
            "type": "shell",
            "command": "make",
            "args": ["release"],
            "group": "build",
            "problemMatcher": ["$gcc"]
        },
        {
            "label": "Make Clean",
            "type": "shell",
            "command": "make",
            "args": ["clean"],
            "group": "build"
        },
        {
            "label": "Make Install",
            "type": "shell",
            "command": "make",
            "args": ["install"],
            "group": "build",
            "options": {
                "env": {
                    "DESTDIR": "${workspaceFolder}/install"
                }
            }
        }
    ]
}
```

**Benefits of Make Integration:**
- **Separation of concerns**: Build logic in Makefile, VSCode just calls it
- **Command line compatibility**: Same build works in terminal and VSCode
- **Team consistency**: Everyone uses same build system
- **Flexibility**: Easy to add new targets without changing VSCode config

### CMake: The Modern Solution

**Why CMake Exists:**
- **Cross-platform**: Same CMakeLists.txt works on Linux/Windows/macOS
- **IDE integration**: Generates project files for any IDE
- **Library discovery**: Automatically finds system libraries
- **Modern C++**: Better support for C++17/20 features and modules
- **Package management**: Integration with package managers like Conan, vcpkg

**Basic CMake Structure:**
```cmake
cmake_minimum_required(VERSION 3.16)
project(Calculator VERSION 1.0.0 LANGUAGES CXX)

# Global settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find external libraries
find_package(Threads REQUIRED)
find_package(Boost 1.70 REQUIRED COMPONENTS system filesystem)

# Create executable target
add_executable(calculator
    src/main.cpp
    src/calculator.cpp
    src/math_utils.cpp
)

# Target-specific settings
target_include_directories(calculator PRIVATE include)
target_compile_features(calculator PRIVATE cxx_std_17)
target_compile_options(calculator PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra>
)

# Link libraries
target_link_libraries(calculator PRIVATE 
    Threads::Threads 
    Boost::system 
    Boost::filesystem
)
```

**Modern CMake Philosophy: Targets, Not Variables**

**Old CMake (avoid):**
```cmake
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
include_directories(include)
link_directories(/usr/local/lib)
add_executable(calculator src/main.cpp)
target_link_libraries(calculator boost_system)
```

**Modern CMake (preferred):**
```cmake
add_executable(calculator src/main.cpp)
target_include_directories(calculator PRIVATE include)
target_compile_options(calculator PRIVATE -Wall -Wextra)
target_link_libraries(calculator PRIVATE Boost::system)
```

**Why Target-Based is Better:**
- **Encapsulation**: Each target has its own settings
- **Transitive properties**: Dependencies automatically propagate
- **No global pollution**: Settings don't affect unrelated targets
- **IDE integration**: Better project structure in generated IDE files

### Advanced CMake Features

**Library Creation:**
```cmake
# Create library target
add_library(math_utils
    src/calculator.cpp
    src/math_utils.cpp
)

# Library's public interface
target_include_directories(math_utils PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Library's compile features
target_compile_features(math_utils PUBLIC cxx_std_17)

# Create executable that uses library
add_executable(calculator src/main.cpp)
target_link_libraries(calculator PRIVATE math_utils)
```

**PUBLIC/PRIVATE/INTERFACE Explained:**
- **PRIVATE**: Only this target uses it
- **PUBLIC**: This target and anyone linking to it uses it
- **INTERFACE**: Only targets linking to this use it

**Generator Expressions:**
```cmake
# Conditional compilation flags
target_compile_options(calculator PRIVATE
    $<$<CONFIG:Debug>:-g -O0>
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Weverything>
)

# Conditional linking
target_link_libraries(calculator PRIVATE
    $<$<PLATFORM_ID:Linux>:dl>
    $<$<PLATFORM_ID:Windows>:ws2_32>
)
```

**Build Types:**
```bash
# Configure for Debug
cmake -B build-debug -DCMAKE_BUILD_TYPE=Debug

# Configure for Release  
cmake -B build-release -DCMAKE_BUILD_TYPE=Release

# Configure with custom flags
cmake -B build-custom -DCMAKE_CXX_FLAGS="-march=native -O3"

# Build
cmake --build build-debug
cmake --build build-release --config Release
```

### CMake Integration with VSCode

**Install CMake Tools Extension:**
```bash
code --install-extension ms-vscode.cmake-tools
```

**VSCode Settings for CMake:**
```json
{
    "cmake.buildDirectory": "${workspaceFolder}/build/${buildType}",
    "cmake.generator": "Unix Makefiles",
    "cmake.configureOnOpen": true,
    "cmake.buildBeforeRun": true,
    "cmake.saveBeforeBuild": true,
    "cmake.clearOutputBeforeBuild": true,
    "cmake.copyCompileCommands": "${workspaceFolder}/compile_commands.json"
}
```

**What CMake Tools Provides:**
- **Configure Command**: `CMake: Configure` runs `cmake -B build`
- **Build Command**: `CMake: Build` runs `cmake --build build`
- **Target Selection**: Choose which executable to build/debug
- **Variant Selection**: Choose Debug/Release/Custom configurations
- **IntelliSense Integration**: Automatically configures `c_cpp_properties.json`

**CMake Presets (CMakePresets.json):**
```json
{
    "version": 3,
    "configurePresets": [
        {
            "name": "debug",
            "displayName": "Debug Configuration",
            "generator": "Unix Makefiles",
            "binaryDir": "${sourceDir}/build/debug",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Debug",
                "CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
            }
        },
        {
            "name": "release",
            "displayName": "Release Configuration", 
            "generator": "Unix Makefiles",
            "binaryDir": "${sourceDir}/build/release",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Release",
                "CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
            }
        }
    ],
    "buildPresets": [
        {
            "name": "debug",
            "configurePreset": "debug"
        },
        {
            "name": "release", 
            "configurePreset": "release"
        }
    ]
}
```

**Benefits of CMake Presets:**
- **Standardization**: Same build configurations across team
- **Tool integration**: Works with VSCode, CLion, Visual Studio
- **Version control**: Presets are committed to repository
- **Multiple configurations**: Easy switching between Debug/Release/etc.

---

## Complete Real-World Project Setup {#complete-project-setup}

### Project Structure Design

```
scientific-calculator/
├── .vscode/                    # VSCode configuration
│   ├── tasks.json
│   ├── launch.json
│   ├── c_cpp_properties.json
│   └── settings.json
├── src/                        # Implementation files
│   ├── main.cpp
│   ├── calculator/
│   │   ├── basic_calculator.cpp
│   │   ├── scientific_calculator.cpp
│   │   └── expression_parser.cpp
│   └── utils/
│       ├── math_utils.cpp
│       └── string_utils.cpp
├── include/                    # Public header files
│   ├── calculator/
│   │   ├── basic_calculator.h
│   │   ├── scientific_calculator.h
│   │   └── expression_parser.h
│   └── utils/
│       ├── math_utils.h
│       └── string_utils.h
├── tests/                      # Unit tests
│   ├── test_basic_calculator.cpp
│   ├── test_scientific_calculator.cpp
│   └── test_math_utils.cpp
├── docs/                       # Documentation
│   ├── README.md
│   └── API.md
├── build/                      # Build artifacts (in .gitignore)
│   ├── debug/
│   ├── release/
│   └── tests/
├── external/                   # Third-party libraries
│   └── catch2/                 # Testing framework
├── CMakeLists.txt             # CMake build configuration
├── Makefile                   # Alternative Make build
├── compile_flags.txt          # clangd configuration
├── .gitignore
└── README.md
```

**Why This Structure:**
- **Separation**: Clear distinction between interface (include/) and implementation (src/)
- **Modularity**: Related functionality grouped together
- **Scalability**: Easy to add new modules without reorganization
- **Tool compatibility**: Works well with CMake, Make, and IDE generators
- **Version control**: Build artifacts excluded, source organization clear

### Complete Source Code Implementation

**include/calculator/basic_calculator.h:**
```cpp
#pragma once
#include <stdexcept>

namespace calc {

class BasicCalculator {
public:
    // Basic arithmetic operations
    double add(double a, double b) const;
    double subtract(double a, double b) const;
    double multiply(double a, double b) const;
    double divide(double a, double b) const;
    
    // Memory operations
    void memoryStore(double value);
    double memoryRecall() const;
    void memoryClear();
    
    // Utility
    void clear();
    double getLastResult() const;

private:
    double memory_ = 0.0;
    double last_result_ = 0.0;
    
    void updateLastResult(double result);
};

// Custom exception for calculator errors
class CalculatorError : public std::runtime_error {
public:
    explicit CalculatorError(const std::string& message) 
        : std::runtime_error(message) {}
};

} // namespace calc
```

**src/calculator/basic_calculator.cpp:**
```cpp
#include "calculator/basic_calculator.h"
#include "utils/math_utils.h"
#include <cmath>

namespace calc {

double BasicCalculator::add(double a, double b) const {
    double result = a + b;
    
    // Check for overflow
    if (std::isinf(result)) {
        throw CalculatorError("Addition overflow");
    }
    
    const_cast<BasicCalculator*>(this)->updateLastResult(result);
    return result;
}

double BasicCalculator::subtract(double a, double b) const {
    double result = a - b;
    
    if (std::isinf(result)) {
        throw CalculatorError("Subtraction overflow");
    }
    
    const_cast<BasicCalculator*>(this)->updateLastResult(result);
    return result;
}

double BasicCalculator::multiply(double a, double b) const {
    double result = a * b;
    
    if (std::isinf(result)) {
        throw CalculatorError("Multiplication overflow");
    }
    
    const_cast<BasicCalculator*>(this)->updateLastResult(result);
    return result;
}

double BasicCalculator::divide(double a, double b) const {
    if (utils::isZero(b)) {
        throw CalculatorError("Division by zero");
    }
    
    double result = a / b;
    const_cast<BasicCalculator*>(this)->updateLastResult(result);
    return result;
}

void BasicCalculator::memoryStore(double value) {
    memory_ = value;
}

double BasicCalculator::memoryRecall() const {
    return memory_;
}

void BasicCalculator::memoryClear() {
    memory_ = 0.0;
}

void BasicCalculator::clear() {
    last_result_ = 0.0;
}

double BasicCalculator::getLastResult() const {
    return last_result_;
}

void BasicCalculator::updateLastResult(double result) {
    last_result_ = result;
}

} // namespace calc
```

**include/utils/math_utils.h:**
```cpp
#pragma once
#include <cmath>

namespace calc::utils {

// Floating point comparison
bool isZero(double value, double epsilon = 1e-9);
bool areEqual(double a, double b, double epsilon = 1e-9);

// Mathematical functions
double factorial(int n);
double power(double base, int exponent);
double degreeToRadian(double degrees);
double radianToDegree(double radians);

// Validation
bool isFinite(double value);
bool isValidForLog(double value);
bool isValidForSqrt(double value);

} // namespace calc::utils
```

**src/utils/math_utils.cpp:**
```cpp
#include "utils/math_utils.h"
#include <stdexcept>
#include <cmath>

namespace calc::utils {

bool isZero(double value, double epsilon) {
    return std::abs(value) < epsilon;
}

bool areEqual(double a, double b, double epsilon) {
    return std::abs(a - b) < epsilon;
}

double factorial(int n) {
    if (n < 0) {
        throw std::invalid_argument("Factorial undefined for negative numbers");
    }
    
    if (n > 170) { // Prevent overflow for double
        throw std::overflow_error("Factorial too large for double precision");
    }
    
    if (n <= 1) return 1.0;
    
    double result = 1.0;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    
    return result;
}

double power(double base, int exponent) {
    if (exponent == 0) return 1.0;
    
    if (exponent < 0) {
        if (isZero(base)) {
            throw std::invalid_argument("Cannot raise zero to negative power");
        }
        return 1.0 / power(base, -exponent);
    }
    
    double result = 1.0;
    double current_power = base;
    
    // Fast exponentiation using binary representation
    while (exponent > 0) {
        if (exponent & 1) {
            result *= current_power;
        }
        current_power *= current_power;
        exponent >>= 1;
    }
    
    return result;
}

double degreeToRadian(double degrees) {
    return degrees * M_PI / 180.0;
}

double radianToDegree(double radians) {
    return radians * 180.0 / M_PI;
}

bool isFinite(double value) {
    return std::isfinite(value);
}

bool isValidForLog(double value) {
    return value > 0.0 && std::isfinite(value);
}

bool isValidForSqrt(double value) {
    return value >= 0.0 && std::isfinite(value);
}

} // namespace calc::utils
```

### Complete VSCode Configuration

**.vscode/tasks.json:**
```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "CMake Configure Debug",
            "type": "shell",
            "command": "cmake",
            "args": [
                "-B", "build/debug",
                "-DCMAKE_BUILD_TYPE=Debug",
                "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
            ],
            "group": "build",
            "presentation": {
                "echo": true,
                "reveal": "silent",
                "focus": false,
                "panel": "shared",
                "showReuseMessage": false,
                "clear": true
            },
            "problemMatcher": []
        },
        {
            "label": "CMake Build Debug",
            "type": "shell", 
            "command": "cmake",
            "args": ["--build", "build/debug", "--parallel", "4"],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "dependsOn": "CMake Configure Debug",
            "presentation": {
                "echo": true,
                "reveal": "always",
                "focus": false,
                "panel": "shared"
            },
            "problemMatcher": ["$gcc"]
        },
        {
            "label": "CMake Configure Release",
            "type": "shell",
            "command": "cmake", 
            "args": [
                "-B", "build/release",
                "-DCMAKE_BUILD_TYPE=Release",
                "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
            ],
            "group": "build",
            "presentation": {
                "echo": true,
                "reveal": "silent",
                "focus": false,
                "panel": "shared"
            }
        },
        {
            "label": "CMake Build Release",
            "type": "shell",
            "command": "cmake",
            "args": ["--build", "build/release", "--parallel", "4"],
            "dependsOn": "CMake Configure Release",
            "group": "build",
            "problemMatcher": ["$gcc"]
        },
        {
            "label": "Build Tests",
            "type": "shell",
            "command": "cmake",
            "args": ["--build", "build/debug", "--target", "tests", "--parallel", "4"],
            "dependsOn": "CMake Configure Debug",
            "group": "test",
            "problemMatcher": ["$gcc"]
        },
        {
            "label": "Run Tests",
            "type": "shell",
            "command": "./build/debug/tests/calculator_tests",
            "dependsOn": "Build Tests",
            "group": "test",
            "presentation": {
                "echo": true,
                "reveal": "always",
                "focus": false,
                "panel": "dedicated"
            }
        },
        {
            "label": "Clean Build",
            "type": "shell",
            "command": "rm",
            "args": ["-rf", "build"],
            "group": "build"
        },
        {
            "label": "Format Code",
            "type": "shell",
            "command": "clang-format",
            "args": ["-i", "src/**/*.cpp", "include/**/*.h"],
            "group": "build"
        }
    ]
}
```

**.vscode/launch.json:**
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug Calculator",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/debug/calculator",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [
                {
                    "name": "CALCULATOR_DEBUG",
                    "value": "1"
                }
            ],
            "externalConsole": false,
            "MIMode": "gdb",
            "miDebuggerPath": "/usr/bin/gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                },
                {
                    "description": "Set Disassembly Flavor to Intel",
                    "text": "-gdb-set disassembly-flavor intel",
                    "ignoreFailures": true
                }
            ],
            "preLaunchTask": "CMake Build Debug",
            "internalConsoleOptions": "openOnSessionStart"
        },
        {
            "name": "Debug Tests",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/debug/tests/calculator_tests",
            "args": ["--verbose"],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "miDebuggerPath": "/usr/bin/gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ],
            "preLaunchTask": "Build Tests"
        },
        {
            "name": "Attach to Process",
            "type": "cppdbg",
            "request": "attach",
            "program": "${workspaceFolder}/build/debug/calculator",
            "processId": "${command:pickProcess}",
            "MIMode": "gdb",
            "miDebuggerPath": "/usr/bin/gdb"
        }
    ]
}
```

**.vscode/c_cpp_properties.json:**
```json
{
    "configurations": [
        {
            "name": "Linux Debug",
            "includePath": [
                "${workspaceFolder}/include",
                "${workspaceFolder}/src", 
                "${workspaceFolder}/external/**",
                "/usr/include/c++/**",
                "/usr/local/include"
            ],
            "defines": [
                "DEBUG=1",
                "_GNU_SOURCE"
            ],
            "compilerPath": "/usr/bin/g++",
            "cStandard": "c17",
            "cppStandard": "c++17",
            "intelliSenseMode": "linux-gcc-x64",
            "compileCommands": "${workspaceFolder}/build/debug/compile_commands.json",
            "configurationProvider": "ms-vscode.cmake-tools"
        },
        {
            "name": "Linux Release",
            "includePath": [
                "${workspaceFolder}/include",
                "${workspaceFolder}/src",
                "${workspaceFolder}/external/**", 
                "/usr/include/c++/**",
                "/usr/local/include"
            ],
            "defines": [
                "NDEBUG=1",
                "_GNU_SOURCE"
            ],
            "compilerPath": "/usr/bin/g++",
            "cStandard": "c17",
            "cppStandard": "c++17",
            "intelliSenseMode": "linux-gcc-x64",
            "compileCommands": "${workspaceFolder}/build/release/compile_commands.json"
        }
    ],
    "version": 4
}
```

### Advanced CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(ScientificCalculator 
    VERSION 1.0.0 
    DESCRIPTION "A scientific calculator with expression parsing"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type specific settings
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall -Wextra -Wpedantic
        -Wconversion -Wsign-conversion
        -Wunused -Wshadow
        $<$<CONFIG:Debug>:-g -O0 -DDEBUG>
        $<$<CONFIG:Release>:-O3 -DNDEBUG -march=native>
    )
endif()

# Find required packages
find_package(Threads REQUIRED)

# Create math utilities library
add_library(math_utils
    src/utils/math_utils.cpp
    src/utils/string_utils.cpp
)

target_include_directories(math_utils PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(math_utils PUBLIC cxx_std_17)

# Create calculator library
add_library(calculator_lib
    src/calculator/basic_calculator.cpp
    src/calculator/scientific_calculator.cpp
    src/calculator/expression_parser.cpp
)

target_include_directories(calculator_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(calculator_lib PUBLIC math_utils)
target_compile_features(calculator_lib PUBLIC cxx_std_17)

# Create main executable
add_executable(calculator src/main.cpp)
target_link_libraries(calculator PRIVATE calculator_lib Threads::Threads)

# Enable testing
enable_testing()
include(CTest)

# Add test executable
if(BUILD_TESTING)
    add_subdirectory(external/catch2)
    
    add_executable(calculator_tests
        tests/test_basic_calculator.cpp
        tests/test_scientific_calculator.cpp
        tests/test_math_utils.cpp
    )
    
    target_link_libraries(calculator_tests PRIVATE 
        calculator_lib
        Catch2::Catch2WithMain
    )
    
    # Discover tests for CTest
    include(Catch)
    catch_discover_tests(calculator_tests)
endif()

# Installation rules
install(TARGETS calculator calculator_lib math_utils
    EXPORT ScientificCalculatorTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
)

install(DIRECTORY include/ DESTINATION include)

# Export configuration
install(EXPORT ScientificCalculatorTargets
    FILE ScientificCalculatorTargets.cmake
    NAMESPACE ScientificCalculator::
    DESTINATION lib/cmake/ScientificCalculator
)


# Generate compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
```

---

## Advanced Debugging and Development Workflow {#debugging-workflow}

### Understanding Debug Information

**What `-g` Actually Does:**
When you compile with `-g`, the compiler embeds additional information in the executable:

```bash
# Without debug info
g++ -O2 calculator.cpp -o calculator_release
strip calculator_release  # Remove any remaining symbols
gdb calculator_release

# With debug info
g++ -g -O0 calculator.cpp -o calculator_debug
gdb calculator_debug
```

**Debug Information Contains:**
- **Line number tables**: Map machine instructions to source lines
- **Variable information**: Names, types, and memory locations of variables
- **Function metadata**: Parameter types, return types, scope information
- **Type definitions**: Struct/class layouts, inheritance hierarchies
- **Source file paths**: Where to find the original source code

**Debug Information Levels:**
```bash
-g0  # No debug information
-g1  # Minimal debug info (function names, external variables)
-g   # Default debug info (same as -g2)
-g2  # Full debug info (local variables, line numbers)
-g3  # Maximum debug info (includes macro definitions)
```

### Effective Debugging Strategies

**1. Strategic Breakpoint Placement**

**Function Entry Points:**
```cpp
double Calculator::divide(double a, double b) {
    // Set breakpoint here to inspect parameters
    if (utils::isZero(b)) {
        // Set breakpoint here to catch division by zero
        throw CalculatorError("Division by zero");
    }
    
    double result = a / b;
    // Set breakpoint here to inspect result
    return result;
}
```

**Loop Debugging:**
```cpp
for (int i = 0; i < size; ++i) {
    // Conditional breakpoint: i == size - 1
    // Log point: "Processing element {i}: {array[i]}"
    process(array[i]);
}
```

**2. Advanced Breakpoint Types**

**Conditional Breakpoints:**
- Right-click line number → "Add Conditional Breakpoint"
- Expression examples:
  - `ptr != nullptr`
  - `i > 100`
  - `result.empty()`
  - `strcmp(name, "target") == 0`

**Hit Count Breakpoints:**
- Break only after N hits: `Hit Count: 10`
- Break every N hits: `Hit Count: % 5 == 0`

**Logpoints (Non-breaking breakpoints):**
- Right-click line number → "Add Logpoint"
- Message: `"Variable x = {x}, y = {y}"`
- Prints to Debug Console without stopping execution

**3. Data Inspection Techniques**

**Watch Expressions:**
- Add complex expressions to Watch panel
- Examples:
  - `this->memory_`
  - `vector.size()`
  - `*pointer_variable`
  - `static_cast<int>(enum_value)`

**Memory View:**
- View → Command Palette → "Debug: Open Memory View"
- Inspect raw memory around pointers
- Useful for buffer overruns, memory corruption

**Call Stack Navigation:**
- Each frame shows function parameters and local variables
- Click any frame to inspect state at that call level
- Essential for understanding complex call chains

### GDB Integration and Commands

**Custom GDB Commands in launch.json:**
```json
{
    "setupCommands": [
        {
            "description": "Enable pretty-printing for STL containers",
            "text": "-enable-pretty-printing",
            "ignoreFailures": true
        },
        {
            "description": "Print arrays in full",
            "text": "-gdb-set print elements 0"
        },
        {
            "description": "Print strings in full", 
            "text": "-gdb-set print characters 0"
        },
        {
            "description": "Show actual types, not typedefs",
            "text": "-gdb-set print object on"
        },
        {
            "description": "Skip boring system library code",
            "text": "-gdb-set skip-solib-deps on"
        },
        {
            "description": "Set custom breakpoint on exceptions",
            "text": "-break-insert -f __cxa_throw"
        }
    ]
}
```

**Direct GDB Commands (Debug Console):**
```bash
# Print variable in different formats
-exec p variable_name
-exec p/x variable_name    # Hexadecimal
-exec p/d variable_name    # Decimal
-exec p/c variable_name    # Character
-exec p/s variable_name    # String

# Examine memory
-exec x/10i $pc           # 10 instructions from current location
-exec x/16xb pointer      # 16 bytes in hex
-exec x/4xw address       # 4 words in hex

# Backtrace with more info
-exec bt full             # Full backtrace with local variables
-exec thread apply all bt # Backtrace for all threads

# Advanced breakpoints
-exec break function_name if condition
-exec break file.cpp:123 if variable > 10
```

### Memory Debugging with Valgrind

**Installation and Basic Usage:**
```bash
# Install valgrind
sudo apt install valgrind

# Compile with debug info (required for meaningful output)
g++ -g -O0 calculator.cpp -o calculator_debug

# Run with memory checking
valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all \
         --track-origins=yes ./calculator_debug
```

**Common Memory Issues Detected:**
- **Invalid memory access**: Reading/writing outside allocated memory
- **Memory leaks**: Allocated memory never freed
- **Double free**: Freeing the same memory twice
- **Use after free**: Accessing freed memory

**VSCode Integration with Valgrind:**
```json
// launch.json configuration for Valgrind
{
    "name": "Debug with Valgrind",
    "type": "cppdbg", 
    "request": "launch",
    "program": "valgrind",
    "args": [
        "--tool=memcheck",
        "--leak-check=full",
        "--show-leak-kinds=all",
        "--track-origins=yes",
        "--verbose",
        "${workspaceFolder}/build/debug/calculator"
    ],
    "stopAtEntry": false,
    "cwd": "${workspaceFolder}",
    "externalConsole": true,
    "preLaunchTask": "CMake Build Debug"
}
```

### Performance Profiling

**Using gprof (GNU Profiler):**
```bash
# Compile with profiling
g++ -pg -O2 calculator.cpp -o calculator_prof

# Run program (generates gmon.out)
./calculator_prof

# Generate profile report
gprof calculator_prof gmon.out > profile.txt
```

**Using perf (Linux Performance Tools):**
```bash
# Install perf
sudo apt install linux-tools-$(uname -r)

# Record performance data
perf record -g ./calculator_debug

# View performance report
perf report

# Top CPU consumers
perf top
```

### Static Analysis Integration

**Clang Static Analyzer:**
```bash
# Install clang tools
sudo apt install clang-tools

# Run static analysis
scan-build g++ -std=c++17 -Iinclude src/*.cpp -o calculator

# Or with clang directly
clang++ --analyze -std=c++17 -Iinclude src/*.cpp
```

**cppcheck Integration:**
```bash
# Install cppcheck
sudo apt install cppcheck

# Run comprehensive check
cppcheck --enable=all --std=c++17 --language=c++ \
         --platform=unix64 --check-config src/
```

**VSCode Task for Static Analysis:**
```json
{
    "label": "Run Static Analysis",
    "type": "shell",
    "command": "cppcheck",
    "args": [
        "--enable=all",
        "--std=c++17", 
        "--language=c++",
        "--platform=unix64",
        "--xml",
        "--xml-version=2",
        "src/"
    ],
    "group": "test",
    "presentation": {
        "echo": true,
        "reveal": "always",
        "focus": false,
        "panel": "dedicated"
    },
    "problemMatcher": {
        "owner": "cppcheck",
        "fileLocation": "absolute",
        "pattern": [
            {
                "regexp": "^.*<error id=\"(.*)\" severity=\"(.*)\" msg=\"(.*)\" verbose=\".*\" cwe=\".*\">$",
                "message": 3,
                "code": 1,
                "severity": 2
            },
            {
                "regexp": "^.*<location file=\"(.*)\" line=\"(\\d+)\".*/>$",
                "file": 1,
                "line": 2
            }
        ]
    }
}
```

### Multi-threaded Debugging

**Thread-aware Debugging Configuration:**
```json
{
    "name": "Debug Multi-threaded",
    "type": "cppdbg",
    "request": "launch", 
    "program": "${workspaceFolder}/build/debug/calculator",
    "args": ["--threads", "4"],
    "stopAtEntry": false,
    "cwd": "${workspaceFolder}",
    "environment": [],
    "externalConsole": false,
    "MIMode": "gdb",
    "setupCommands": [
        {
            "description": "Enable pretty-printing",
            "text": "-enable-pretty-printing"
        },
        {
            "description": "Set scheduler-locking off",
            "text": "-gdb-set scheduler-locking off"
        },
        {
            "description": "Set non-stop mode",
            "text": "-gdb-set non-stop on"
        }
    ],
    "preLaunchTask": "CMake Build Debug"
}
```

**Thread Debugging Commands:**
```bash
# List all threads
-exec info threads

# Switch to specific thread
-exec thread 3

# Apply command to all threads
-exec thread apply all bt

# Set breakpoint for specific thread only
-exec break main.cpp:42 thread 2

# Continue only current thread
-exec continue &
```

### Development Workflow Best Practices

**1. Test-Driven Development Integration**

**VSCode Task for TDD:**
```json
{
    "label": "TDD Cycle",
    "dependsOrder": "sequence",
    "dependsOn": [
        "Build Tests",
        "Run Tests",
        "CMake Build Debug"
    ]
}
```

**Continuous Testing:**
```json
{
    "label": "Watch Tests",
    "type": "shell",
    "command": "watch",
    "args": ["-n", "2", "make", "test"],
    "isBackground": true,
    "presentation": {
        "echo": false,
        "reveal": "always",
        "focus": false,
        "panel": "dedicated"
    }
}
```

**2. Code Quality Automation**

**Pre-commit Formatting:**
```json
{
    "label": "Format and Build",
    "dependsOrder": "sequence", 
    "dependsOn": [
        "Format Code",
        "CMake Build Debug",
        "Run Tests"
    ]
}
```

**3. Debugging Workflow**

**Typical Debugging Session:**
1. **Reproduce the bug**: Create minimal test case
2. **Set strategic breakpoints**: Entry points, error conditions
3. **Inspect data flow**: Watch variables change through execution
4. **Test hypotheses**: Modify values in debugger, continue execution
5. **Verify fix**: Run tests, check edge cases

**Debug Configuration Switching:**
```json
// Multiple launch configurations for different scenarios
{
    "configurations": [
        {
            "name": "Debug Main",
            "program": "${workspaceFolder}/build/debug/calculator"
        },
        {
            "name": "Debug with Args",
            "program": "${workspaceFolder}/build/debug/calculator",
            "args": ["--verbose", "--input", "test.txt"]
        },
        {
            "name": "Debug Tests",
            "program": "${workspaceFolder}/build/debug/tests/calculator_tests"
        },
        {
            "name": "Debug Specific Test",
            "program": "${workspaceFolder}/build/debug/tests/calculator_tests",
            "args": ["[calculator]"]
        }
    ]
}
```

### Troubleshooting Common Issues

**1. IntelliSense Problems**

**Symptoms:**
- Red squiggly lines under correct code
- Missing code completion
- "Cannot find header file" errors

**Solutions:**
```bash
# Regenerate compile_commands.json
cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
ln -sf build/compile_commands.json .

# Reset IntelliSense
# Ctrl+Shift+P → "C/C++: Reset IntelliSense Database"

# Check configuration
# Ctrl+Shift+P → "C/C++: Edit Configurations (UI)"
```

**2. Build System Issues**

**CMake Cache Problems:**
```bash
# Clear CMake cache
rm -rf build/
cmake -B build -DCMAKE_BUILD_TYPE=Debug

# Or force reconfigure
cmake -B build --fresh
```

**3. Debugging Connection Issues**

**GDB Path Problems:**
```json
// Specify exact GDB path in launch.json
{
    "miDebuggerPath": "/usr/bin/gdb",
    // Or find dynamically
    "miDebuggerPath": "${command:cmake.launchTargetPath}"
}
```

**Permission Issues:**
```bash
# Check executable permissions
chmod +x build/debug/calculator

# Check if executable exists
ls -la build/debug/calculator
```

**4. Library Linking Issues**

**Runtime Library Path:**
```bash
# Check library dependencies
ldd build/debug/calculator

# Set runtime library path
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

# Or in launch.json
{
    "environment": [
        {
            "name": "LD_LIBRARY_PATH", 
            "value": "/usr/local/lib:${env:LD_LIBRARY_PATH}"
        }
    ]
}
```


        