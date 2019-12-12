#include <jni.h>
#include <string>
#include <opencv2/opencv.hpp>

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_rephoto_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_wu6shen_rephoto_MainActivity_alphaBlend(JNIEnv *env, jobject instance, jlong src1,
                                                         jlong src2, jlong result) {

    // TODO
    cv::Mat *ptr1 = (cv::Mat *) src1;
    cv::Mat *ptr2 = (cv::Mat *) src2;
    cv::Mat *ptr3 = (cv::Mat *) result;
    (*ptr3) = 0.7 * (*ptr2) + 0.3 * (*ptr1);

    cv::resize((*ptr3), (*ptr3), cv::Size(1080, 1920));

}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_rephoto_CameraView_alphaBlend(JNIEnv *env, jobject thiz, jlong src1, jlong src2,
                                               jlong result) {
    // TODO: implement alphaBlend()
    cv::Mat *ptr1 = (cv::Mat *)src1;
    cv::Mat *ptr2 = (cv::Mat *)src2;
    cv::Mat *ptr3 = (cv::Mat *)result;

    (*ptr3) = (*ptr2) * 0.7 + (*ptr1) * 0.3;
}