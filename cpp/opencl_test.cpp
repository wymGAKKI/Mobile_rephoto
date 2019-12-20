//
// Created by admin on 2019/12/20.
//

#include <android/log.h>
#include "include/opencl_test.h"

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

