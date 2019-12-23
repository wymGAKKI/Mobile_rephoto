//
// Created by admin on 2019/12/19.
//


#include <android/log.h>
#include "sift_cl/DetectExtrema.h"



DetectExtrema::~DetectExtrema(void)
{
}

DetectExtrema::DetectExtrema(int _maxNumberKeys): GPUBase( DETECT_EXTREMA_OPENCL_SOURCE, DETECT_EXTREMA_OPENCL_KERNEL)
{
    int counter = 0;
    maxNumberKeys = _maxNumberKeys;

    cmDevBufNumber = clCreateBuffer(GPUContext, CL_MEM_READ_WRITE, sizeof(int), NULL, &GPUError);
    CheckError(GPUError);

    cmDevBufCount = clCreateBuffer(GPUContext, CL_MEM_READ_WRITE, sizeof(int), NULL, &GPUError);
    CheckError(GPUError);
    GPUError = clEnqueueWriteBuffer(GPUCommandQueue, cmDevBufCount, CL_TRUE, 0, sizeof(int), (void*)&counter, 0, NULL, NULL);
    CheckError(GPUError);

    cmDevBufKeys = clCreateBuffer(GPUContext, CL_MEM_READ_WRITE, maxNumberKeys*sizeof(Keys), NULL, &GPUError);
    CheckError(GPUError);

    GPUKernelDesc = clCreateKernel(GPUProgram, DESC_EXTREMA_OPENCL_KERNEL, &GPUError);
    CheckError(GPUError);
}


bool DetectExtrema::Process(cl_mem dogPyr, cl_mem gaussPyr, int imageWidth, int imageHeight, int OffsetPrev , int OffsetAct, int OffsetNext, int* numExtr, float prelim_contr_thr, int intvl, int octv, Keys* keys)
{
    int counter = 0;
    int numberExtr = 0;

    OffsetAct = OffsetAct / 4;
    OffsetPrev = OffsetPrev / 4;
    OffsetNext = OffsetNext / 4;

    GPUError = clEnqueueWriteBuffer(GPUCommandQueue, cmDevBufNumber, CL_TRUE, 0, sizeof(int), (void*)&numberExtr, 0, NULL, NULL);
    CheckError(GPUError);

    size_t GPULocalWorkSize[2];
    GPULocalWorkSize[0] = iBlockDimX;
    GPULocalWorkSize[1] = iBlockDimY;
    GPUGlobalWorkSize[0] = RoundUpGroupDim((int)GPULocalWorkSize[0], (int)imageWidth);
    GPUGlobalWorkSize[1] = RoundUpGroupDim((int)GPULocalWorkSize[1], (int)imageHeight);

    int iLocalPixPitch = iBlockDimX + 2;
    GPUError = clSetKernelArg(GPUKernel, 0, sizeof(cl_mem), (void*)&dogPyr);
    GPUError |= clSetKernelArg(GPUKernel, 1, sizeof(cl_mem), (void*)&cmDevBufKeys);
    GPUError |= clSetKernelArg(GPUKernel, 2, sizeof(cl_mem), (void*)&cmDevBufNumber);
    GPUError |= clSetKernelArg(GPUKernel, 3, sizeof(cl_int), (void*)&OffsetPrev);
    GPUError |= clSetKernelArg(GPUKernel, 4, sizeof(cl_int), (void*)&OffsetAct);
    GPUError |= clSetKernelArg(GPUKernel, 5, sizeof(cl_int), (void*)&OffsetNext);
    GPUError |= clSetKernelArg(GPUKernel, 6, sizeof(cl_int), (void*)&imageWidth);
    GPUError |= clSetKernelArg(GPUKernel, 7, sizeof(cl_int), (void*)&imageHeight);
    GPUError |= clSetKernelArg(GPUKernel, 8, sizeof(cl_float), (void*)&prelim_contr_thr);
    GPUError |= clSetKernelArg(GPUKernel, 9, sizeof(cl_int), (void*)&intvl);
    GPUError |= clSetKernelArg(GPUKernel, 10, sizeof(cl_int), (void*)&octv);

    if(GPUError) return false;

    if(clEnqueueNDRangeKernel( GPUCommandQueue, GPUKernel, 2, NULL, GPUGlobalWorkSize, GPULocalWorkSize, 0, NULL, NULL))
    {
        cout << "Error clEnqueueNDRangeKernel" << endl;
        return false;
    }

    GPUError = clEnqueueReadBuffer(GPUCommandQueue, cmDevBufNumber, CL_TRUE, 0, sizeof(int), (void*)&numberExtr, 0, NULL, NULL);
    CheckError(GPUError);

    GPUError = clEnqueueWriteBuffer(GPUCommandQueue, cmDevBufCount, CL_TRUE, 0, sizeof(int), (void*)&counter, 0, NULL, NULL);
    CheckError(GPUError);

    float sqrtNuber = sqrt((float)numberExtr);

    GPULocalWorkSize[0] = iBlockDimX;
    GPULocalWorkSize[1] = iBlockDimY;
    GPUGlobalWorkSize[0] = RoundUpGroupDim((int)GPULocalWorkSize[0], (int)(sqrtNuber+1));
    GPUGlobalWorkSize[1] = RoundUpGroupDim((int)GPULocalWorkSize[1], (int)sqrtNuber);

    iLocalPixPitch = iBlockDimX + 2;
    GPUError = clSetKernelArg(GPUKernelDesc, 0, sizeof(cl_mem), (void*)&gaussPyr);
    GPUError |= clSetKernelArg(GPUKernelDesc, 1, sizeof(cl_uint), (void*)&OffsetPrev);
    GPUError |= clSetKernelArg(GPUKernelDesc, 2, sizeof(cl_mem), (void*)&cmDevBufCount);
    GPUError |= clSetKernelArg(GPUKernelDesc, 3, sizeof(cl_mem), (void*)&cmDevBufKeys);
    GPUError |= clSetKernelArg(GPUKernelDesc, 4, sizeof(cl_int), (void*)&imageWidth);
    GPUError |= clSetKernelArg(GPUKernelDesc, 5, sizeof(cl_int), (void*)&imageHeight);
    GPUError |= clSetKernelArg(GPUKernelDesc, 6, sizeof(cl_float), (void*)&prelim_contr_thr);
    GPUError |= clSetKernelArg(GPUKernelDesc, 7, sizeof(cl_int), (void*)&intvl);
    GPUError |= clSetKernelArg(GPUKernelDesc, 8, sizeof(cl_int), (void*)&octv);
    GPUError |= clSetKernelArg(GPUKernelDesc, 9, sizeof(cl_mem), (void*)&cmDevBufNumber);
    if(GPUError) return false;

    if(clEnqueueNDRangeKernel( GPUCommandQueue, GPUKernelDesc, 2, NULL, GPUGlobalWorkSize, GPULocalWorkSize, 0, NULL, NULL))
    {
        cout << "Error clEnqueueNDRangeKernel" << endl;
        return false;
    }

    GPUError = clEnqueueReadBuffer(GPUCommandQueue, cmDevBufCount, CL_TRUE, 0, sizeof(int), (void*)&counter, 0, NULL, NULL);
    CheckError(GPUError);

    GPUError = clEnqueueReadBuffer(GPUCommandQueue, cmDevBufKeys, CL_TRUE, 0, maxNumberKeys*sizeof(Keys), (void*)keys, 0, NULL, NULL);
    CheckError(GPUError);

    *numExtr = numberExtr;

    return true;
}