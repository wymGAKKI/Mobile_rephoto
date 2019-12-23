//
// Created by admin on 2019/12/19.
//

#ifndef REPHOTO_DETECTEXTREMA_H
#define REPHOTO_DETECTEXTREMA_H

#include "sift_cl/Shared.h"
#include "sift_cl/GPUBase.h"


class DetectExtrema :
        public GPUBase
{
private:

    cl_mem cmDevBufNumber;

    int maxNumberKeys;

    cl_mem cmDevBufKeys;

    cl_mem cmDevBufCount;

    cl_kernel GPUKernelDesc;

public:

    /*!
    * Destructor.
    */
    ~DetectExtrema(void);

    /*!
    * Constructor.
    */
    DetectExtrema(int _maxNumberKeys);

    bool Process(cl_mem dogPyr, cl_mem gaussPyr, int imageWidth, int imageHeight, int OffsetPrev, int OffsetAct, int OffsetNext,int* numExtr, float prelim_contr_thr, int intvl, int octv, Keys* keys);
};
#endif //REPHOTO_DETECTEXTREMA_H
