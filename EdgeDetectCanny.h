#ifndef EDGEDETECTCANNY_H
#define EDGEDETECTCANNY_H

#include "EdgeDetectCommon.h"
#include "OpCommons.h"

/* Scale floating point magnitudes and angles to 8 bits */
#define ORI_SCALE 40.0
#define MAG_SCALE 20.0

/* Biggest possible filter mask */
#define MAX_MASK_SIZE 20

class EdgeDetectCanny: public EdgeDetectCommon
{
public:
    EdgeDetectCanny(QImage *img, int dP, int thresLow, int thresHigh);
private:
    /* Fraction of pixels that should be above the HIGH threshold */
    float ratio;
    int WIDTH;

    int trace (int i, int j, int low, QImage *im, QImage *mag, QImage *ori);
    float dGauss (float x, float sigma);
    void hysteresis (int high, int low, QImage *im, QImage *mag, QImage *oriim);
    void canny (float desvPadrao, QImage *im, QImage *mag, QImage *ori);
    void seperable_convolution (QImage *im, float *gau, int width,
                                float **smx, float **smy);
    void dxy_seperable_convolution (float **im, int nr, int nc, float *gau,
                                    int width, float **sm, int which);
    void nonmax_suppress (float **dx, float **dy, QImage *mag, QImage *ori);
    void estimate_thresh (QImage *mag, int *hi, int *low);

};

#endif // EDGEDETECTCANNY_H
