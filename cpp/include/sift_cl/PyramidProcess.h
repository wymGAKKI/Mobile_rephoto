//
// Created by admin on 2019/12/19.
//

#ifndef REPHOTO_PYRAMIDPROCESS_H
#define REPHOTO_PYRAMIDPROCESS_H

#include <opencv2/core/types_c.h>
#include "sift_cl/GPUBase.h"
#include "sift_cl/Shared.h"


class PyramidProcess :
        public GPUBase
{

public:

    cl_mem cmBufPyramid;

    bool CreateBufferForPyramid(float size);

    bool ReceiveImageFromPyramid( cv::Mat &img, int offset);

    bool SendImageToPyramid(cv::Mat &img, int offset);

    /*!
    * Destructor.
    */
    ~PyramidProcess(void);

    /*!
    * Constructor.
    */
    PyramidProcess(char* source, char* KernelName);
};

#endif //REPHOTO_PYRAMIDPROCESS_H
