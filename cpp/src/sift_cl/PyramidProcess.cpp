//
// Created by admin on 2019/12/19.
//
#include "sift_cl/PyramidProcess.h"



PyramidProcess::PyramidProcess(char* source, char* KernelName) : GPUBase(source,KernelName)
{

}


PyramidProcess::~PyramidProcess(void)
{

}

bool PyramidProcess::CreateBufferForPyramid( float size )
{
    cmBufPyramid = clCreateBuffer(GPUContext, CL_MEM_READ_WRITE, size, NULL, &GPUError);
    CheckError(GPUError);
    return true;
}


bool PyramidProcess::ReceiveImageFromPyramid( cv::Mat &img, int offset)
{
    clock_t start, finish;
    double duration = 0;
    start = clock();
    GPUError = clEnqueueReadBuffer(GPUCommandQueue, cmBufPyramid, CL_TRUE, offset, img.step[0] * img.rows, (void*)img.data, 0, NULL, NULL);
    CheckError(GPUError);
    finish = clock();
    duration = (double)(finish - start) / CLOCKS_PER_SEC;

    return true;
}

bool PyramidProcess::SendImageToPyramid( cv::Mat &img, int offset)
{
    clock_t start, finish;
    double duration = 0;
    start = clock();
    GPUError = clEnqueueWriteBuffer(GPUCommandQueue, cmBufPyramid, CL_TRUE, offset, img.step[0] * img.rows, (void*)img.data, 0, NULL, NULL);
    CheckError(GPUError);
    finish = clock();
    duration = (double)(finish - start) / CLOCKS_PER_SEC;

    return true;
}
