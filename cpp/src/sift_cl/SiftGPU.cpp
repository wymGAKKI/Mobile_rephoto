//
// Created by admin on 2019/12/19.
//
#include "sift_cl/imgfeatures.h"
#include "sift_cl/utils.h"
#include "sift_cl/SiftGPU.h"
#include <opencv2/opencv.hpp>
#include <android/log.h>
#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc_c.h>


SiftGPU::SiftGPU(int height, int width, int _intvls, float _sigma, float _contr_thr, int _curv_thr, int _descr_width, int _descr_hist_bins, int _img_dbl)
{
    intvls = _intvls;
    sigma = _sigma;
    contr_thr = _contr_thr;
    curv_thr = _curv_thr;
    descr_width = _descr_width;
    descr_hist_bins = _descr_hist_bins;
    img_dbl = _img_dbl;
    total = 0;
    SizeOfPyramid = 0;

    /*intvls = SIFT_INTVLS;
    sigma = SIFT_SIGMA;
    contr_thr = SIFT_CONTR_THR;
    curv_thr = SIFT_CURV_THR;
    img_dbl = SIFT_IMG_DBL;
    descr_width = SIFT_DESCR_WIDTH;
    descr_hist_bins = SIFT_DESCR_HIST_BINS;*/

    sig = (float*)calloc( intvls + 3, sizeof(float));
    gaussFilter = new GaussFilter();
    subtract = new Subtract();
    detectExt = new DetectExtrema(SIFT_MAX_NUMBER_KEYS);

    float k;
    int intvlsSum = intvls + 3;
    float sig_total, sig_prev;
    int cur_h = height * 2;
    int cur_w = width * 2;

    sig[0] = sigma;
    k = pow( 2.0, 1.0 / intvls );


    for(int i = 1; i < intvlsSum; i++ )
    {
        sig_prev = pow( k, i - 1 ) * sigma;
        sig_total = sig_prev * k;
        sig[i] = sqrt( sig_total * sig_total - sig_prev * sig_prev );
    }

    octvs = log( (float)MIN( cur_h,  cur_w ) ) / log((float)2) - 2;
    sizeOfImages = new int[octvs];
    imageWidth = new int[octvs];
    imageHeight = new int[octvs];

    sizeOfImages[0] = sizeof(float)* cur_h * cur_w;
    SizeOfPyramid += sizeOfImages[0] * intvlsSum;
    imageHeight[0] = cur_h;
    imageWidth[0] = cur_w;

    for(int o = 1; o < octvs; o++ )
    {
        cur_h /= 2;
        cur_w /= 2;
        sizeOfImages[o] = sizeof(float)* cur_h * cur_w;
        SizeOfPyramid += sizeOfImages[o] * intvlsSum;
        imageHeight[o] = cur_h;
        imageWidth[o] = cur_w;
    }

    gaussFilter->CreateBufferForPyramid(SizeOfPyramid);
    subtract->CreateBufferForPyramid(SizeOfPyramid);
}

/*
Compares features for a decreasing-scale ordering.  Intended for use with
CvSeqSort
@param feat1 first feature
@param feat2 second feature
@param param unused
@return Returns 1 if feat1's scale is greater than feat2's, -1 if vice versa,
and 0 if their scales are equal
*/
int FeatureCmp( void* feat1, void* feat2, void* param )
{
    feature* f1 = (feature*) feat1;
    feature* f2 = (feature*) feat2;

    if( f1->scl < f2->scl )
        return 1;
    if( f1->scl > f2->scl )
        return -1;
    return 0;
}

int SiftGPU::DoSift(cv::Mat &img)
{
    //printf("\n ----------- DoSift START --------------- \n");

    cv::Mat init_img;
    CvSeq* features;


    init_img = CreateInitialImg( img, img_dbl, sigma );

    BuildGaussPyramid(init_img);

    storage = cvCreateMemStorage( 0 );

    double duration = 0;
    features = DetectAndGenerateDesc();

    cvSeqSort( features, (CvCmpFunc)FeatureCmp, NULL );
    total = features->total;
    feat = (feature*)calloc(total, sizeof(feature));
    feat = (feature*)cvCvtSeqToArray( features, feat, CV_WHOLE_SEQ );

    kpts.resize(total);
    descriptor.create(total, 128, CV_32F);

    for(int i = 0; i < total; i++ )
    {
        free( feat[i].feature_data );
        feat[i].feature_data = NULL;
        kpts[i] = cv::KeyPoint(feat[i].x, feat[i].y, feat[i].ori, feat[i].scl);
        for (int j = 0; j < 128; j++) {
            descriptor.at<float>(i, j) = feat[i].descr[j];
        }

    }

    free(feat);
    cvReleaseMemStorage( &storage );

    return total;
}




/*
Builds Gaussian scale space pyramid from an image
@param base base image of the pyramid
@param octvs number of octaves of scale space
@param intvls number of intervals per octave
@param sigma amount of Gaussian smoothing per octave
@return Returns a Gaussian scale space pyramid as an octvs x (intvls + 3) array
*/
bool SiftGPU::BuildGaussPyramid(cv::Mat &base)
{
    int intvlsSum = intvls + 3;

    std::vector<cv::Mat> pyr;
    pyr.resize(octvs);


    pyr[0] = base.clone();

    int offset = 0;

    /*for( o = 0; o < octvs; o++ )
    {
        for( i = 0; i < intvlsSum; i++ )
        {
            if( o == 0  &&  i == 0 )
            {
                GaussFilter->SendImageToBufPyramid(imgArray[o], offset);
            } else if(i == 0)
            {
                imgArray[o] = Downsample( imgArray[o-1] );
                GaussFilter->SendImageToBufPyramid(imgArray[o], offset);
            }
            offset += sizeOfImages[o];
        }
    }*/

    offset = 0;
    int OffsetAct = 0;
    int OffsetPrev = 0;

    for(int o = 0; o < octvs; o++ )
    {
        for(int i = 0; i < intvlsSum; i++ )
        {

            if( o == 0  &&  i == 0 )
            {
                gaussFilter->SendImageToPyramid(pyr[o], OffsetAct);
            } else if(i == 0)
            {
                gaussFilter->ReceiveImageFromPyramid(pyr[o-1], OffsetPrev);
                cv::resize(pyr[o-1], pyr[o], cv::Size(pyr[o-1].cols/2, pyr[o-1].rows/2), 0, 0, cv::INTER_NEAREST);
                gaussFilter->SendImageToPyramid(pyr[o], OffsetAct);
            }

            if(i > 0 )
            {
                gaussFilter->Process( sig[i], imageWidth[o], imageHeight[o], OffsetPrev, OffsetAct);
                subtract->Process(gaussFilter->cmBufPyramid, imageWidth[o], imageHeight[o], OffsetPrev, OffsetAct);
            }
            OffsetPrev = OffsetAct;
            OffsetAct += sizeOfImages[o];
        }
    }

    /*
    OffsetPrev = 0;
    for(int o = 0; o < octvs; o++ )
    {
    	for(int i = 0; i < intvlsSum; i++ )
    	{
    	    __android_log_print(ANDROID_LOG_INFO, "build pyr", "size %d channel %d", pyr[o].rows * pyr[o].cols, pyr[o].channels());
    		subtract->ReceiveImageFromPyramid(pyr[o], OffsetPrev);
    		std::string num = std::to_string(o * intvlsSum + i);
    		pyr[o]*=255;
    		//cv::Mat now = cv::cvarrToMat(imgArray[o])*255;
    	    cv::imwrite("/storage/emulated/0/" + num + ".png",pyr[o]);
    		OffsetPrev += sizeOfImages[o];
    	}
    }
     */


    return true;
}


/*
Downsamples an image to a quarter of its size (half in each dimension)
using nearest-neighbor interpolation
@param img an image
@return Returns an image whose dimensions are half those of img
*/
IplImage* SiftGPU::Downsample( IplImage* img )
{
    int width = img->width / 2;
    int height = img->height / 2;

    if( width < 50 || height < 50 )
    {
        width = width*2;
        height = height*2;
    }
    IplImage* smaller = cvCreateImage( cvSize( width, height),
                                       img->depth, img->nChannels );
    cvResize( img, smaller, CV_INTER_NN );

    return smaller;
}



CvSeq* SiftGPU::DetectAndGenerateDesc()
{
    float prelim_contr_thr = 0.5 * contr_thr / intvls;
    struct detection_data* ddata;
    int o, i, r, c;
    int num=0;				// Number of keypoins detected
    int numRemoved=0;		// The number of key points rejected because they failed a test
    int numberExtrema = 0;
    int number = 0;

    CvSeq* features = cvCreateSeq( 0, sizeof(CvSeq), sizeof(feature), storage );
    total = features->total;
    int intvlsSum = intvls + 3;
    int OffsetAct = 0;
    int OffsetNext = 0;
    int OffsetPrev = 0;



    Keys keysArray[SIFT_MAX_NUMBER_KEYS];
    /*for (int j = 0 ; j < SIFT_MAX_NUMBER_KEYS ; j++)
    {
        keysArray[j].x = 0.0;
        keysArray[j].y = 0.0;
        keysArray[j].intvl = 0.0;
        keysArray[j].octv = 0.0;
        keysArray[j].subintvl = 0.0;
        keysArray[j].scx = 0.0;
        keysArray[j].scy = 0.0;
        keysArray[j].ori = 0.0;
    }*/


    int count = 0;
    for( o = 0; o < octvs; o++ )
    {
        for( i = 0; i < intvlsSum; i++ )
        {
            OffsetNext += sizeOfImages[o];

            if( i > 0 && i <= intvls )
            {
                num = 0;
                detectExt->Process(subtract->cmBufPyramid, gaussFilter->cmBufPyramid, imageWidth[o], imageHeight[o], OffsetPrev, OffsetAct, OffsetNext, &num, prelim_contr_thr, i, o, keysArray);
                total = features->total;
                number = num;
                struct detection_data* ddata;

                for(int ik = 0; ik < number ; ik++)
                {
                    feat = NewFeature();
                    ddata = FeatDetectionData( feat );
                    feat->img_pt.x = feat->x = keysArray[ik].scx;
                    feat->img_pt.y = feat->y = keysArray[ik].scy;
                    ddata->r = keysArray[ik].y;
                    ddata->c = keysArray[ik].x;
                    ddata->subintvl = keysArray[ik].subintvl;
                    ddata->octv = keysArray[ik].octv;
                    ddata->intvl = keysArray[ik].intvl;
                    feat->scl = keysArray[ik].scl;
                    ddata->scl_octv = keysArray[ik].scl_octv;
                    feat->ori = (double)keysArray[ik].ori;
                    feat->d = 128;


                    for(int i = 0; i < 128 ; i++ )
                    {
                        //descriptor.at<float>(count, i) = keysArray[ik].desc[i];
                        feat->descr[i] = keysArray[ik].desc[i];
                    }
                    cvSeqPush( features, feat );
                    free( feat );
                    count++;
                }
            }
            OffsetPrev = OffsetAct;
            OffsetAct += sizeOfImages[o];
        }
    }


    return features;
}


/*
Allocates and initializes a new feature
@return Returns a pointer to the new feature
*/
feature* SiftGPU::NewFeature()
{
    feature* feat;
    struct detection_data* ddata;

    feat = (feature*)malloc( sizeof( feature ) );
    memset( feat, 0, sizeof( feature ) );
    ddata = (detection_data*)malloc( sizeof( struct detection_data ) );
    memset( ddata, 0, sizeof( struct detection_data ) );
    feat->feature_data = ddata;
    feat->type = FEATURE_LOWE;

    return feat;
}


/*
Converts an image to 8-bit grayscale and Gaussian-smooths it.  The image is
optionally doubled in size prior to smoothing.
@param img input image
@param img_dbl if true, image is doubled in size prior to smoothing
@param sigma total std of Gaussian smoothing
*/
cv::Mat SiftGPU::CreateInitialImg( const cv::Mat& img, int img_dbl, float sigma )
{
    cv::Mat gray, gray_fpt;
    if( img.channels() == 3 || img.channels() == 4 )
        cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    else
        img.copyTo(gray);
    gray.convertTo(gray_fpt, cv::DataType<float>::type, 1, 0);
    gray_fpt/=255;

    float sig_diff;

    if( img_dbl )
    {
        sig_diff = sqrtf( std::max(sigma * sigma - 0.5 * 0.5 * 4, 0.01) );
        cv::Mat dbl;
        cv::resize(gray_fpt, dbl, cv::Size(gray.cols*2, gray.rows*2), 0, 0, cv::INTER_LINEAR);
        GaussianBlur(dbl, dbl, cv::Size(), sig_diff, sig_diff);
        return dbl;
    }
    else
    {
        sig_diff = sqrtf( std::max(sigma * sigma - SIFT_INIT_SIGMA * SIFT_INIT_SIGMA, 0.01) );
        GaussianBlur(gray_fpt, gray_fpt, cv::Size(), sig_diff, sig_diff);
        return gray_fpt;
    }
}

/*
Converts an image to 32-bit grayscale
@param img a 3-channel 8-bit color (BGR) or 8-bit gray image
@return Returns a 32-bit grayscale image
*/
IplImage* SiftGPU::ConvertToGray32( IplImage* img )
{
    IplImage* gray8, * gray32;

    gray32 = cvCreateImage( cvGetSize(img), 32, 1 );
    if( img->nChannels == 1 )
        gray8 = (IplImage*)cvClone( img );
    else
    {
        gray8 = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 1 );
        cvCvtColor( img, gray8, CV_BGR2GRAY );
    }
    cvConvertScale( gray8, gray32, 1.0 / 255.0, 0 );

    cvReleaseImage( &gray8 );
    return gray32;
}
