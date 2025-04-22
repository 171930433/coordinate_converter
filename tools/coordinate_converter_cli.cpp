#include "../coordinate_converter.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <gflags/gflags.h>
#include <glog/logging.h>

using namespace coordinate_converter;

// 字面量操作符，用于角度到弧度的转换
constexpr double operator"" _deg(long double x) { return x / 180.0 * M_PI; }
constexpr double operator"" _deg(unsigned long long x) { return x / 180.0 * M_PI; }

// 将角度转换为弧度
double deg2rad(double deg)
{
    return deg * M_PI / 180.0;
}

// 将弧度转换为角度
double rad2deg(double rad)
{
    return rad * 180.0 / M_PI;
}

// LLH到ECEF转换的命令行参数
DEFINE_double(lat, 0.0, "纬度（度）");
DEFINE_double(lon, 0.0, "经度（度）");
DEFINE_double(height, 0.0, "高度（米）");

// ECEF到LLH转换的命令行参数
DEFINE_double(x, 0.0, "ECEF X坐标（米）");
DEFINE_double(y, 0.0, "ECEF Y坐标（米）");
DEFINE_double(z, 0.0, "ECEF Z坐标（米）");

// ENU转换的命令行参数
DEFINE_double(origin_lat, 0.0, "原点纬度（度）");
DEFINE_double(origin_lon, 0.0, "原点经度（度）");
DEFINE_double(origin_height, 0.0, "原点高度（米）");
DEFINE_double(east, 0.0, "ENU东向坐标（米）");
DEFINE_double(north, 0.0, "ENU北向坐标（米）");
DEFINE_double(up, 0.0, "ENU上向坐标（米）");

// 将LLH转换为ECEF
void llh2ecef()
{
    double lat_rad = deg2rad(FLAGS_lat);
    double lon_rad = deg2rad(FLAGS_lon);
    double height = FLAGS_height;

    Eigen::Vector3d llh{lat_rad, lon_rad, height};
    Eigen::Vector3d ecef = WGS84::LLH2ECEF(llh);

    LOG(INFO) << "ECEF坐标 (米):";
    LOG(INFO) << "  X: " << std::fixed << std::setprecision(6) << ecef.x();
    LOG(INFO) << "  Y: " << std::fixed << std::setprecision(6) << ecef.y();
    LOG(INFO) << "  Z: " << std::fixed << std::setprecision(6) << ecef.z();
}

// 将ECEF转换为LLH
void ecef2llh()
{
    Eigen::Vector3d ecef{FLAGS_x, FLAGS_y, FLAGS_z};
    Eigen::Vector3d llh = WGS84::ECEF2LLH(ecef);

    LOG(INFO) << "LLH坐标:";
    LOG(INFO) << "  纬度: " << std::fixed << std::setprecision(6) << rad2deg(llh.x()) << "°";
    LOG(INFO) << "  经度: " << std::fixed << std::setprecision(6) << rad2deg(llh.y()) << "°";
    LOG(INFO) << "  高度: " << std::fixed << std::setprecision(6) << llh.z() << " 米";
}

// 将LLH转换为ENU
void llh2enu()
{
    double lat_rad = deg2rad(FLAGS_lat);
    double lon_rad = deg2rad(FLAGS_lon);
    double height = FLAGS_height;
    double origin_lat_rad = deg2rad(FLAGS_origin_lat);
    double origin_lon_rad = deg2rad(FLAGS_origin_lon);
    double origin_height = FLAGS_origin_height;

    Eigen::Vector3d llh{lat_rad, lon_rad, height};
    Eigen::Vector3d origin{origin_lat_rad, origin_lon_rad, origin_height};
    Eigen::Vector3d enu = WGS84::LLH2ENU(llh, origin);

    LOG(INFO) << "ENU坐标 (米):";
    LOG(INFO) << "  东: " << std::fixed << std::setprecision(6) << enu.x();
    LOG(INFO) << "  北: " << std::fixed << std::setprecision(6) << enu.y();
    LOG(INFO) << "  上: " << std::fixed << std::setprecision(6) << enu.z();
}

// 将ENU转换为LLH
void enu2llh()
{
    double origin_lat_rad = deg2rad(FLAGS_origin_lat);
    double origin_lon_rad = deg2rad(FLAGS_origin_lon);
    double origin_height = FLAGS_origin_height;

    Eigen::Vector3d enu{FLAGS_east, FLAGS_north, FLAGS_up};
    Eigen::Vector3d origin{origin_lat_rad, origin_lon_rad, origin_height};
    Eigen::Vector3d llh = WGS84::ENU2LLH(enu, origin);

    LOG(INFO) << "LLH坐标:";
    LOG(INFO) << "  纬度: " << std::fixed << std::setprecision(6) << rad2deg(llh.x()) << "°";
    LOG(INFO) << "  经度: " << std::fixed << std::setprecision(6) << rad2deg(llh.y()) << "°";
    LOG(INFO) << "  高度: " << std::fixed << std::setprecision(6) << llh.z() << " 米";
}

// 帮助信息
void printHelp()
{
    LOG(INFO) << "坐标转换工具 (Coordinate Converter CLI)";
    LOG(INFO) << "用法: coordinate_converter_cli <命令> [参数...]";
    LOG(INFO) << "";
    LOG(INFO) << "命令:";
    LOG(INFO) << "  llh2ecef --lat=<纬度> --lon=<经度> --height=<高度>";
    LOG(INFO) << "  ecef2llh --x=<x> --y=<y> --z=<z>";
    LOG(INFO) << "  llh2enu --lat=<纬度> --lon=<经度> --height=<高度> --origin_lat=<原点纬度> --origin_lon=<原点经度> --origin_height=<原点高度>";
    LOG(INFO) << "  enu2llh --east=<东> --north=<北> --up=<上> --origin_lat=<原点纬度> --origin_lon=<原点经度> --origin_height=<原点高度>";
    LOG(INFO) << "  help";
    LOG(INFO) << "";
    LOG(INFO) << "注意:";
    LOG(INFO) << "  - 角度单位为度，高度单位为米";
    LOG(INFO) << "  - 坐标顺序为: 纬度, 经度, 高度";
}

int main(int argc, char *argv[])
{
    // 初始化glog
    google::InitGoogleLogging(argv[0]);

    // 设置程序使用说明
    std::string usage = R"(
("坐标转换工具 (Coordinate Converter CLI))
  用法: coordinate_converter_cli <命令> [参数...]
  命令:
    llh2ecef --lat=<纬度> --lon=<经度> --height=<高度>
    ecef2llh --x=<x> --y=<y> --z=<z>
    llh2enu --lat=<纬度> --lon=<经度> --height=<高度> --origin_lat=<原点纬度> --origin_lon=<原点经度> --origin_height=<原点高度>
    enu2llh --east=<东> --north=<北> --up=<上> --origin_lat=<原点纬度> --origin_lon=<原点经度> --origin_height=<原点高度>
    help;
    注意:    角度单位为度，高度单位为米;    坐标顺序为: 纬度, 经度, 高度;
)";

    google::SetUsageMessage(usage);

    // 解析命令行参数
    google::ParseCommandLineFlags(&argc, &argv, true);

    if (argc < 2 || std::string(argv[1]) == "help")
    {
        printHelp();
        google::ShutdownGoogleLogging();
        return 0;
    }

    std::string command = argv[1];

    if (command == "llh2ecef")
    {
        llh2ecef();
    }
    else if (command == "ecef2llh")
    {
        ecef2llh();
    }
    else if (command == "llh2enu")
    {
        llh2enu();
    }
    else if (command == "enu2llh")
    {
        enu2llh();
    }
    else
    {
        LOG(ERROR) << "错误: 未知命令 '" << command << "'";
        printHelp();
        google::ShutdownGoogleLogging();
        return 1;
    }

    google::ShutdownGoogleLogging();
    return 0;
}