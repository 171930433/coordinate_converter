cmake_minimum_required(VERSION 3.10.0)

project(coordinate_converter VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Eigen3 CONFIG REQUIRED)

# 创建 header-only 库
add_library(coordinate_converter INTERFACE)
add_library(coordinate_converter::coordinate_converter ALIAS coordinate_converter)

# 设置包含目录
target_include_directories(coordinate_converter
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

# 链接依赖
target_link_libraries(coordinate_converter INTERFACE Eigen3::Eigen)

option(ENABLE_TESTS "ENABLE_TESTS OPTIONS" ON)
if(ENABLE_TESTING)
  include(CTest)
  enable_testing()
  add_subdirectory(tests)
endif()

# 安装配置
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# 安装头文件
install(FILES coordinate_converter.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/coordinate_converter
)

# 安装目标
install(TARGETS coordinate_converter
    EXPORT coordinate_converter-targets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# 安装导出文件
install(EXPORT coordinate_converter-targets
    FILE coordinate_converter-targets.cmake
    NAMESPACE coordinate_converter::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/coordinate_converter
)

# 生成并安装配置文件
write_basic_package_version_file(
    "${PROJECT_NAME}-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(FILES
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# 测试配置
include(CTest)
enable_testing()

