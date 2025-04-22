# 坐标转换库 (Coordinate Converter)

这是一个用于地理坐标系统转换的C++库，支持WGS84椭球体上的各种坐标系统之间的转换。

## 功能特点

- 支持WGS84椭球体上的坐标转换
- 提供纬经度高度(LLH)到地心地固坐标系(ECEF)的转换
- 提供地心地固坐标系(ECEF)到纬经度高度(LLH)的转换
- 提供纬经度高度(LLH)到东北天坐标系(ENU)的转换
- 提供东北天坐标系(ENU)到纬经度高度(LLH)的转换
- 支持自定义椭球体参数

## 依赖项

- Eigen3：用于矩阵和向量运算
- C++11或更高版本

## 安装

```bash
# 克隆仓库
git clone https://github.com/yourusername/coordinate_converter.git
cd coordinate_converter

# 编译测试
mkdir build && cd build
cmake ..
make
```

## 使用方法

### 基本用法

```cpp
#include "coordinate_converter.hpp"

using namespace coordinate_converter;

// 创建WGS84椭球体实例
WGS84 wgs84;

// 定义纬经度高度坐标 (经度, 纬度, 高度)
// 注意：纬经度单位为弧度
Eigen::Vector3d llh{120.0 * M_PI / 180.0, 30.0 * M_PI / 180.0, 250.0};

// 转换为ECEF坐标
Eigen::Vector3d ecef = wgs84.LLH2ECEF(llh);

// 从ECEF坐标转回LLH坐标
Eigen::Vector3d llh_recovered = wgs84.ECEF2LLH(ecef);
```

### 自定义原点

```cpp
// 设置原点
Eigen::Vector3d origin{120.0 * M_PI / 180.0, 30.0 * M_PI / 180.0, 0.0};
WGS84 wgs84(origin);

// 定义纬经度高度坐标
Eigen::Vector3d llh{120.005 * M_PI / 180.0, 30.005 * M_PI / 180.0, 10.0};

// 转换为ENU坐标
Eigen::Vector3d enu = wgs84.LLH2ENU(llh);

// 从ENU坐标转回LLH坐标
Eigen::Vector3d llh_recovered = wgs84.ENU2LLH(enu);
```

## 主要接口

### 类定义

```cpp
template <typename _Para>
class Ellipsoid
```

`Ellipsoid` 是一个模板类，接受一个包含 `Re` 和 `F` 静态成员的类型参数，分别表示椭球体的长半轴和扁率。

### 构造函数

```cpp
Ellipsoid() = default;
Ellipsoid(Eigen::Vector3d const &origin);
```

- 默认构造函数：创建一个没有设置原点的椭球体
- 带参数构造函数：创建一个并设置原点的椭球体

### 主要方法

#### 设置原点

```cpp
void SetOrigin(Eigen::Vector3d const &origin);
```

设置椭球体的原点，用于ENU坐标系的转换。

#### 坐标转换

```cpp
// 静态方法
static Eigen::Vector3d LLH2ECEF(const Eigen::Vector3d &pos);
static Eigen::Vector3d ECEF2LLH(const Eigen::Vector3d &xyz);
static Eigen::Vector3d LLH2ENU(const Eigen::Vector3d &pos, const Eigen::Vector3d &origin);
static Eigen::Vector3d ENU2LLH(const Eigen::Vector3d &pos, const Eigen::Vector3d &origin);

// 实例方法
Eigen::Vector3d LLH2ENU(const Eigen::Vector3d &pos) const;
Eigen::Vector3d ENU2LLH(const Eigen::Vector3d &pos) const;
```

- `LLH2ECEF`：将纬经度高度坐标转换为地心地固坐标系坐标
- `ECEF2LLH`：将地心地固坐标系坐标转换为纬经度高度坐标
- `LLH2ENU`：将纬经度高度坐标转换为东北天坐标系坐标
- `ENU2LLH`：将东北天坐标系坐标转换为纬经度高度坐标

#### 坐标变换

```cpp
static Eigen::Isometry3d Tn0n1(const Eigen::Vector3d &pos, const Eigen::Vector3d &origin);
```

计算从一个位置到另一个位置的变换矩阵。

### 预定义类型

```cpp
using WGS84 = Ellipsoid<WGS84Para>;
```

`WGS84` 是一个使用WGS84椭球体参数的 `Ellipsoid` 实例。

## 示例

### 纬经度高度到ECEF的转换

```cpp
WGS84 wgs84;
Eigen::Vector3d llh{120.0 * M_PI / 180.0, 30.0 * M_PI / 180.0, 250.0};
Eigen::Vector3d ecef = wgs84.LLH2ECEF(llh);
```

### 纬经度高度到ENU的转换

```cpp
Eigen::Vector3d origin{120.0 * M_PI / 180.0, 30.0 * M_PI / 180.0, 0.0};
WGS84 wgs84(origin);
Eigen::Vector3d llh{120.005 * M_PI / 180.0, 30.005 * M_PI / 180.0, 10.0};
Eigen::Vector3d enu = wgs84.LLH2ENU(llh);
```

## 测试

运行测试以验证库的功能：

```bash
cd build
./coordinate_converter_test
```

## 许可证

[待定]

## 贡献

欢迎提交问题和拉取请求！ 