#ifndef COORDINATE_CONVERTER_HPP
#define COORDINATE_CONVERTER_HPP

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <type_traits>

namespace coordinate_converter
{
  // 检测类型是否有Re和F静态成员
  template <typename T, typename = void>
  struct has_para_members : std::false_type
  {
  };

  template <typename T>
  struct has_para_members<T, std::void_t<decltype(T::Re), decltype(T::F)>> : std::true_type
  {
  };

  struct WGS84Para
  {
    static constexpr double Re = 6378137.0;
    static constexpr double F = (1.0 / 298.257223563);
  };
}

namespace coordinate_converter
{

  constexpr double operator"" _deg(long double x) { return x / 180.0 * M_PI; }
  constexpr double operator"" _deg(unsigned long long x) { return x / 180.0 * M_PI; }

  template <typename _Para>
  class Ellipsoid
  {
    static_assert(has_para_members<_Para>::value, "Ellipsoid parameter must be a *Para");
    static constexpr double _a = _Para::Re;
    static constexpr double _f = _Para::F;
    static constexpr double _b = (1 - _f) * _a;
    static constexpr double _c = _a * _a / _b;
    static constexpr double _e1 = sqrt(_a * _a - _b * _b) / _a;
    static constexpr double _e2 = sqrt(_a * _a - _b * _b) / _b;

  public:
    Ellipsoid() = default;
    Ellipsoid(Eigen::Vector3d const &origin) { SetOrigin(origin); }

    void SetOrigin(Eigen::Vector3d const &origin)
    {
      Ten_ = Eigen::Translation3d(LLH2ECEF(origin)) * Pos2Qne(origin);
    }

  public:
    static constexpr void LLH2ECEF(double const *pos, double *xyz)
    {
      double const &b = pos[0];
      double const &l = pos[1];
      double const &h = pos[2];
      double const &n = N(b);
      double const &cb = cos(b);
      xyz[0] = (n + h) * cb * cos(l);
      xyz[1] = (n + h) * cb * sin(l);
      xyz[2] = (n * (1 - _e1 * _e1) + h) * sin(b);
    }

    static constexpr void ECEF2LLH(double const *xyz, double *pos)
    {
      double const e1_2 = _e1 * _e1;
      double const r2 = xyz[0] * xyz[0] + xyz[1] * xyz[1];
      double v = _a;
      double z = xyz[2];
      double sinp = 0;
      for (double zk = 0; fabs(z - zk) >= 1e-4;)
      {
        zk = z;
        sinp = z / sqrt(r2 + z * z);
        v = _a / sqrt(1 - e1_2 * sinp * sinp);
        z = xyz[2] + v * e1_2 * sinp;
      }
      double temp_x = xyz[2] > 0.0 ? M_PI / 2.0 : -M_PI / 2.0;
      pos[0] = r2 > 1E-12 ? atan(z / sqrt(r2)) : temp_x;
      pos[1] = r2 > 1E-12 ? atan2(xyz[1], xyz[0]) : 0.0;
      pos[2] = sqrt(r2 + z * z) - v;
    }

    static Eigen::Quaterniond Pos2Qne(const Eigen::Vector3d &pos)
    {
      using namespace Eigen;
      return AngleAxisd(-(M_PI / 2 - pos[0]), Vector3d::UnitX()) * AngleAxisd(-(M_PI / 2 + pos[1]), Vector3d::UnitZ());
    }

    // eigen wrapper
    static Eigen::Vector3d LLH2ECEF(const Eigen::Vector3d &pos)
    {
      Eigen::Vector3d xyz = Eigen::Vector3d::Zero();
      LLH2ECEF(pos.data(), xyz.data());
      return xyz;
    }
    static Eigen::Vector3d ECEF2LLH(const Eigen::Vector3d &xyz)
    {
      Eigen::Vector3d pos = Eigen::Vector3d::Zero();
      ECEF2LLH(xyz.data(), pos.data());
      return pos;
    }

    // pos与origin均为纬经度，计算pos在origin坐标系下的东北天坐标
    static Eigen::Vector3d LLH2ENU(const Eigen::Vector3d &pos, const Eigen::Vector3d &origin)
    {
      Eigen::Isometry3d Ten = Eigen::Translation3d(LLH2ECEF(origin)) * Pos2Qne(origin);
      return Ten.inverse() * LLH2ECEF(pos);
    }

    // pos为origin下的enu坐标，orign为纬经度
    static Eigen::Vector3d ENU2LLH(const Eigen::Vector3d &pos, const Eigen::Vector3d &origin)
    {
      Eigen::Isometry3d Ten = Eigen::Translation3d(LLH2ECEF(origin)) * Pos2Qne(origin);
      return ECEF2LLH(Ten * pos);
    }

    Eigen::Vector3d LLH2ENU(const Eigen::Vector3d &pos) const
    {
      assert(!Ten_.translation().isZero(1e-12));
      return Ten_.inverse() * LLH2ECEF(pos);
    }
    Eigen::Vector3d ENU2LLH(const Eigen::Vector3d &pos) const
    {
      assert(!Ten_.translation().isZero(1e-12));
      return ECEF2LLH(Ten_ * pos);
    }

  public:
    Eigen::Isometry3d Ten_ = Eigen::Isometry3d::Identity();

  public:
    static constexpr double W(const double B_) { return sqrt(1 - pow(_e1 * sin(B_), 2)); }
    static constexpr double V(const double B_) { return sqrt(1 + pow(_e2 * cos(B_), 2)); }
    static constexpr double M(const double B_) { return _c / pow(V(B_), 3); } // 子午曲率半径
    static constexpr double N(const double B_) { return _c / V(B_); }         // 卯酉曲率半径
  };

  using WGS84 = Ellipsoid<WGS84Para>;

} // namespace coordinate_converter

#endif // COORDINATE_CONVERTER_HPP