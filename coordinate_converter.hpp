#ifndef COORDINATE_CONVERTER_HPP
#define COORDINATE_CONVERTER_HPP

#include <Eigen/Core>
#include <Eigen/Geometry>
namespace coordinate_converter {

constexpr double WGS84_ELLIPSOID_RE = 6378137.0;
constexpr double WGS84_ELLIPSOID_F = (1.0 / 298.257223563);

template <double _Re, double _F>
struct Ellipsoid {
public:
  Ellipsoid() = default;
  Ellipsoid(Eigen::Vector3d const& origin) { SetOrigin(origin); }

  void SetOrigin(Eigen::Vector3d const& origin) {
    origin_ecef_ = LLH2ECEF(origin);
    origin_cne_ = Pos2Cne(origin);
  }

public:
  static constexpr void LLH2ECEF(double const* pos, double* xyz) {
    double const& b = pos[0];
    double const& l = pos[1];
    double const& h = pos[2];
    double const& n = N(b);
    double const& cb = cos(b);
    xyz[0] = (n + h) * cb * cos(l);
    xyz[1] = (n + h) * cb * sin(l);
    xyz[2] = (n * (1 - _e1 * _e1) + h) * sin(b);
  }

  static constexpr void ECEF2LLH(double const* xyz, double* pos) {
    double const e1_2 = _e1 * _e1;
    double const r2 = std::hypot(xyz[0], xyz[1]);
    double v = _a;
    double z = xyz[2];
    double sinp = 0;
    for (double zk = 0; fabs(z - zk) >= 1e-4;) {
      zk = z;
      sinp = z / sqrt(r2 + z * z);
      v = _a / sqrt(1 - e1_2 * sinp * sinp);
      z = xyz[2] + v * e1_2 * sinp;
    }
    auto temp_x = xyz[2] > 0.0 ? M_PI / 2.0 : -M_PI / 2.0;
    pos[0] = r2 > 1E-12 ? atan(z / sqrt(r2)) : temp_x;
    pos[1] = r2 > 1E-12 ? atan2(xyz[1], xyz[0]) : 0.0;
    pos[2] = sqrt(r2 + z * z) - v;
  }

  static Eigen::Matrix3d Pos2Cne(const Eigen::Vector3d& pos) {
    using namespace Eigen;
    double const& b = pos[0];
    double const& l = pos[1];
    Matrix3d re = (AngleAxisd(-(M_PI / 2 - b), Vector3d::UnitX()) * AngleAxisd(-(M_PI / 2 + l), Vector3d::UnitZ()))
                    .toRotationMatrix();
    return re;
  }

  // eigen wrapper
  static Eigen::Vector3d LLH2ECEF(const Eigen::Vector3d& pos) {
    Eigen::Vector3d xyz = Eigen::Vector3d::Zero();
    LLH2ECEF(pos.data(), xyz.data());
    return xyz;
  }
  static Eigen::Vector3d ECEF2LLH(const Eigen::Vector3d& xyz) {
    Eigen::Vector3d pos = Eigen::Vector3d::Zero();
    ECEF2LLH(xyz.data(), pos.data());
    return pos;
  }

  static Eigen::Vector3d LLH2ENU(const Eigen::Vector3d& pos, const Eigen::Vector3d& origin) {
    Eigen::Vector3d origin_ecef = LLH2ECEF(origin);
    Eigen::Matrix3d c_ne = Pos2Cne(origin);
    Eigen::Vector3d local_enu = c_ne * (LLH2ECEF(pos) - origin_ecef);
    return local_enu;
  }

  Eigen::Vector3d LLH2ENU(const Eigen::Vector3d& pos) const {
    assert(origin_cne_.isZero(1e-12));
    return origin_cne_ * (LLH2ECEF(pos) - origin_ecef_);
  }
  Eigen::Vector3d ENU2LLh(const Eigen::Vector3d& pos) const {
    assert(origin_cne_.isZero(1e-12));
    return ECEF2LLH(origin_cne_ + origin_cne_.transpose() * pos);
  }

  static Eigen::Vector3d ENU2LLH(const Eigen::Vector3d& pos, const Eigen::Vector3d& origin) {
    Eigen::Vector3d const origin_ecef = LLH2ECEF(origin);
    Eigen::Matrix3d const c_ne = Pos2Cne(origin);
    Eigen::Vector3d const pos_llh = ECEF2LLH(c_ne.transpose() * pos + origin_ecef);
    return pos_llh;
  }

  static Eigen::Isometry3d Tn0n1(const Eigen::Vector3d& pos, const Eigen::Vector3d& origin) {
    using namespace Eigen;
    Matrix3d const Cn0_e = Pos2Cne(origin);
    Matrix3d const Cn1_e = Pos2Cne(pos);
    Isometry3d const T_n0_n1 = Translation3d(LLH2ENU(pos, origin)) * Quaterniond(Cn0_e * Cn1_e.transpose());
    return T_n0_n1;
  }

public:
  Eigen::Vector3d origin_ecef_{ 0, 0, 0 };
  Eigen::Matrix3d origin_cne_ = Eigen::Matrix3d::Identity();

public:
  static constexpr double _a = _Re;
  static constexpr double _f = _F;
  static constexpr double _b = (1 - _f) * _a;
  static constexpr double _c = _a * _a / _b;
  static constexpr double _e1 = sqrt(_a * _a - _b * _b) / _a;
  static constexpr double _e2 = sqrt(_a * _a - _b * _b) / _b;

  static constexpr double W(const double B_) { return sqrt(1 - pow(_e1 * sin(B_), 2)); }
  static constexpr double V(const double B_) { return sqrt(1 + pow(_e2 * cos(B_), 2)); }
  static constexpr double M(const double B_) { return _c / pow(V(B_), 3); } // 子午曲率半径
  static constexpr double N(const double B_) { return _c / V(B_); }         // 卯酉曲率半径
};

using WGS84 = Ellipsoid<WGS84_ELLIPSOID_RE, WGS84_ELLIPSOID_F>;

} // namespace coordinate_converter

#endif // COORDINATE_CONVERTER_HPP 