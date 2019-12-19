# Computational Rephotograph in Android 

## Environment

### OpenCV in Android Studio

 - Android_Opencv == 3.1.0
 - Import Module from ```${Opencv_FILE}/sdk/java```
 - Modify openCV library gradle & AndroidManifest.xml SDK version
 - Project Structure -> Dependencies add Opencv
 - Add NDK Library ```${Opencv_FILE}/sdk/native/libs```
 - Add Include File 
 - Modify CMakeLists.txt (Sync)
 
 
 ### OpenCL 
 
  - Download OpenCL-Z in phone, get version of OpenCL and position of library
  - Use adb pull the library in project```adb pull $LIBRARY_PATH```
  
    - Huawei libGLES_mali.so and dependence libraries
    - Others libOpenCL.so and dependence libraries
        
  - Modify CMakeLists.txt