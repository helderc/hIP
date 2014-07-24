#ifndef EDGEDETECTMARRHILL_H
#define EDGEDETECTMARRHILL_H

#include <QImage>
#include <QDebug>

#include "EdgeDetectCommon.h"
#include "OpCommons.h"

class EdgeDetectMarrHill: public EdgeDetectCommon
{
public:
    EdgeDetectMarrHill(QImage *img, int dP);


private:
    int desvioPadrao;

    void edgeDetect();
    void marr(float s, QImage *img);
    float LoG (float x, float sigma);
    float distance (float a, float b, float c, float d);
    void convolution (QImage *img, float **mask, int nr, int nc,
                      float **res, int NR, int NC);
    void zero_cross (float **lapim, QImage *img);
    void dolap (float **x, int nr, int nc, float **y);
};

#endif // EDGEDETECTMARRHILL_H
