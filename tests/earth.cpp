#include "coordinate_converter.hpp"
#include <gtest/gtest.h>
#include <iomanip>
using namespace coordinate_converter;
constexpr double operator"" _deg(long double x) { return x / 180.0 * M_PI; }
constexpr double operator"" _deg(unsigned long long x) { return x / 180.0 * M_PI; }

TEST(Ellipsoid, type)
{
  EXPECT_TRUE(has_para_members<WGS84Para>::value);
  EXPECT_FALSE(has_para_members<int>::value);
}

TEST(Ellipsoid, base)
{

  WGS84 wgs84;

  Eigen::Vector3d ecef_true = {-2764128.3196464167, 4787610.6882675830, 3170373.7353836372};
  Eigen::Vector3d llh_true{30.0_deg, 120.0_deg, 0};
  Eigen::Vector3d ecef = WGS84::LLH2ECEF(llh_true);
  Eigen::Vector3d llh = WGS84::ECEF2LLH(ecef_true);

  GTEST_LOG_(INFO) << std::setprecision(6) << std::fixed << ecef_true.transpose();
  GTEST_LOG_(INFO) << std::setprecision(6) << std::fixed << llh_true.transpose();
  GTEST_LOG_(INFO) << std::setprecision(6) << std::fixed << ecef.transpose();
  GTEST_LOG_(INFO) << std::setprecision(6) << std::fixed << llh.transpose();

  EXPECT_TRUE(ecef_true.isApprox(ecef, 1e-6));
  EXPECT_TRUE(llh_true.isApprox(llh, 1e-6));
}

TEST(Ellipsoid, origin)
{

  Eigen::Vector3d origin{30.0_deg, 120.0_deg, 0};
  WGS84 wgs84(origin);

  Eigen::Vector3d enu_true = {0, 10, 0};
  Eigen::Vector3d llh_true{0.5236003501, 2.0943951024, 0.0000078734};

  Eigen::Vector3d enu = wgs84.LLH2ENU(llh_true);
  Eigen::Vector3d llh = wgs84.ENU2LLH(enu_true);

  GTEST_LOG_(INFO) << "origin " << std::setprecision(6) << std::fixed << (origin).transpose();
  GTEST_LOG_(INFO) << "llh_true " << std::setprecision(6) << std::fixed << (llh_true).transpose();
  GTEST_LOG_(INFO) << "enu_true " << std::setprecision(6) << std::fixed << (enu_true).transpose();
  GTEST_LOG_(INFO) << "enu 2 llh " << std::setprecision(6) << std::fixed << (llh).transpose();
  GTEST_LOG_(INFO) << "llh 2 enu " << std::setprecision(6) << std::fixed << (enu).transpose();

  EXPECT_TRUE(enu_true.isApprox(enu, 1e-4));
  EXPECT_TRUE(llh_true.isApprox(llh, 1e-8));
  EXPECT_TRUE(1);
}