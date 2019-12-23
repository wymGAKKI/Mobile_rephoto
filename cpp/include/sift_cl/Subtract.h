//
// Created by admin on 2019/12/19.
//

#ifndef REPHOTO_SUBTRACT_H
#define REPHOTO_SUBTRACT_H
#include "sift_cl/PyramidProcess.h"


class Subtract :
        public PyramidProcess
{
public:

    bool Process(cl_mem gaussPyr, int imageWidth, int imageHeight, int OffsetPrev, int OffsetAct);

    /*!
    * Destructor.
    */
    ~Subtract(void);

    /*!
    * Constructor.
    */
    Subtract();
};
#endif //REPHOTO_SUBTRACT_H
