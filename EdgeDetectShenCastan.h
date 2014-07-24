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

#ifndef EDGEDETECTSHENCASTAN_H
#define EDGEDETECTSHENCASTAN_H

#include "EdgeDetectCommon.h"
#include "OpCommons.h"

#define OUTLINE 25

class EdgeDetectShenCastan: public EdgeDetectCommon
{
public:
    EdgeDetectShenCastan(QImage *img, float ratioThresh,
                         int sizeAdapGrad, float smoothFactor,
                         int thinFactor, bool doHysteresis);


private:
    double b;
    int window_size;
    QImage edges;			/* keep track of edge points (thresholded) */
    float **lap;			/* keep track of laplacian of image */
    int nr, nc;			/* nrows, ncols */
    double low_thresh, high_thresh;	/* threshold for hysteresis */
    int do_hysteresis;
    int thinFactor;
    double ratio;

    void shen (const QImage *im, QImage *res);
    void embed (QImage *im, int width);
    void debed (QImage *im, int width);
    void apply_ISEF_horizontal (float **x, float **y, float **A, float **B,
                                int nrows, int ncols);
    void apply_ISEF_vertical (float **x, float **y, float **A, float **B,
                              int nrows, int ncols);
    void compute_ISEF (float **x, float **y, int nrows, int ncols);
    QImage compute_bli (float **buff1, float **buff2, int nrows, int ncols);
    float compute_adaptive_gradient (QImage BLI_buffer, float **orig_buffer,
                                     int row, int col);
    int is_candidate_edge (QImage buff, float **orig, int row, int col);
    void locate_zero_crossings (float **orig, float **smoothed, QImage bli,
                           int nrows, int ncols);
    int mark_connected (int i, int j, int level);
    void threshold_edges (float **in, QImage out, int nrows, int ncols);
    void estimate_threshShen (double *low, double *hi, int nr, int nc);
};

#endif // EDGEDETECTSHENCASTAN_H
