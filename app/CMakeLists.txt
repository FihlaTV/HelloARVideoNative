# CMake版本信息
cmake_minimum_required(VERSION 3.4.1)

# 支持-std=gnu++11
set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

# 工程路径
set(pathToProject D:/android_studio_workspace/HelloARVideoNative)
# Native 路径（HelloARVideoNative/app/src/main/cpp）
set(pathToNative ${pathToProject}/app/src/main/cpp)

# 配置加载native依赖
include_directories(${pathToProject}/package)

# 添加待编译的文件
add_library(helloarvideo SHARED ${pathToNative}/easyar/ar.hpp
                                ${pathToNative}/easyar/renderer.hpp
                                ${pathToNative}/easyar/ar.cc
                                ${pathToNative}/easyar/renderer.cc
                                ${pathToNative}/easyar/helloarvideo.cc)

# 动态方式加载
add_library(so_EasyAR SHARED IMPORTED)
# 引入so文件
set_target_properties(so_EasyAR PROPERTIES IMPORTED_LOCATION ${pathToProject}/app/src/main/jniLibs/${ANDROID_ABI}/libEasyAR.so)

target_link_libraries(helloarvideo
                      so_EasyAR
                      log GLESv2)