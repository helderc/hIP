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
