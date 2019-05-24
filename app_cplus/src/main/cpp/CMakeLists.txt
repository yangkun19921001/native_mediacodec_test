cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -UNDEBUG")

add_library(native-codec-jni SHARED
        native-codec-jni.cpp
        main_video.cpp)

add_library(hwcodec_ndk
        SHARED
        IMPORTED)

set_target_properties(hwcodec_ndk
        PROPERTIES
        IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../../../libs/${ANDROID_ABI}/libhwcodec_ndk.a)

# Include libraries needed for native-codec-jni lib
target_link_libraries(native-codec-jni
        android
        hwcodec_ndk
        log
        mediandk
        OpenMAXAL)

