#include "time_system.hpp"
#include <gtest/gtest.h>
#include <iomanip>
#include <chrono>

using namespace time_system;

TEST(TimeSystem, GPST2Unix)
{
  // 测试GPST到Unix时间的转换
  int32_t week = 2000;
  double seconds = 3600.0;

  // 使用默认的微秒精度
  int64_t unix_time = GPST2Unix(week, seconds);

  // 使用纳秒精度
  int64_t unix_time_ns = GPST2Unix<std::chrono::nanoseconds>(week, seconds);

  // 使用毫秒精度
  int64_t unix_time_ms = GPST2Unix<std::chrono::milliseconds>(week, seconds);

  // 验证不同精度的转换结果一致（考虑精度差异）
  EXPECT_EQ(unix_time / 1000, unix_time_ms);
  EXPECT_EQ(unix_time, unix_time_ns / 1000);

  // 测试使用gpst_t类型的转换
  gpst_t gpst = std::make_pair(week, seconds);
  int64_t unix_time_pair = GPST2Unix(gpst);
  EXPECT_EQ(unix_time, unix_time_pair);
}

TEST(TimeSystem, Unix2GPST)
{
  // 测试Unix时间到GPST的转换
  int64_t unix_time = 1609459200000000; // 2021-01-01 00:00:00 UTC

  // 使用默认的微秒精度
  gpst_t gpst = Unix2GPST(unix_time);

  // 使用纳秒精度
  gpst_t gpst_ns = Unix2GPST<std::chrono::nanoseconds>(unix_time * 1000);

  // 使用毫秒精度
  gpst_t gpst_ms = Unix2GPST<std::chrono::milliseconds>(unix_time / 1000);

  // 验证不同精度的转换结果一致（考虑精度差异）
  EXPECT_EQ(gpst.first, gpst_ns.first);
  EXPECT_EQ(gpst.first, gpst_ms.first);

  // 验证秒数部分在合理范围内
  EXPECT_GE(gpst.second, 0.0);
  EXPECT_LT(gpst.second, 7 * 24 * 3600.0); // 一周的秒数
}

TEST(TimeSystem, Epoch2Unix)
{
  // 测试从年月日时分秒到Unix时间的转换
  int year = 2021;
  int month = 1;
  int day = 1;
  int hour = 0;
  int minute = 0;
  double second = 0.0;

  int64_t unix_time = Epoch2Unix(year, month, day, hour, minute, second);

  // 测试从DDMMYY和HHMMSS.SS格式到Unix时间的转换
  int32_t ddmmyy = 10121; // 01-01-21
  double hhmmss = 0.0;    // 00:00:00.0

  int64_t unix_time_format = Epoch2Unix(ddmmyy, hhmmss);

  // 两种方式应该得到相同的结果
  EXPECT_EQ(unix_time, unix_time_format);
}

TEST(TimeSystem, Unix2TimeStr)
{
  // 测试Unix时间到字符串的转换
  uint64_t time_us = 1609459200000000; // 2021-01-01 00:00:00 UTC

  std::string time_str = Unix2TimeStr(time_us);

  // 验证字符串格式
  EXPECT_EQ(time_str.substr(0, 19), "2021-01-01 00:00:00");

  // 测试不同精度的转换
  std::string time_str_ms = Unix2TimeStr<std::chrono::milliseconds>(time_us / 1000);
  std::string time_str_s = Unix2TimeStr<std::chrono::seconds>(time_us / 1000000);

  // 验证不同精度的转换结果一致（考虑精度差异）
  EXPECT_EQ(time_str.substr(0, 19), time_str_ms.substr(0, 19));
  EXPECT_EQ(time_str.substr(0, 19), time_str_s.substr(0, 19));
}

TEST(TimeSystem, Str2Unix)
{
  // 测试字符串到Unix时间的转换
  std::string time_str = "2021-01-01 00:00:00";

  int64_t unix_time = Str2Unix(time_str);

  // 验证转换结果
  EXPECT_EQ(unix_time, Epoch2Unix(2021, 1, 1, 0, 0, 0.0));

  // 测试无效字符串
  std::string inalid_str = "invalid";
  int64_t invalid_time = Str2Unix(inalid_str);

  // 无效字符串应该返回0
  EXPECT_EQ(invalid_time, 0);
}

TEST(TimeSystem, GPST2Str)
{
  // 测试GPST到字符串的转换
  gpst_t gpst = std::make_pair(2000, 3600.0);

  std::string gpst_str = GPST2Str(gpst);
  std::string gpst_str_week = GPST2Str(gpst, true);

  // 验证字符串格式
  EXPECT_EQ(gpst_str_week.substr(0, 6), "  2000");
  EXPECT_EQ(gpst_str_week.substr(6, 16), "     3600.000000");

  // 测试不同精度的转换
  std::string gpst_str_ns = GPST2Str<std::chrono::nanoseconds>(gpst);
  std::string gpst_str_ms = GPST2Str<std::chrono::milliseconds>(gpst);

  // 验证不同精度的转换结果一致（考虑精度差异）
  EXPECT_EQ(gpst_str.substr(0 + 3, 16), gpst_str_ns.substr(0, 16 - 3));
  EXPECT_EQ(gpst_str.substr(0, 16 - 3), gpst_str_ms.substr(0 + 3, 16));
}

TEST(TimeSystem, Unix2GPSTStr)
{
  // 测试Unix时间到GPST字符串的转换
  unix_t unix_time = 1609459200000000; // 2021-01-01 00:00:00 UTC

  std::string gpst_str = Unix2GPSTStr(unix_time);
  std::string gpst_str_week = Unix2GPSTStr(unix_time, true);

  // 验证字符串格式
  EXPECT_EQ(gpst_str_week.substr(0, 6), "  2138"); // 2021-01-01对应的GPS周

  // 测试不同精度的转换
  std::string gpst_str_ns = Unix2GPSTStr<std::chrono::nanoseconds>(unix_time * 1000);
  std::string gpst_str_ms = Unix2GPSTStr<std::chrono::milliseconds>(unix_time / 1000);

  // 验证不同精度的转换结果一致（考虑精度差异）
  EXPECT_EQ(gpst_str.substr(0 + 3, 16), gpst_str_ns.substr(0, 16 - 3));
  EXPECT_EQ(gpst_str.substr(0, 16 - 3), gpst_str_ms.substr(0 + 3, 16));
}

TEST(TimeSystem, Unix2Str)
{
  // 测试Unix时间到字符串的转换
  unix_t unix_time = 1609459200000000; // 2021-01-01 00:00:00 UTC

  std::string unix_str = Unix2Str(unix_time);

  // 验证字符串格式
  EXPECT_EQ(unix_str, "    1609459200000000");

  // 测试不同精度的转换
  std::string unix_str_ns = Unix2Str<std::chrono::nanoseconds>(unix_time * 1000);
  std::string unix_str_ms = Unix2Str<std::chrono::milliseconds>(unix_time / 1000);

  // 验证不同精度的转换结果一致（考虑精度差异）
  EXPECT_EQ(unix_str.substr(4, 16), unix_str_ns.substr(4, 16));
  EXPECT_EQ(unix_str.substr(4, 13), unix_str_ms.substr(4, 16));

}

TEST(TimeSystem, FullTimeString)
{
  // 测试完整时间字符串的转换
  double t_s = 1609459200.0; // 2021-01-01 00:00:00 UTC

  std::string full_str = FullTimeString(t_s);

  // 验证字符串格式
  EXPECT_EQ(full_str.substr(0, 20), "   1609459200.000000");
  EXPECT_EQ(full_str.substr(20, 6), "  2138"); // 2021-01-01对应的GPS周

  // 测试UnixTimeString
  std::string unix_str = UnixTimeString(t_s);

  // 验证字符串格式
  EXPECT_EQ(unix_str, "1609459200.000000");
}

TEST(TimeSystem, UnixSecondsToString)
{
  // 测试Unix秒数到字符串的转换
  uint64_t unix_seconds = 1609459200; // 2021-01-01 00:00:00 UTC

  std::string time_str = UnixSecondsToString(unix_seconds);

  // 验证字符串格式
  EXPECT_EQ(time_str, "2021-01-01 00:00:00");

  // 测试自定义格式
  std::string custom_format = "%Y/%m/%d %H:%M:%S";
  std::string custom_str = UnixSecondsToString(unix_seconds, custom_format);

  // 验证自定义格式
  EXPECT_EQ(custom_str, "2021/01/01 00:00:00");
}

TEST(TimeSystem, CurrentUnixTime)
{
  // 测试获取当前Unix时间
  double current_time = CurrentUnixTime();

  // 验证当前时间在合理范围内
  // 2021-01-01 00:00:00 UTC对应的Unix时间为1609459200.0
  EXPECT_GT(current_time, 1609459200.0);

  // 获取系统当前时间作为参考
  auto now = std::chrono::system_clock::now();
  auto now_s = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

  // 验证当前时间与系统时间接近（允许1秒的误差）
  EXPECT_NEAR(current_time, static_cast<double>(now_s), 1.0);
}