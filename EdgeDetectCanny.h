/***************************************************************************

Copyright (c) 2014, Helder C. R. de Oliveira
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

* Neither the name of the {organization} nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

***************************************************************************/

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
