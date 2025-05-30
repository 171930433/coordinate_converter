# Coordinate Converter

A C++ library for geographic coordinate system conversion, supporting various coordinate system transformations on the WGS84 ellipsoid.

## Features

- Support for coordinate conversion on WGS84 ellipsoid
- Conversion between Latitude-Longitude-Height (LLH) and Earth-Centered, Earth-Fixed (ECEF) coordinates
- Conversion between Latitude-Longitude-Height (LLH) and East-North-Up (ENU) coordinates
- Support for custom ellipsoid parameters

## Dependencies

- Eigen3: For matrix and vector operations
- C++11 or higher

## Installation

```bash
# Clone the repository
git clone https://github.com/171930433/coordinate_converter.git
cd coordinate_converter

# Build and test
mkdir build && cd build
cmake ..
make
```

## Usage

### Basic Usage

```cpp
#include "coordinate_converter.hpp"

using namespace coordinate_converter;

// Create a WGS84 ellipsoid instance
WGS84 wgs84;

// Define Latitude-Longitude-Height coordinates (latitude, longitude, height)
// Note: Latitude and longitude are in radians, height is in meters
Eigen::Vector3d llh{30.0 * M_PI / 180.0, 120.0 * M_PI / 180.0, 250.0};

// Convert to ECEF coordinates
Eigen::Vector3d ecef = wgs84.LLH2ECEF(llh);

// Convert back to LLH coordinates
Eigen::Vector3d llh_recovered = wgs84.ECEF2LLH(ecef);
```

### Custom Origin

```cpp
// Set the origin
Eigen::Vector3d origin{30.0 * M_PI / 180.0, 120.0 * M_PI / 180.0, 0.0};
WGS84 wgs84(origin);

// Define LLH coordinates relative to the origin
Eigen::Vector3d llh{30.005 * M_PI / 180.0, 120.005 * M_PI / 180.0, 10.0};

// Convert to ENU coordinates
Eigen::Vector3d enu = wgs84.LLH2ENU(llh);

// Convert back to LLH coordinates
Eigen::Vector3d llh_recovered = wgs84.ENU2LLH(enu);
```

### Using Literal Operators

To simplify angle-to-radian conversion, the library provides literal operators:

```cpp
// Define literal operators for angles
constexpr double operator"" _deg(long double x) { return x / 180.0 * M_PI; }
constexpr double operator"" _deg(unsigned long long x) { return x / 180.0 * M_PI; }

// Use literal operators to define coordinates
Eigen::Vector3d llh{30.0_deg, 120.0_deg, 250.0};
```

## Main Interface

### Class Definition

```cpp
template <typename _Para>
class Ellipsoid
```

`Ellipsoid` is a template class that accepts a type parameter containing static members `Re` and `F`, representing the ellipsoid's semi-major axis and flattening respectively.

### Constructors

```cpp
Ellipsoid() = default;
Ellipsoid(Eigen::Vector3d const &origin);
```

- Default constructor: Creates an ellipsoid without an origin
- Parameterized constructor: Creates an ellipsoid with a specified origin

### Main Methods

#### Setting Origin

```cpp
void SetOrigin(Eigen::Vector3d const &origin);
```

Sets the ellipsoid's origin for ENU coordinate system conversions.

#### Coordinate Conversion

```cpp
// Static methods
static Eigen::Vector3d LLH2ECEF(const Eigen::Vector3d &pos);
static Eigen::Vector3d ECEF2LLH(const Eigen::Vector3d &xyz);
static Eigen::Vector3d LLH2ENU(const Eigen::Vector3d &pos, const Eigen::Vector3d &origin);
static Eigen::Vector3d ENU2LLH(const Eigen::Vector3d &pos, const Eigen::Vector3d &origin);

// Instance methods
Eigen::Vector3d LLH2ENU(const Eigen::Vector3d &pos) const;
Eigen::Vector3d ENU2LLH(const Eigen::Vector3d &pos) const;
```

- `LLH2ECEF`: Converts Latitude-Longitude-Height coordinates to Earth-Centered, Earth-Fixed coordinates
- `ECEF2LLH`: Converts Earth-Centered, Earth-Fixed coordinates to Latitude-Longitude-Height coordinates
- `LLH2ENU`: Converts Latitude-Longitude-Height coordinates to East-North-Up coordinates
- `ENU2LLH`: Converts East-North-Up coordinates to Latitude-Longitude-Height coordinates

#### Coordinate Transformation

```cpp
static Eigen::Isometry3d Tn0n1(const Eigen::Vector3d &pos, const Eigen::Vector3d &origin);
```

Computes the transformation matrix from one position to another.

### Predefined Types

```cpp
using WGS84 = Ellipsoid<WGS84Para>;
```

`WGS84` is an instance of `Ellipsoid` using WGS84 ellipsoid parameters.

## Examples

### Basic Coordinate Conversion

```cpp
WGS84 wgs84;

// Test point: (30°N, 120°E, 0m)
Eigen::Vector3d llh{30.0_deg, 120.0_deg, 0.0};

// Convert to ECEF
Eigen::Vector3d ecef = WGS84::LLH2ECEF(llh);
// Expected result: {-2764128.319646, 4787610.688268, 3170373.735384}

// Convert back to LLH
Eigen::Vector3d llh_recovered = WGS84::ECEF2LLH(ecef);
// Expected result: {30.0°, 120.0°, 0.0m}
```

### ENU Coordinate Conversion with Origin

```cpp
// Set origin at (30°N, 120°E, 0m)
Eigen::Vector3d origin{30.0_deg, 120.0_deg, 0.0};
WGS84 wgs84(origin);

// Define ENU coordinates (0m East, 10m North, 0m Up)
Eigen::Vector3d enu{0.0, 10.0, 0.0};

// Convert to LLH
Eigen::Vector3d llh = wgs84.ENU2LLH(enu);
// Expected result: {30.00000787°N, 120.00000787°E, 0.0m}

// Convert back to ENU
Eigen::Vector3d enu_recovered = wgs84.LLH2ENU(llh);
// Expected result: {0.0m, 10.0m, 0.0m}

// Convert to ECEF
Eigen::Vector3d ecef = WGS84::LLH2ECEF(llh);
// Expected result: {-2764125.819646, 4787606.358141, 3170382.395638}
```

These examples demonstrate:
1. Basic LLH to ECEF conversion and back
2. ENU coordinate conversion with a custom origin
3. Coordinate transformation accuracy (within 1e-6 to 1e-8 precision)

## Testing

Run tests to verify the library's functionality:

```bash
cd build
./coordinate_converter_test
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Feel free to submit issues and pull requests. 