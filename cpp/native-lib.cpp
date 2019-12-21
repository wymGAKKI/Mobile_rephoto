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

void sift_match(const char* im_name1, const char* im_name2) {
    float start = clock();

    IplImage *im1 = cvLoadImage(im_name1);
    IplImage *im2 = cvLoadImage(im_name2);

    SiftGPU* siftOpenCL = new SiftGPU(SIFT_INTVLS, SIFT_SIGMA, SIFT_CONTR_THR, SIFT_CURV_THR, SIFT_DESCR_WIDTH, SIFT_DESCR_HIST_BINS, SIFT_IMG_DBL);

    int feature_num1 = siftOpenCL->DoSift(im1);
    vector<cv::KeyPoint> kpts1 = siftOpenCL->kpts;
    cv::Mat desc1 = siftOpenCL->descriptor;

    int feature_num2 = siftOpenCL->DoSift(im2);
    vector<cv::KeyPoint> kpts2 = siftOpenCL->kpts;
    cv::Mat desc2 = siftOpenCL->descriptor;

    cv::BFMatcher matcher(cv::NORM_HAMMING);
    std::vector<std::vector<cv::DMatch>> knnMatches;

    matcher.knnMatch(desc1, desc2, knnMatches, 2);

    vector<cv::DMatch> matches;

    matches.clear();

    for (auto match : knnMatches) {
        float ratio = match[0].distance / match[1].distance;
        if (ratio < 0.8) {
            matches.push_back(match[0]);
        }
    }

    float end = clock();
    cv::Mat img1 = cv::imread(im_name1);
    cv::Mat img2 = cv::imread(im_name2);
    cv::Mat img3;
    cv::drawMatches(img1, kpts1, img2, kpts2, matches, img3);
    cv::imwrite("/storage/emulated/0/test.jpg", img3);
    __android_log_print(ANDROID_LOG_INFO, "sift-test", "img size: %d*%d, feature num: %d %d, match num: %d, time: %f", im1->width, im1->height, feature_num1, feature_num2, matches.size(), (end - start) / CLOCKS_PER_SEC);
}

void sift_test(const char* imagename) {
    float start = clock();

    IplImage *img = cvLoadImage(imagename);
    CvSize size;
    size.width = img->width/2;
    size.height = img->height/2;
    IplImage *img2 = cvCreateImage(size, img->depth, img->nChannels);
    cvResize(img, img2);

    SiftGPU* siftOpenCL = new SiftGPU(SIFT_INTVLS, SIFT_SIGMA, SIFT_CONTR_THR, SIFT_CURV_THR, SIFT_DESCR_WIDTH, SIFT_DESCR_HIST_BINS, SIFT_IMG_DBL);

    int feature_num = siftOpenCL->DoSift(img2);

    float end = clock();
    __android_log_print(ANDROID_LOG_INFO, "sift-test", "img size: %d*%d, feature num: %d, time: %f", img2->width, img2->height, feature_num, (end - start) / CLOCKS_PER_SEC);
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
    sift_match(imagename, imagename);
    //cvSaveImage("/storage/emulated/0/test.jpg", outputImg);
	return env->NewStringUTF("test");
}

