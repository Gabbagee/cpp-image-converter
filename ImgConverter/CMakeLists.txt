cmake_minimum_required(VERSION 3.11)

project(Negate CXX)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(../ExtImgLib ImgLibBuildDir)

if (CMAKE_SYSTEM_NAME MATCHES "^MINGW")
    set(SYSTEM_LIBS -lstdc++)
else()
    set(SYSTEM_LIBS)
endif()

add_executable(imgconv main.cpp)
target_include_directories(imgconv PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../ExtImgLib")
target_link_libraries(imgconv ImgLib ${SYSTEM_LIBS})

# D:\Artem\Cpp-Course\cpp-image-converter\build_img_lib>cmake ../ImgConverter -DCMAKE_BUILD_TYPE=Debug -DLIBJPEG_DIR=D:\Artem\Cpp-Course\cpp-image-converter\JPEGLib -G "MinGW Makefiles"
# D:\Artem\Cpp-Course\cpp-image-converter\build_img_lib>cmake --build .
# D:\Artem\Cpp-Course\cpp-image-converter\build_img_lib>imgconv.exe ../b4.bmp result.ppm
