<div align="center">

# Maximum Common Induced Subgraph (MCIS) Algorithm Testing Framework

_A Senior Thesis Project in Electrical Engineering and Computer Science_\
_Yale University • CPSC 4900_

[![C++](https://img.shields.io/badge/C++-20-blue.svg?style=flat&logo=cplusplus)](https://en.cppreference.com/w/cpp/20)
[![CMake](https://img.shields.io/badge/CMake-3.22+-green.svg?style=flat&logo=cmake)](https://cmake.org/)
[![GTest](https://img.shields.io/badge/Testing-Google%20Test-red.svg?style=flat)](https://github.com/google/googletest)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg?style=flat)](LICENSE)
[![GitHub release (latest
SemVer)](https://img.shields.io/github/v/release/sebaraj/MCIS-BCI?include_prereleases?sort=semver?display_name=tag)](https://github.com/sebaraj/MCIS-BCI/releases/latest)\
[![cpp-linter](https://github.com/sebaraj/MCIS-BCI/actions/workflows/cpp-linter.yml/badge.svg)](https://github.com/sebaraj/MCIS-BCI/actions/workflows/cpp-linter.yml)
[![Build and Test](https://github.com/sebaraj/MCIS-BCI/actions/workflows/build-and-test.yml/badge.svg)](https://github.com/sebaraj/MCIS-BCI/actions/workflows/build-and-test.yml)

</div>

---

## Overview

This library provides a comprehensive C++ testing framework for evaluating Maximum Common Induced
Subgraph (MCIS) algorithms specifically designed for Brain-Computer Interface (BCI) accelerator
workloads.

### Why This Matters

Brain-Computer Interfaces represent the cutting edge of medical technology, offering hope for
restoring sensory and motor functions to individuals with severe neurological disorders. However,
the practical implementation of fully implanted BCIs is severely constrained by ultra-low power
requirements, since implanted devices must minimize heat dissipation to prevent damage to surrounding
neural tissue.

Many BCI workloads (neural signal processing, data interpretation) can be modeled as Computational
Directed Acyclic Graphs (CDAGs). Rather than designing separate hardware accelerators for each
algorithm, this research aims to validate the most efficient algorithms to identify the largest computational structures shared among different BCI
algorithms, enabling the design of a single, minimal, canonical hardware accelerator that
maximizes hardware reuse while minimizing power consumption.

### The Challenge

The Maximum Common Induced Subgraph (MCIS) problem is NP-hard, making brute-force approaches computationally infeasible for complex CDAGs representing realistic BCI workloads. However, CDAGs derived from signal processing algorithms often possess specific structural properties that may be exploitable for developing efficient algorithms.

---

## Architecture

This framework provides:

- **Modular Graph Representation**: Robust data structures for representing and manipulating CDAGs
- **Algorithm Interface**: Clean interfaces for integrating various MCIS-finding algorithms
- **Comprehensive Benchmarking**: Performance measurement tools for runtime, memory usage, and subgraph quality
- **Testing Infrastructure**: Google Test-based validation framework
- **Dataset Generation**: Tools for creating BCI-relevant CDAG datasets (DWT, FFT, MVM algorithms)

---

## Quick Start

### Prerequisites

Ensure you have the following dependencies installed:

- **C++ Compiler**: Supporting C++20 standard
  - GCC 10+ / Clang 12+ / MSVC 2019+
- **CMake**: Version 3.22.1 or higher
- **Conan**: C++ package manager
- **Ninja**: Build system
- **Graphviz (dot)**: For graph visualization (optional)
- **Doxygen**: For documentation generation (optional)

```bash
# macOS
brew install cmake conan ninja graphviz doxygen

# Ubuntu/Debian
sudo apt update
sudo apt install cmake graphviz ninja-build python3 python3-pip doxygen
pip install conan

# Windows (using Chocolatey)
choco install cmake conan ninja graphviz doxygen.install
```

### Building the Project

```bash
# Clone the repository
git clone https://github.com/sebaraj/MCIS-BCI.git
cd MCIS-BCI

# Create directories for graph outputs (optional)
mkdir dot && mkdir diagrams

# Create build directory
mkdir build && cd build

# Install dependencies with Conan
conan profile detect
conan install .. --build=missing -of . -v

# Configure with CMake
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DOpenMP_ROOT=.

# Build the project
cmake --build .

# Run tests
ctest --verbose
```

---

## Usage

### Basic Graph Operations

```cpp
#include "mcis/graph.h"
```

### Running MCIS Algorithms

```cpp
#include "mcis/graph.h"
#include "mcis/mcis_algorithms.h"

```

---

## MCIS Algorithms

### Current Implementations

(proposed, still need to implement)

- **Brute Force Solver**: Exact MCIS using maximum clique reduction

### Planned Implementations

---

## Workload CDAG Generators

(proposed, still need to implement)\
The framework includes generators for BCI-relevant computational graphs, such as

- **Discrete Wavelet Transform (DWT)**: Multi-resolution signal analysis
- **Fast Fourier Transform (FFT)**: Frequency domain analysis
- **Matrix-Vector Multiplication (MVM)**: Linear algebraic operations

---

## Testing

```bash
# Tests are build alongside the main project
cd build
cmake --build .

# Run all tests
ctest --verbose --parallel 4

# Specific test categories
ctest -R NodeTest
ctest -R GraphTest
ctest -R MVMTest
ctest -R DWTTest
ctest -R FFTTest
ctest -R AlgorithmTest
ctest -R BenchmarkTest

# Run a specific test executable directly
ctest -R MVMTest.MVM2x2GraphCreation --verbose

# Test and generate a png of the generated CDAG for all non-NodeTests
# dot (.gv) files are saved in the `./dot/` directory
# diagrams (.png) files are saved in the `./diagrams/` directory
GENERATE_DIAGRAMS=1 ctest -R MVMTest -R MVMTest.MVM2x2GraphCreation
GENERATE_DIAGRAMS=1 ctest -R DWTTest
```

---

## Performance Analysis

(proposed, still need to implement)\
Generate comprehensive performance reports with various metrics:

```bash
# Runtime analysis
./build/bin/benchmark --metric runtime --graphs mvm5x5,mvm20x10 --algorithms all

# Memory usage profiling
./build/bin/benchmark --metric memory --profile detailed
```

---

## Contributing

This is a senior thesis project, but suggestions and discussions are welcome. Please feel free to contact me at bryan.sebaraj@yale.edu,
or open up an issue or feature request on GitHub.

---
