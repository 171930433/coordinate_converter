#pragma once
#include <chrono>
#include <algorithm>
#include <array>
#include <string>
#include <iomanip>
#include <sstream>
#include <vector>
#include <cmath>

namespace time_system
{
  namespace sc = std::chrono;
  using _tps = sc::time_point<sc::system_clock, sc::seconds>;
  using _week = sc::duration<int32_t, std::ratio<7 * 24 * 3600>>;
  using _day = sc::duration<int32_t, std::ratio<24 * 3600>>;
  using _ms = sc::milliseconds;
  using _us = sc::microseconds;
  using _ns = sc::nanoseconds;
  using _d_second = sc::duration<double, sc::seconds::period>;
  using gpst_t = std::pair<int32_t, double>;
  using unix_t = int64_t;

  namespace inner
  {
    static constexpr std::array<int, 12> doy = {1, 32, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};
    static constexpr int Days(const int y, const int m, const int d)
    {
      return (y - 1970) * 365 + (y - 1969) / 4 + doy[m - 1] + d - 2 + (y % 4 == 0 && m >= 3 ? 1 : 0);
    }
    static constexpr _tps GenerateTimeFromYYMMDD(const int y, const int m, const int d) { return _tps(_day(Days(y, m, d))); }
    static constexpr _tps gpst0 = GenerateTimeFromYYMMDD(1980, 1, 6);
    static constexpr sc::seconds delta_gpst0 = gpst0.time_since_epoch();
    static constexpr std::pair<int64_t, int32_t> __leaps[] = {
        {1483228800, -18}, {1435708800, -17}, {1341100800, -16}, {1230768000, -15}, {1136073600, -14}, {915148800, -13}, {867715200, -12}, {820454400, -11}, {773020800, -10}, {741484800, -9}, {709948800, -8}, {662688000, -7}, {631152000, -6}, {567993600, -5}, {489024000, -4}, {425865600, -3}, {394329600, -2}, {362793600, -1}};
    static constexpr int Leaps() { return -18; }
    inline int GetLeas(const _tps &t_)
    {
      auto pos = std::find_if(std::begin(__leaps), std::end(__leaps),
                              [&t_](const std::pair<int64_t, int32_t> &row)
                              { return t_.time_since_epoch().count() > row.first; });
      return pos == std::end(__leaps) ? 0 : pos->second;
    }
  }

  inline double CurrentUnixTime() { return sc::system_clock::now().time_since_epoch().count() / 1e9; }
  template <typename _Dura = sc::microseconds>
  constexpr typename _Dura::rep GPST2Unix(const int32_t w_, const double s_)
  {
    auto other = static_cast<typename _Dura::rep>((s_ + inner::Leaps()) * _Dura::period::den);
    return (inner::delta_gpst0 + _week(w_) + _Dura(other)).count();
  }

  template <typename _Dura = sc::microseconds>
  constexpr typename _Dura::rep GPST2Unix(const gpst_t &gpst_)
  {
    return GPST2Unix<_Dura>(gpst_.first, gpst_.second);
  }

  template <typename _Dura = sc::microseconds>
  constexpr gpst_t Unix2GPST(const int64_t t_)
  {
    auto gpst = _Dura(t_) - inner::delta_gpst0 - sc::seconds(inner::Leaps());
    auto w = gpst / _week(1);
    auto s = _d_second(gpst % _week(1)).count();
    return gpst_t(w, s);
  }

  template <typename _Dura = sc::microseconds>
  inline std::string Unix2TimeStr(const uint64_t time_us)
  {
    time_t t2 = sc::duration_cast<sc::seconds>(_Dura(time_us)).count();
    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&t2), "%Y-%m-%d %H:%M:%S");
    oss << '.' << std::setw(log10(_Dura::period::den)) << std::setfill('0') << (_Dura(time_us) - sc::seconds(t2)).count();
    return oss.str();
  }

  template <typename _Dura = sc::microseconds>
  constexpr int64_t Epoch2Unix(const int y = 1970, const int m = 0, const int d = 0, const int hh = 0, const int mm = 0,
                               const double ss = 0)
  {
    auto other = static_cast<typename _Dura::rep>(ss * _Dura::period::den);
    return (_day(inner::Days(y, m, d)) + sc::hours(hh) + sc::minutes(mm) + _Dura(other)).count();
  }

  /**
   * @brief GPRMC中的年月日时分秒转unix时间
   *
   * @tparam sc::microseconds
   * @param ddmmyy 日月年
   * @param hhmmss.ss 时分秒.秒
   * @return constexpr int64_t
   */
  template <typename _Dura = sc::microseconds>
  constexpr int64_t Epoch2Unix(const int32_t ddmmyy, const double hhmmss)
  {
    int32_t dd = (ddmmyy / 10000) % 10000;
    int32_t mon = (ddmmyy / 100) % 100;
    int32_t yy = ddmmyy % 100 + 2000;
    int32_t hh = (static_cast<int32_t>(hhmmss) / 10000) % 10000;
    int32_t mm = (static_cast<int32_t>(hhmmss) / 100) % 100;
    double ss = (static_cast<int32_t>(hhmmss) % 100) + (hhmmss - static_cast<int32_t>(hhmmss));
    return Epoch2Unix<_Dura>(yy, mon, dd, hh, mm, ss);
  }

  template <typename _Dura = sc::microseconds>
  inline int64_t Str2Unix(const std::string &str_)
  {
    int data[5] = {0};
    double sec = 0;
    int n = sscanf(str_.c_str(), "%d-%d-%d %d:%d:%lf", data, data + 1, data + 2, data + 3, data + 4, &sec);
    return n > 0 ? Epoch2Unix<_Dura>(data[0], data[1], data[2], data[3], data[4], sec) : 0;
  }

  template <typename _Dura = sc::microseconds>
  inline std::string GPST2Str(const gpst_t &t_, bool show_week_ = false)
  {
    std::ostringstream oss;
    if (show_week_)
      oss << std::setw(6) << t_.first;
    oss << std::setw(16) << std::fixed << std::setprecision(log10(_Dura::period::den)) << t_.second;

    return oss.str();
  }

  template <typename _Dura = sc::microseconds>
  inline std::string GPST2Str(const int32_t w_, const double s_, bool show_week_ = false)
  {
    return GPST2Str<_Dura>(std::make_pair(w_, s_), show_week_);
  }

  template <typename _Dura = sc::microseconds>
  inline std::string Unix2GPSTStr(const unix_t &t_, bool show_week_ = false)
  {
    return GPST2Str<_Dura>(Unix2GPST<_Dura>(t_), show_week_);
  }

  template <typename _Dura = sc::microseconds>
  inline std::string Unix2Str(const unix_t &t_)
  {
    std::ostringstream oss;
    oss << std::setw(log10(_Dura::period::den) + 14) << t_;
    return oss.str();
  }

  /**
   * @brief 输出unix时间,gps时间
   *
   * @param t_s 以秒为单位的unix时间
   * @return std::string
   */
  inline std::string FullTimeString(double t_s)
  {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6) << std::setw(20) << t_s << Unix2GPSTStr(t_s / 1e-6, true);
    return oss.str();
  }

  inline std::string UnixTimeString(double t_s)
  {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6) << std::setw(15) << t_s;
    return oss.str();
  }

  inline std::string UnixSecondsToString(uint64_t unix_seconds, const std::string &format_str = "%Y-%m-%d %H:%M:%S")
  {
    std::time_t t = unix_seconds;
    struct tm ptm;
    struct tm *ret = gmtime_r(&t, &ptm);
    if (ret == nullptr)
    {
      return std::string("");
    }
    uint32_t length = 64;
    std::vector<char> buff(length, '\0');
    strftime(buff.data(), length, format_str.c_str(), ret);
    return std::string(buff.data());
  }
}