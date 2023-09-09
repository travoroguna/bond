# bond programming language

## What is bond?

bond is a dynamically typed language. It is designed to be simple to use

## building

bond is written in C++23 and uses CMake as its build system. To build bond, you need to have CMake installed. Then, run
the following commands:
bond has been tested on windows using MSVC(Visual Studio 17 2022) and on linux using GCC
make sure cmake and ninja are installed for your platform

```bash
git clone https://github.com/travoroguna/bond.git
cd bond
mkdir build
cd build
```

### windows

you can use also use ninja on windows but make sure you have ninja installed
and is using developer command prompt for visual studio

```bash
cmake .. -G "Visual Studio 17 2022" -A x64  -DCMAKE_BUILD_TYPE=Release
```

or change install prefix with

```bash
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=<path to install>
```

### linux

```bash
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
```

or change install prefix with

```bash
cmake .. -G Ninja -DCMAKE_INSTALL_PREFIX=<path to install>
```

then build with

```bash
cmake --build . --config Release
````

or build and install with

```bash
cmake --build . --target install
```