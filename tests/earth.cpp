#include "coordinate_converter.hpp"
#include <gtest/gtest.h>

using namespace coordinate_converter;
constexpr double dpr = 180.0 / M_PI;
constexpr double rpd = M_PI / 180.0;

TEST(Earth, base)
{

  WGS84 wgs84;

  Eigen::Vector3d ecef_true = {801927, -5.13661e+06, 3.68258e+06};
  Eigen::Vector3d llh{(120.0 * rpd), (30.0 * rpd), 0};
  Eigen::Vector3d ecef = WGS84::LLH2ECEF(llh);

  GTEST_LOG_(INFO) << llh.transpose();
  GTEST_LOG_(INFO) << ecef.transpose();

  EXPECT_TRUE(ecef_true.isApprox(ecef, 1e-6));
}

TEST(Earth, origin)
{

  Eigen::Vector3d origin{(120.0 * rpd), (30.0 * rpd), 0};
  WGS84 wgs84(origin);

  Eigen::Vector3d enu_true = {25901.5, 31823, -122.186};
  Eigen::Vector3d llh{(120.005 * rpd), (30.005 * rpd), 10};
  Eigen::Vector3d enu = wgs84.LLH2ENU(llh);

  GTEST_LOG_(INFO) << enu.transpose();

  EXPECT_TRUE(enu_true.isApprox(enu, 1e-6));
}