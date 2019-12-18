#include <jni.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/core/ocl.hpp>
#include <CL/cl.h>
#include <dlfcn.h>
#include <android/log.h>

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

/**
cl_uint num_device;
cl_uint num_platform;
cl_platform_id *platform;
cl_device_id *devices;
cl_int err;
cl_context context;
cl_command_queue cmdQueue;
cl_mem buffer,sum_buffer;
cl_program program ;
cl_kernel kernel;


void Init_OpenCL() {

        size_t nameLen1;
        char platformName[1024];

        err = clGetPlatformIDs(0, 0, &num_platform);
        platform = (cl_platform_id *) malloc(sizeof(cl_platform_id) * num_platform);
        err = clGetPlatformIDs(num_platform, platform, NULL);

        err = clGetDeviceIDs(platform[0], CL_DEVICE_TYPE_GPU, 0, NULL, &num_device);
        devices = (cl_device_id *) malloc(sizeof(cl_device_id) * num_device);
        err = clGetDeviceIDs(platform[0], CL_DEVICE_TYPE_GPU, num_device, devices, NULL);
}
 */


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



#define N 20	//这里的N是sobel滤波的阈值
#define  KERNEL_SRC "\n" \
	"			__kernel void Sobel(__global char *array1, __global char *array2, __global int *array3)		\n "\
	"			{																							\n "\
	"				size_t gidx = get_global_id(0);															\n "\
	"				size_t gidy = get_global_id(1);															\n "\
	"				unsigned char a00, a01, a02;															\n "\
	"				unsigned char a10, a11, a12;															\n "\
	"				unsigned char a20, a21, a22;															\n "\
	"				int width=array3[0];																	\n "\
	"				int heigh=array3[1];																	\n "\
	"				int widthStep=array3[2];																\n "\
	"				if(gidy>0&&gidy<heigh-1&&gidx>0&&gidx<width-1)											\n "\
	"				{																						\n "\
	"					a00 = array1[gidx-1+widthStep*(gidy-1)];											\n "\
	"					a01 = array1[gidx+widthStep*(gidy-1)];												\n "\
	"					a02 = array1[gidx+1+widthStep*(gidy-1)];											\n "\
	"					a10 = array1[gidx-1+widthStep*gidy];												\n "\
	"					a11 = array1[gidx+widthStep*gidy];													\n "\
	"					a12 = array1[gidx+1+widthStep*gidy];												\n "\
	"					a20 = array1[gidx-1+widthStep*(gidy+1)];											\n "\
	"					a21 = array1[gidx+widthStep*(gidy+1)];												\n "\
	"					a22 = array1[gidx+1+widthStep*(gidy+1)];											\n "\
	"					float ux=a20+2*a21+a22-a00-2*a01-a02;												\n "\
	"					float uy=a02+2*a12+a22-a00-2*a10-a20;												\n "\
	"					//array2[gidx+width*gidy]=sqrt(ux*ux + uy*uy);										\n "\
	"					float u=sqrt(ux*ux + uy*uy);														\n "\
	"					if(u>255) {																			\n "\
	"						u=-1;																			\n "\
	"					} else if(u<20) {																	\n "\
	"						u=0;																			\n "\
	"					}																					\n "\
	"					array2[gidx+widthStep*gidy]=u;														\n "\
	"				}																						\n "\
"}"


int GPU_Sobel(IplImage* inputImg_gray, IplImage* outputImg);
double difference(IplImage* input1, IplImage* input2, IplImage* output);

int GPU_Sobel(IplImage* inputImg_gray, IplImage* outputImg) {

	//initFns();
	__android_log_print(ANDROID_LOG_INFO, "opencl", "test-0");
	cl_uint numPlatforms; //the NO. of platforms
	cl_platform_id platform = NULL; //the chosen platform
	cl_int status;
	cl_platform_id* platforms;
	cl_uint numDevices = 0;
	cl_device_id *devices;
	cl_context context;
	cl_command_queue commandQueue;
	cl_program program;
	cl_kernel kernel;
	//size_t global;
	cl_mem a1, a2, a3;

	int IMG_WIDTH = inputImg_gray->width;
	int IMG_HEIGHT = inputImg_gray->height;

	long IN_DATA_SIZE = inputImg_gray->widthStep * IMG_HEIGHT;
	long OUT_DATA_SIZE = outputImg->widthStep * IMG_HEIGHT;
	//unsigned char *inputData1;
	//inputData1=(unsigned char *)malloc(DATA_SIZE*sizeof(unsigned char));
	char *inputData1;
	inputData1 = inputImg_gray->imageData;

	//float *outputData;
	//outputData=(float *)malloc(DATA_SIZE*sizeof(float));
	char *outputData;
	outputData = outputImg->imageData;

	int inputData2[3] = { IMG_WIDTH, IMG_HEIGHT, inputImg_gray->widthStep };
	__android_log_print(ANDROID_LOG_INFO, "opencl", "test-1");

	status = clGetPlatformIDs(0, NULL, &numPlatforms);

	__android_log_print(ANDROID_LOG_INFO, "opencl", "test-a");

	if (numPlatforms > 0) {
		platforms = (cl_platform_id*) malloc(
				numPlatforms * sizeof(cl_platform_id));
		status = clGetPlatformIDs(numPlatforms, platforms, NULL);
		platform = platforms[0];
		free(platforms);
	}
	__android_log_print(ANDROID_LOG_INFO, "opencl", "test-b");

	status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &numDevices);
//	if (numDevices == 0) //no GPU available.
//			{
//		printf("No GPU device available.\n");
//		printf("Choose CPU as default device.\n");
//		status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 0, NULL,
//				&numDevices);
//		devices = (cl_device_id*) malloc(numDevices * sizeof(cl_device_id));
//		status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, numDevices,
//				devices, NULL);
//	} else {
//		devices = (cl_device_id*) malloc(numDevices * sizeof(cl_device_id));
//		status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, numDevices,
//				devices, NULL);
//	}
	devices = (cl_device_id*) malloc(numDevices * sizeof(cl_device_id));
	status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, numDevices, devices,
							NULL);

	context = clCreateContext(NULL, 1, devices, NULL, NULL, &status);

	commandQueue = clCreateCommandQueue(context, devices[0], 0, &status);

	const char *source = KERNEL_SRC;
	size_t sourceSize[] = { strlen(source) };
	program = clCreateProgramWithSource(context, 1, &source, sourceSize,
										&status);

	status = clBuildProgram(program, 1, devices, NULL, NULL, NULL);
	__android_log_print(ANDROID_LOG_INFO, "opencl", "test-c");

	a1 = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
						sizeof(unsigned char) * IN_DATA_SIZE, inputData1, &status);

	a2 = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
						sizeof(unsigned char) * OUT_DATA_SIZE, outputData, &status);

	a3 = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
						sizeof(int) * 3, inputData2, &status);

	kernel = clCreateKernel(program, "Sobel", &status);
	__android_log_print(ANDROID_LOG_INFO, "opencl", "test-d");

	// set the argument list for the kernel command
	status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &a1);
	status = clSetKernelArg(kernel, 1, sizeof(cl_mem), &a2);
	status = clSetKernelArg(kernel, 2, sizeof(cl_mem), &a3);

	size_t local[] = { 11, 11 };
	size_t global[2];
	global[0] = (
			IMG_WIDTH % local[0] == 0 ?
			IMG_WIDTH : (IMG_WIDTH + local[0] - IMG_WIDTH % local[0]));
	global[1] =
			(IMG_HEIGHT % local[1] == 0 ?
			 IMG_HEIGHT : (IMG_HEIGHT + local[1] - IMG_HEIGHT % local[1]));
	__android_log_print(ANDROID_LOG_INFO, "opencl", "test-e");
//	size_t local[] = { 2, 2 };
//		size_t global[]={10,10};

	// enqueue the kernel command for execution
	status = clEnqueueNDRangeKernel(commandQueue, kernel, 2, NULL, global,
									local, 0, NULL, NULL);
	if (status != 0)
		return status;
	clFinish(commandQueue);

	clEnqueueReadBuffer(commandQueue, a2, CL_TRUE, 0,
						sizeof(unsigned char) * OUT_DATA_SIZE, outputData, 0, NULL, NULL);

	clReleaseMemObject(a1);
	clReleaseMemObject(a2);
	clReleaseMemObject(a3);
	clReleaseProgram(program);
	clReleaseKernel(kernel);
	clReleaseCommandQueue(commandQueue);
	clReleaseContext(context);
	__android_log_print(ANDROID_LOG_INFO, "opencl", "test-f");

	return 0;
}

double difference(IplImage* input1, IplImage* input2, IplImage* output) {
	unsigned char a;
	double re;
	int k = 0;
	for (int i = 0; i < input1->height; i++) {
		for (int j = 0; j < input1->width; j++) {
			a = input1->imageData[j + input1->widthStep * i]
				- input2->imageData[j + input2->widthStep * i];
			output->imageData[j + output->widthStep * i] = a;
			if (a != 0) {
				k++;
			}
		}
	}
	printf("差异点：%d\n", k);
	re = (double) k / (double) (input1->height * input1->width);
	return re;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_rephoto_CameraView_sobelFilter(JNIEnv *env, jobject thiz, jstring image_path) {
	// TODO: implement sobelFilter()
	char *clImagePath = (char *) (env)->GetStringUTFChars(image_path, 0);
	const char* imagename = clImagePath;
	IplImage* inputImg = cvLoadImage(imagename);
    CvSize size;
    size.width = inputImg->width;
    size.height = inputImg->height;
    __android_log_print(ANDROID_LOG_INFO, "sobelfilter", "width %d %d", size.width, size.height);

    IplImage* inputImg_gray = cvCreateImage(size, IPL_DEPTH_8U, 1);
    cvCvtColor(inputImg, inputImg_gray, CV_BGR2GRAY);
    IplImage* outputImg = cvCreateImage(size, IPL_DEPTH_8U, 1);

    GPU_Sobel(inputImg_gray, outputImg);
    cvSaveImage("/storage/emulated/0/test.jpg", outputImg);
	return env->NewStringUTF("test");
}
