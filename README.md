# Newton

C++ infrastructure for code running on Newton cluster with optional Boost support.

## Overview

This project provides a modern C++17 infrastructure designed to run on compute clusters. It includes:

- Core library with extensible architecture
- Optional Boost library integration for filesystem operations
- CMake-based build system
- Example programs demonstrating usage
- Test suite

## Requirements

- CMake 3.14 or higher
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- Boost 1.65 or higher (optional, but recommended)

## Building

### Basic Build (without Boost)

```bash
mkdir build
cd build
cmake .. -DUSE_BOOST=OFF
make
```

### Build with Boost Support

```bash
mkdir build
cd build
cmake .. -DUSE_BOOST=ON
make
```

If Boost is not found, the build will continue without Boost support.

### Build Options

- `USE_BOOST`: Enable/disable Boost library support (default: ON)

## Running Examples

After building, you can run the example programs:

```bash
# Basic example (always available)
./examples/basic_example

# Boost example (if built with Boost support)
./examples/boost_example
```

## Running Tests

```bash
cd build
ctest
# or
make test
```

## Installation

```bash
cd build
make install
```

By default, this will install:
- Headers to `/usr/local/include/newton/`
- Library to `/usr/local/lib/`
- Examples to `/usr/local/bin/`

You can change the installation prefix:

```bash
cmake .. -DCMAKE_INSTALL_PREFIX=/path/to/install
make install
```

## Project Structure

```
newton/
├── include/newton/     # Public header files
│   ├── core.hpp        # Core library interface
│   └── boost_utils.hpp # Boost utilities (optional)
├── src/                # Source implementations
│   ├── core.cpp
│   └── boost_utils.cpp
├── examples/           # Example programs
│   ├── basic_example.cpp
│   └── boost_example.cpp
├── tests/              # Test files
│   └── test_core.cpp
└── CMakeLists.txt      # Main CMake configuration
```

## Usage in Your Code

```cpp
#include <newton/core.hpp>
#include <vector>

int main() {
    newton::Core core;
    core.initialize();
    
    std::vector<double> data = {1.0, 2.0, 3.0};
    auto result = core.processData(data);
    
    return 0;
}
```

### Using Boost Utilities

```cpp
#ifdef USE_BOOST
#include <newton/boost_utils.hpp>

// Check if file exists
bool exists = newton::boost_utils::fileExists("path/to/file");

// List files in directory
auto files = newton::boost_utils::listFiles("path/to/directory");

// Create directory
newton::boost_utils::createDirectory("path/to/new/directory");
#endif
```

## License

This project is open source and available under the MIT License.