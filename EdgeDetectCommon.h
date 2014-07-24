#ifndef EDGEDETECTCOMMON_H
#define EDGEDETECTCOMMON_H

#include <QImage>
#include <QDebug>

#include <math.h>

#include "OpCommons.h"

class EdgeDetectCommon: public OpCommons
{
public:
    EdgeDetectCommon(QImage *img);
    //EdgeDetectCommon();
    QImage *getImgPtr();
    void setImgPtr(QImage *img);

protected:
    QImage *imgOrig;

    float ** f2d (int nr, int nc);
    float gauss(float x, float sigma);
    float norm (float x, float y);
    int range (QImage *x, int i, int j);
    float meanGauss (float x, float sigma);

};

#endif // EDGEDETECTCOMMON_H
