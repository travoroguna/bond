# Bond Programming Language

## What is Bond?

Bond is a dynamically typed, scripting language with a C-like syntax, designed for simplicity and ease of use.

Key features include:
* Support for basic data types (integers, floats, strings).
* Mutable lists that can hold mixed data types.
* C-style functions that support variable arguments and explicit error handling via a `Result` type (`Ok`/`Err`) and the `try` keyword.
* Structs for creating custom data structures with methods and operator overloading.
* Modules for code organization and reusability.
* Destructuring assignment.
* Control flow constructs like `if/else`, `while`, and `for` loops.

Bond can be used for general scripting tasks and simple GUI development (as shown by its examples using a Nuklear binding). Its design seems to draw some inspiration from languages like Python (e.g., in its approach to some operations and features like destructuring).

## Building Bond

### Prerequisites

**Essential:**
* A C++23 compliant compiler (e.g., GCC, Clang, MSVC).
* CMake (version 3.22 or newer).
* Git (for cloning and dependency fetching via CPM).

**Recommended:**
* Ninja build system.

**Platform-specific for GUI capabilities (Bond's `bnk` module using GLFW):**
* **Linux:** You will need development libraries for X11 and OpenGL. For example, on Debian/Ubuntu:
    ```bash
    sudo apt-get install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl-dev
    ```
    For other distributions, please install the equivalent packages.
* **Windows:** If you are using MSVC (as per the instructions below), these dependencies are generally handled. For MinGW (not officially covered in these instructions), you might need to install GLFW manually.
* **macOS:** macOS is not yet officially tested or supported. Users attempting to build would need Xcode Command Line Tools. Ensure GLFW can be built or found (e.g., via Homebrew: `brew install glfw`).

### Build Steps

First, clone the repository and navigate into the project directory:
```bash
git clone https://github.com/travoroguna/bond.git
cd bond
mkdir build
cd build
```

Now, configure the project using CMake and build it.

**Windows:**
To configure with MSVC (Visual Studio 17 2022):
```bash
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
```
To specify a custom install prefix with MSVC:
```bash
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="C:/my/install/location"
```
Alternatively, you can use Ninja on Windows. Ensure Ninja is installed and you are using a Developer Command Prompt for Visual Studio, then use:
```bash
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
```
To specify a custom install prefix with Ninja on Windows:
```bash
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="C:/my/install/location"
```

**Linux:**
To configure with Ninja:
```bash
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
```
To specify a custom install prefix with Ninja on Linux:
```bash
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="/my/install/location"
```

**macOS (Experimental):**
macOS is not yet officially supported or extensively tested. However, you can try building using the same commands as Linux. Success will depend on having the necessary dependencies for GLFW installed (typically via Homebrew, e.g., `brew install glfw`).
```bash
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release 
# or cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
```
To specify a custom install prefix with Ninja on macOS:
```bash
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="/my/install/location"
# or cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="/my/install/location"
```

**General Build & Install:**

After configuring, build the project:
```bash
cmake --build . --config Release
```

To install Bond (optional):
```bash
# Installs to default system location:
# - Windows: C:/Program Files (x86)/bond (or C:/Program Files/bond if building 64-bit with a 64-bit generator)
# - Linux/macOS: /usr/local/bond (or as specified by CMAKE_INSTALL_PREFIX during configuration)
cmake --build . --target install 

# Note: The custom install prefix (-DCMAKE_INSTALL_PREFIX="<path>") 
# must be set during the CMake configuration step (e.g., `cmake .. -DCMAKE_INSTALL_PREFIX="/your/custom/path"`). 
# The install command above will then use that specified path.
```

### Running Tests

After successfully building the project:

1.  Navigate to your build directory (e.g., `build`).
2.  Run the tests using the command corresponding to your build system:

    *   If using Ninja:
        ```bash
        ninja test
        ```
    *   If using Makefiles (e.g., with "Unix Makefiles" generator):
        ```bash
        make test
        ```
    *   Alternatively, you can use the CMake generic command:
        ```bash
        ctest
        ```
        (For more detailed output on failures, you might want to use `ctest --output-on-failure`).

## Getting Started

This section will guide you through running your first Bond program.

1.  **Create your Bond file:**
    Create a new file named `hello.bd` (or use the existing one in the `examples` directory, which contains `println("Hello, World!");`) with the following content:
    ```bond
    // examples/hello.bd
    println("Hello, World!");
    ```

2.  **Run your program:**
    After building Bond (see the "Building Bond" section), you can run your script using the `bond` executable from your build directory (e.g., `build/`).
    If you are in the root directory of the project and built in a `build` subdirectory:
    ```bash
    # On Linux or macOS (if built successfully)
    ./build/bond examples/hello.bd
    # On Windows
    .\build\bond.exe examples\hello.bd
    ```
    To run a different Bond script file, replace `examples/hello.bd` with the path to your file.

    If you have installed Bond to a location in your system's PATH, you can run it directly:
    ```bash
    bond examples/hello.bd
    # Or, if your script is elsewhere:
    # bond path/to/your/script.bd
    ```

    You should see the output:
    ```
    Hello, World!
    ```

## Contributing

Contributions to Bond are welcome! Whether you're fixing a bug, proposing a new feature, or improving documentation, your help is appreciated.

Here are some ways you can contribute:

* **Reporting Bugs:** If you find a bug, please open an issue on the GitHub repository. Include as much detail as possible, such as steps to reproduce, expected behavior, and actual behavior.
* **Suggesting Enhancements:** If you have an idea for a new feature or an improvement to an existing one, feel free to open an issue to discuss it.
* **Pull Requests:** For bug fixes or new features, you can submit a pull request. Please ensure your code follows the existing style and that your changes are well-tested (if applicable).

When contributing code, please try to match the existing coding style found throughout the project. While there isn't a formal style guide yet, consistency is appreciated.

We look forward to your contributions!

## License

A specific license file for the Bond project itself (e.g., `LICENSE` or `LICENSE.md`) was not found in the root of this repository. It is recommended that a license file be added to clarify the terms under which the Bond language, its tools, and its source code can be used, modified, and distributed.

License information for third-party libraries and dependencies used by Bond can be found in the `third_party.txt` file. This file is typically generated in the build directory when Bond is compiled (as specified by the `CPACK_RESOURCE_FILE_LICENSE` variable in `CMakeLists.txt`). Users should consult this `third_party.txt` file for details on the licenses of these bundled components.