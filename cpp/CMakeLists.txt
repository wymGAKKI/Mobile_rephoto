# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

# Set OpenCV
add_library(libopencv_java3 SHARED IMPORTED)
set_target_properties(libopencv_java3 PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}}/../jniLibs/${ANDROID_ABI}/libopencv_java3.so)

# Set OpenCL
#add_library(libopencl SHARED IMPORTED)
#set_target_properties(libopencl PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}}/../jniLibs/${ANDROID_ABI}/libOpenCL.so)

add_library(libgles SHARED IMPORTED)
set_target_properties(libgles PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}}/../jniLibs/${ANDROID_ABI}/libGLES_mali.so)

add_library(libfoo SHARED IMPORTED )
set_target_properties(libfoo PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libfoo.so )

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/CL)

add_library(
        sift-lib
        SHARED

        GPUBase.cpp
        PyramidProcess.cpp
        GaussFilter.cpp
        DetectExtrema.cpp
        Subtract.cpp
        imgfeatures.cpp
        utils.cpp
        SiftGPU.cpp
)

add_library( # Sets the name of the library.
        native-lib
        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        native-lib.cpp)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries(
        sift-lib

        libopencv_java3
        libgles
        libfoo
)
target_link_libraries( # Specifies the target library.
        native-lib

        libopencv_java3
        libgles
        libfoo
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})