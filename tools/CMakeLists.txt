# 查找gflags库
find_package(gflags REQUIRED)

# 查找glog库
find_package(glog REQUIRED)

# 添加命令行工具可执行文件
add_executable(coordinate_converter_cli coordinate_converter_cli.cpp)

# 链接依赖
target_link_libraries(coordinate_converter_cli PRIVATE coordinate_converter gflags glog)

# 安装命令行工具
install(TARGETS coordinate_converter_cli
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
) 