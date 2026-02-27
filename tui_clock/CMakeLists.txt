cmake_minimum_required(VERSION 3.10)

# 设置项目名称和版本
project(tui_clock VERSION 1.0 DESCRIPTION "A simple TUI clock written in C")

# 指定 C 语言标准
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# 添加编译选项
add_compile_options(-Wall -Wextra)

# 添加可执行文件，包含所有 C 源文件
add_executable(${PROJECT_NAME} main.c timeDeal.c renderer.c)
