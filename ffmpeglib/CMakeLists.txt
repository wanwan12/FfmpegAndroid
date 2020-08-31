# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.

cmake_minimum_required(VERSION 3.4.1)


#add_library(
#        native-lib
#        SHARED
#        src/main/cpp/native-lib.cpp)

add_library(
        ffmpeg-examples
        SHARED
        src/main/cpp/ffmpeg-examples.cpp)

find_library(
        log-lib
        log)


set(distribution_DIR ${CMAKE_SOURCE_DIR}/../../../../libs)

add_library(avutil
        SHARED
        IMPORTED)
set_target_properties(avutil
        PROPERTIES IMPORTED_LOCATION
        ../../../../libs/armeabi-v7a/libavutil.so)

add_library(swresample
        SHARED
        IMPORTED)
set_target_properties(swresample
        PROPERTIES IMPORTED_LOCATION
        ../../../../libs/armeabi-v7a/libswresample.so)
add_library(avcodec
        SHARED
        IMPORTED)
set_target_properties(avcodec
        PROPERTIES IMPORTED_LOCATION
        ../../../../libs/armeabi-v7a/libavcodec.so)
add_library(avfilter
        SHARED
        IMPORTED)
set_target_properties(avfilter
        PROPERTIES IMPORTED_LOCATION
        ../../../../libs/armeabi-v7a/libavfilter.so)
add_library(swscale
        SHARED
        IMPORTED)
set_target_properties(swscale
        PROPERTIES IMPORTED_LOCATION
        ../../../../libs/armeabi-v7a/libswscale.so)
add_library(avdevice
        SHARED
        IMPORTED)
set_target_properties(avdevice
        PROPERTIES IMPORTED_LOCATION
        ../../../../libs/armeabi-v7a/libavdevice.so)
add_library(avformat
        SHARED
        IMPORTED)
set_target_properties(avformat
        PROPERTIES IMPORTED_LOCATION
        ../../../../libs/armeabi-v7a/libavformat.so)
add_library(avresample
        SHARED
        IMPORTED)
set_target_properties(avresample
        PROPERTIES IMPORTED_LOCATION
        ../../../../libs/armeabi-v7a/libavresample.so)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")


include_directories(libs/include)

#target_include_directories(native-lib PRIVATE libs/include)

target_link_libraries( ffmpeg-examples log android avutil swresample avcodec avfilter swscale avdevice avformat avresample
        ${log-lib})
