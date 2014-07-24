#include "EdgeDetectShenCastan.h"

EdgeDetectShenCastan::EdgeDetectShenCastan(QImage *img, float ratioThresh,
                                           int sizeAdapGrad, float smoothFactor,
                                           int thinFactor, bool doHysteresis):
        EdgeDetectCommon(img)
{
    b = 0.9;
    window_size = 7;
    low_thresh = 20;
    high_thresh = 22;	/* threshold for hysteresis */
    do_hysteresis = 1;
    ratio = 0.99;

    QImage img1 = *img;

    qDebug() << "Num. of pixels to be above HIGH threshold: " << ratioThresh;
    qDebug() << "Size of window for adaptive gradient: " << sizeAdapGrad;
    qDebug() << "Thinning factor: " << thinFactor;
    qDebug() << "Smoothing factor: " << smoothFactor;
    qDebug() << "Hysteresis thresholding turned: " << (doHysteresis?"Yes":"No");

    embed (&img1, 25);

    shen (img, &img1);

    debed (&img1, 25);

    *img = img1;
}

void EdgeDetectShenCastan::shen (const QImage *im, QImage *res)
{
    register int i,j;
    float **buffer;
    float **smoothed_buffer;
    QImage bli_buffer;


    /* Convert the input image to floating point */
    buffer = f2d (im->width(), im->height());

    for (i = 0; i < im->width(); i++)
        for (j = 0; j < im->height(); j++)
            buffer[i][j] = (float)(qGray(im->pixel(i, j)));

    /* Smooth input image using recursively implemented ISEF filter */
    smoothed_buffer = f2d(im->width(), im->height());
    compute_ISEF (buffer, smoothed_buffer, im->width(), im->height());

    /* Compute bli image band-limited laplacian image from smoothed image */
    bli_buffer = compute_bli(smoothed_buffer, buffer,
                             im->width(), im->height());

    /* Perform edge detection using bli and gradient thresholding */
    locate_zero_crossings(buffer, smoothed_buffer, bli_buffer,
                          im->width(), im->height());

    free(smoothed_buffer[0]);
    free(smoothed_buffer);

    //freeimage (bli_buffer); //liberar img

    threshold_edges(buffer, *res, im->width(), im->height());

    for (i = 0; i < im->width(); i++) {
        for (j = 0; j < im->height(); j++) {
            if (qGray(res->pixel(QPoint(i, j))) > 0)
                res->setPixel(QPoint(i, j), qRgb(0, 0, 0));
            else
                res->setPixel(QPoint(i, j), qRgb(255, 255, 255));
        }
    }

    free(buffer[0]);
    free(buffer);
}

void EdgeDetectShenCastan::embed (QImage *im, int width)
{
    int i, j, I, J;
    QImage newImg;

    width += 2;

    newImg = QImage(im->width()+width+width,
                    im->height()+width+width,
                    im->format());

    for (i = 0; i < newImg.width(); i++) {
        for (j = 0; j < newImg.height(); j++) {
            I = (i-width+im->width()) % (im->width());
            J = (j-width+im->height()) % (im->height());
            newImg.setPixel(QPoint(i, j), im->pixel(I, J));
        }
    }

    *im = newImg;
}

void EdgeDetectShenCastan::debed (QImage *im, int width)
{
    int i,j;
    QImage old;

    width += 2;

    old = QImage(im->width()-width-width, im->height()-width-width, im->format());

    for (i = 0; i < old.width()-1; i++) {
        for (j = 1; j < old.height(); j++) {
            old.setPixel(QPoint(i, j), im->pixel(QPoint(i+width, j+width)));
            old.setPixel(QPoint(old.width()-1, j), qRgb(255, 255, 255));
        }
        old.setPixel(QPoint(i, 0), qRgb(255, 255, 255));
    }

    //free (im);
    //free(im->data[0]); free(im->data);
    //im->info = old->info;
    *im = old;
}


void EdgeDetectShenCastan::apply_ISEF_horizontal (float **x, float **y,
                                        float **A, float **B,
                                        int nrows, int ncols)
{
    register int row, col;
    float b1, b2;

    b1 = (1.0 - b) / (1.0 + b);
    b2 = b * b1;

    /* compute boundary conditions */
    for (row = 0; row < nrows; row++) {
        A[row][0] = b1 * x[row][0];
        B[row][ncols-1] = b2 * x[row][ncols-1];
    }

    /* compute causal component */
    for (col = 1; col < ncols; col++)
        for (row = 0; row < nrows; row++)
            A[row][col] = b1 * x[row][col] + b * A[row][col-1];

    /* compute anti-causal component */
    for (col = ncols-2; col >= 0; col--)
        for (row = 0; row < nrows; row++)
            B[row][col] = b2 * x[row][col] + b * B[row][col+1];

    /* boundary case for computing output of first filter */
    for (row = 0; row < nrows; row++)
        y[row][ncols-1] = A[row][ncols-1];

    /* now compute the output of the second filter and store in y */
    /* this is the sum of the causal and anti-causal components */
    for (row = 0; row < nrows; row++)
        for (col = 0; col < ncols-1; col++)
            y[row][col] = A[row][col] + B[row][col+1];
}

void EdgeDetectShenCastan::apply_ISEF_vertical (float **x, float **y,
                                      float **A, float **B,
                                      int nrows, int ncols)
{
    register int row, col;
    float b1, b2;

    b1 = (1.0 - b)/(1.0 + b);
    b2 = b*b1;

    /* compute boundary conditions */
    for (col = 0; col < ncols; col++) {
        /* boundary exists for 1st and last column */
        A[0][col] = b1 * x[0][col];
        B[nrows-1][col] = b2 * x[nrows-1][col];
    }

    /* compute causal component */
    for (row = 1; row < nrows; row++)
        for (col = 0; col < ncols; col++)
            A[row][col] = b1 * x[row][col] + b * A[row-1][col];

    /* compute anti-causal component */
    for (row = nrows-2; row >= 0; row--)
        for (col=0; col < ncols; col++)
            B[row][col] = b2 * x[row][col] + b * B[row+1][col];

    /* boundary case for computing output of first filter */
    for (col = 0; col < ncols-1; col++)
        y[nrows-1][col] = A[nrows-1][col];

    /* now compute the output of the first filter and store in y */
    /* this is the sum of the causal and anti-causal components */
    for (row = 0; row < nrows-2; row++)
        for (col = 0; col < ncols-1; col++)
            y[row][col] = A[row][col] + B[row+1][col];
}

/**
  *
  * Recursive filter realization of the ISEF (Shen and Castan CVIGP March 1992)
  *
  **/
void EdgeDetectShenCastan::compute_ISEF (float **x, float **y,
                                         int nrows, int ncols)
{
    float **A, **B;

    A = f2d(nrows, ncols); /* store causal component */
    B = f2d(nrows, ncols); /* store anti-causal component */

    /* first apply the filter in the vertical direcion (to the rows) */
    apply_ISEF_vertical (x, y, A, B, nrows, ncols);

    /* now apply the filter in the horizontal direction (to the columns) and */
    /* apply this filter to the results of the previous one */
    apply_ISEF_horizontal (y, y, A, B, nrows, ncols);

    /* libera a memória */
    free (B[0]); free(B);
    free (A[0]); free(A);
}

/**
  *
  * compute the band-limited laplacian of the input image
  *
  **/
QImage EdgeDetectShenCastan::compute_bli (float **buff1, float **buff2,
                                int nrows, int ncols)
{
    register int row, col;
    float buff;
    QImage bli_buffer(nrows, ncols, QImage::Format_RGB32); //newimage(nrows, ncols);

    for (row = 0; row < nrows; row++)
        for (col = 0; col < ncols; col++)
            bli_buffer.setPixel(QPoint(row, col), qRgb(0, 0, 0));

    /* The bli is computed by taking the difference between the smoothed image
     and the original image.  In Shen and Castan's paper this is shown to
     approximate the band-limited laplacian of the image.  The bli is then
     made by setting all values in the bli to 1 where the laplacian is
     positive and 0 otherwise. 						*/

    for (row = 0; row < nrows; row++) {
        for (col = 0; col < ncols; col++) {
            if (row < OUTLINE || row >= nrows-OUTLINE ||
                col < OUTLINE || col >= ncols-OUTLINE)
                continue;
            buff = ((buff1[row][col] - buff2[row][col]) > 0.0);
            bli_buffer.setPixel(QPoint(row,col), qRgb(buff, buff, buff));
        }
    }

    return bli_buffer;
}

/**
  *
  * finds zero-crossings in laplacian (buff)  orig is the smoothed image
  *
  * test for zero-crossings of laplacian then make sure that zero-crossing
  * sign correspondence principle is satisfied.  i.e. a positive z-c must
  * have a positive 1st derivative where positive z-c means the 2nd deriv
  * goes from positive to negative as we pass through the step edge
  *
  **/
int EdgeDetectShenCastan::is_candidate_edge (QImage buff, float **orig,
                                             int row, int col)
{
    /* positive z-c */
    if (qGray(buff.pixel(QPoint(row, col))) == 1 &&
        qGray(buff.pixel(QPoint(row+1, col))) == 0) {
        if (orig[row+1][col] - orig[row-1][col] > 0)
            return 1;
       else
           return 0;
    }
    /* positive z-c */
    else if (qGray(buff.pixel(QPoint(row, col))) == 1 &&
             qGray(buff.pixel(QPoint(row, col+1))) == 0) {
        if (orig[row][col+1] - orig[row][col-1] > 0)
            return 1;
        else
           return 0;
    }
    /* negative z-c */
    else if (qGray(buff.pixel(QPoint(row, col))) == 1 &&
             qGray(buff.pixel(QPoint(row+1, col))) == 0) {
        if (orig[row+1][col] - orig[row-1][col] < 0)
            return 1;
        else
            return 0;
    }
    /* negative z-c */
    else if (qGray(buff.pixel(QPoint(row, col)))  == 1 &&
             qGray(buff.pixel(QPoint(row, col-1))) == 0) {
        if (orig[row][col+1] - orig[row][col-1] < 0)
            return 1;
       else
           return 0;
    }
    /* not a z-c */
    else
      return 0;
}

float EdgeDetectShenCastan::compute_adaptive_gradient (QImage BLI_buffer,
                                             float **orig_buffer,
                                             int row, int col)
{
    register int i, j;
    float sum_on, sum_off;
    float avg_on, avg_off;
    int num_on, num_off;

    sum_on = sum_off = 0.0;
    num_on = num_off = 0;

    for (i = (-window_size/2); i <= (window_size/2); i++) {
        for (j = (-window_size/2); j <= (window_size/2); j++) {
            if (qGray(BLI_buffer.pixel(QPoint(row+i, col+j)))) {
                sum_on += orig_buffer[row+i][col+j];
                num_on++;
            }
            else {
                sum_off += orig_buffer[row+i][col+j];
                num_off++;
            }
        }
    }

    if (sum_off)
        avg_off = sum_off / num_off;
    else
        avg_off = 0;

    if (sum_on)
        avg_on = sum_on / num_on;
    else
        avg_on = 0;

    return (avg_off - avg_on);
}

void EdgeDetectShenCastan::locate_zero_crossings (float **orig, float **smoothed,
                                        QImage bli, int nrows, int ncols)
{
    register int row, col;

    for (row = 0; row < nrows; row++) {
        for (col = 0; col < ncols; col++) {
            /* ignore pixels around the boundary of the image */
            if (row < OUTLINE || row >= nrows-OUTLINE ||
                col < OUTLINE || col >= ncols-OUTLINE) {
                orig[row][col] = 0.0;
            }

            /* next check if pixel is a zero-crossing of the laplacian  */
            else if (is_candidate_edge (bli, smoothed, row, col)) {
                /* now do gradient thresholding  */
                float grad = compute_adaptive_gradient (bli, smoothed, row, col);
                orig[row][col] = grad;
            }
            else
                orig[row][col] = 0.0;
        }
    }
}

/*	return true if it marked something */
int EdgeDetectShenCastan::mark_connected (int i, int j, int level)
{
    int notChainEnd;

    /* stop if you go off the edge of the image */
    if (i >= nr || i < 0 || j >= nc || j < 0)
        return 0;

    /* stop if the point has already been visited */
    if (qGray(edges.pixel(QPoint(i,j))) != 0)
        return 0;

    /* stop when you hit an image boundary */
    if (lap[i][j] == 0.0)
        return 0;

    if ((lap[i][j]) > low_thresh) {
        edges.setPixel(QPoint(i, j), qRgb(1, 1, 1));
    }
    else {
        edges.setPixel(QPoint(i, j), qRgb(255, 255, 255));
    }

    notChainEnd = 0;

    notChainEnd |= mark_connected(i  ,j+1, level+1);
    notChainEnd |= mark_connected(i  ,j-1, level+1);
    notChainEnd |= mark_connected(i+1,j+1, level+1);
    notChainEnd |= mark_connected(i+1,j  , level+1);
    notChainEnd |= mark_connected(i+1,j-1, level+1);
    notChainEnd |= mark_connected(i-1,j-1, level+1);
    notChainEnd |= mark_connected(i-1,j  , level+1);
    notChainEnd |= mark_connected(i-1,j+1, level+1);

    if (notChainEnd && (level > 0)) {
    /* do some contour thinning */
        if (thinFactor > 0)
            if ((level%thinFactor) != 0) {
                /* delete this point */
                edges.setPixel(QPoint(i, j), qRgb(255, 255, 255));
            }
    }

    return 1;
}

void EdgeDetectShenCastan::threshold_edges (float **in, QImage out,
                                  int nrows, int ncols)
{
    register int i, j;

    lap = in;
    edges = out;
    nr = nrows;
    nc = ncols;

    estimate_threshShen (&low_thresh, &high_thresh, nr, nc);

    if (!do_hysteresis)
        low_thresh = high_thresh;

    for (i = 0; i < nrows; i++)
        for (j = 0; j < ncols; j++)
            edges.setPixel(QPoint(i, j), qRgb(0, 0, 0));

    for (i = 0; i < nrows; i++)
        for (j = 0; j < ncols; j++) {
            if (i < OUTLINE || i >= nrows-OUTLINE ||
                j < OUTLINE || j >= ncols-OUTLINE)
                continue;

            /* only check a contour if it is above high_thresh */
            if ((lap[i][j]) > high_thresh)
                /* mark all connected points above low thresh */
                mark_connected (i, j, 0);
        }

    /* erase all points which were 255 */
    for (i = 0; i < nrows; i++)
        for (j=0; j<ncols; j++)
            if (qGray(edges.pixel(QPoint(i, j))) == 255)
                edges.setPixel(QPoint(i, j), qRgb(0, 0, 0));
}

void EdgeDetectShenCastan::estimate_threshShen (double *low, double *hi,
                                                int nr, int nc)
{
    float vmax, vmin, scale, x;
    int i, j, k, hist[256], count;

    /* Build a histogram of the Laplacian image. */
    vmin = vmax = fabs((float)(lap[20][20]));

    for (i = 0; i < nr; i++)
        for (j = 0; j < nc; j++) {
            if (i < OUTLINE || i >= nr-OUTLINE ||
                j < OUTLINE || j >= nc-OUTLINE)
                continue;
            x = lap[i][j];
            if (vmin > x)
                vmin = x;
            if (vmax < x)
                vmax = x;
        }

    for (k = 0; k < 256; k++)
        hist[k] = 0;

    scale = 256.0/(vmax-vmin + 1);

    for (i = 0; i < nr; i++)
        for (j = 0; j < nc; j++) {
            if (i < OUTLINE || i >= nr-OUTLINE ||
                j < OUTLINE || j >= nc-OUTLINE)
                continue;
            x = lap[i][j];
            k = (int)((x - vmin)*scale);
            hist[k] += 1;
        }

    /* The high threshold should be > 80 or 90% of the pixels */
    k = 255;
    j = (int)(ratio*nr*nc);
    count = hist[255];

    while (count < j) {
        k--;
        if (k < 0)
            break;
        count += hist[k];
    }

    *hi = (double)k/scale + vmin ;
    *low = (*hi)/2;
}
