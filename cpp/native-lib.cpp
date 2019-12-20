#include <jni.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/core/ocl.hpp>
#include <CL/cl.h>
#include <dlfcn.h>
#include <android/log.h>
#include "GPUBase.h"
#include "SiftGPU.h"
#include "opencl_test.h"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO , "REPHOTO", __VA_ARGS__)

void Init_OpenCL();

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_rephoto_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {

    std::string hello = "Hello from C++";
    //Init_OpenCL();
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
Java_com_example_rephoto_CameraView_calAlphaBlend(JNIEnv *env, jobject thiz, jlong src1, jlong src2,
                                               jlong result) {
    // TODO: implement alphaBlend()
    cv::Mat *ptr1 = (cv::Mat *)src1;
    cv::Mat *ptr2 = (cv::Mat *)src2;
    cv::Mat *ptr3 = (cv::Mat *)result;

    (*ptr3) = (*ptr2) * 0.7 + (*ptr1) * 0.3;
}

void sift_test(const char* imagename) {
    IplImage *img = cvLoadImage(imagename);
    CvSize size;
    size.width = img->width/2;
    size.height = img->height/2;
    IplImage *img2 = cvCreateImage(size, img->depth, img->nChannels);
    cvResize(img, img2);
    SiftGPU* siftOpenCL = new SiftGPU(SIFT_INTVLS, SIFT_SIGMA, SIFT_CONTR_THR, SIFT_CURV_THR, SIFT_DESCR_WIDTH, SIFT_DESCR_HIST_BINS, SIFT_IMG_DBL);
    float start = clock();
    int feature_num = siftOpenCL->DoSift(img2);
    float end = clock();
    __android_log_print(ANDROID_LOG_INFO, "sift-test", "img size: %d*%d, feature num: %d, time: %f", img->width, img->height, feature_num, (end - start) / CLOCKS_PER_SEC);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_rephoto_CameraView_sobelFilter(JNIEnv *env, jobject thiz, jstring image_path) {
	// TODO: implement sobelFilter()

	char *clImagePath = (char *) (env)->GetStringUTFChars(image_path, 0);
	const char* imagename = clImagePath;
	/**
	IplImage* inputImg = cvLoadImage(imagename);
    CvSize size;
    size.width = inputImg->width;
    size.height = inputImg->height;
    __android_log_print(ANDROID_LOG_INFO, "sobelfilter", "width %d %d", size.width, size.height);

    IplImage* inputImg_gray = cvCreateImage(size, IPL_DEPTH_8U, 1);
    cvCvtColor(inputImg, inputImg_gray, CV_BGR2GRAY);
    IplImage* outputImg = cvCreateImage(size, IPL_DEPTH_8U, 1);

    GPU_Sobel(inputImg_gray, outputImg);
	 */
    sift_test(imagename);
    //cvSaveImage("/storage/emulated/0/test.jpg", outputImg);
	return env->NewStringUTF("test");
}

