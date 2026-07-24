# ML Inference Project

A simple ML inference framework using C++ and GitHub Actions for CI/CD.

## Build Requirements

- C++ compiler (tested with GCC)
- CMake 3.10+

## Build Instructions

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Testing

Running tests:
```bash
cd build
ctest
```

## Project Structure

- `src/` - Source code
- `include/` - Header files
- `models/` - Model configuration files

## Models

- **Sort Model**: Sorts an array of integers
- **Average Model**: Calculates the average of an array of integers
- **Sum Model**: Calculates the sum of an array of integers

## ML Framework

This project implements a simple ML model framework with:
- Model factory for loading and managing models
- Thread pool for parallel processing
- Serializable models for persistence