#include "EdgeDetectCanny.h"

/***
  * Inicializacao do Ambiente
  *
  */
EdgeDetectCanny::EdgeDetectCanny(QImage *img, int dP,
                                 int thresLow, int thresHigh):
        EdgeDetectCommon(img)
{
    // Canny
    ratio = 0.1f;
    WIDTH = 0;

    int i, j;
    //float s=1.0; // ja estava comentado
    //int low= 0,high=-1; // ja estava comentado
    QImage oriim, magim, img2;

    /*
      tirando 2 copias da img original
      img = imagem a ser alterada
    */
    oriim = magim = img2 = *img;

    /* Apply the filter */
    canny ((float)dP, &img2, &magim, &oriim);

    /* Hysteresis thresholding of edge pixels */
    hysteresis (thresHigh, thresLow, img, &magim, &oriim);

    for (i = 0; i < WIDTH; i++) // TODO: falta definir o que eh o WIDTH
        for (j = 0; j < img->height(); j++)
            img->setPixel(QPoint(i, j), qRgb(255, 255, 255));

    for (i = img->height()-1; i > img->height()-1-WIDTH; i--)
        for (j = 0; j < img->height(); j++)
            img->setPixel(QPoint(i, j), qRgb(255, 255, 255));

    for (i = 0; i < img->width(); i++)
        for (j = 0; j < WIDTH; j++)
            img->setPixel(QPoint(i, j), qRgb(255, 255, 255));

    for (i = 0; i < img->width(); i++)
        for (j = img->height()-WIDTH-1; j < img->height(); j++)
            img->setPixel(QPoint(i, j), qRgb(255, 255, 255));
}


/***
  * TRACE - recursively trace edge pixels that have a threshold > the low
  * edge threshold, continuing from the pixel at (i,j).
  ***/
int EdgeDetectCanny::trace (int i, int j, int low,
                       QImage *im,
                       QImage *mag,
                       QImage *ori)
{
    int n, m;
    char flag = 0;

    if (qGray(im->pixel(QPoint(i, j))) == 0) {
        im->setPixel(QPoint(i, j), qRgb(255, 255, 255));
        flag = 0;
        for (n = -1; n <= 1; n++) {
            for(m = -1; m <= 1; m++) {
                if (i == 0 && m == 0)
                    continue;
                /*
                neste ponto as coordenadas estrapolam os limites da imagem
                por causa +n e +m
                */
                if (range(mag, i+n, j+m) && qGray(mag->pixel(QPoint(i+n, j+m))) >= low)
                    if (trace(i+n, j+m, low, im, mag, ori)) {
                        flag=1;
                        break;
                    }
            }
            if (flag)
                break;
        }
        return(1);
    }
    return(0);
}

/***
  * Primeira derivada da gaussiana
  ***/
float EdgeDetectCanny::dGauss (float x, float sigma)
{
    return -x/(sigma*sigma) * gauss(x, sigma);
}

void EdgeDetectCanny::hysteresis (int high,
                             int low,
                             QImage *im,
                             QImage *mag,
                             QImage *oriim)
{
    int i,j;

    qDebug() << "Beginning hysteresis thresholding...";
    for (i = 0; i < im->width(); i++)
        for (j = 0; j < im->height(); j++)
            im->setPixel(QPoint(i, j), qRgb(0, 0, 0));

    if (high < low) {
        estimate_thresh (mag, &high, &low);
        qDebug() << "Hysteresis thresholds (from image): HI " << high
                << " LOW " << low;
    }

    /* For each edge with a magnitude above the high threshold, begin
    tracing edge pixels that are above the low threshold.                */
    for (i = 0; i < im->width(); i++)
        for (j = 0; j < im->height(); j++)
            if (qGray(mag->pixel(QPoint(i, j))) >= high) {
                trace (i, j, low, im, mag, oriim);
            }

    /*
    Make the edge black (to be the same as the other methods)
    */
    for (i = 0; i < im->width(); i++)
        for (j = 0; j < im->height(); j++)
            if (qGray(im->pixel(QPoint(i, j))) == 0)
                im->setPixel(QPoint(i, j), qRgb(255, 255, 255));
            else
                im->setPixel(QPoint(i, j), qRgb(0, 0, 0));
}

void EdgeDetectCanny::canny (float desvPadrao,
                             QImage *im, QImage *mag, QImage *ori)
{
    int width = 0;
    float **smx, **smy;
    float **dx, **dy;
    int i, j, n;
    float gau[20], dgau[20], z;
    unsigned int rgb;

    /* Create a Gaussian and a derivative of Gaussian filter mask */
    for (i = 0; i < 20; i++) {
        gau[i] = meanGauss ((float)i, desvPadrao);
        if (gau[i] < 0.005) {
            width = i;
            break;
        }
        dgau[i] = dGauss ((float)i, desvPadrao);
    }

    n = width + width + 1;
    WIDTH = width/2;
    qDebug() << "Smoothing with a Gaussian (width = " <<  n << ")";

    // troquei width <> height
    // padrao: im->info->nr, im->info->nc
    smx = f2d (im->width(), im->height());
    smy = f2d (im->width(), im->height());

    /* Convolution of source image with a Gaussian in X and Y directions  */
    seperable_convolution (im, gau, width, smx, smy);

    /* Now convolve smoothed data with a derivative */
    qDebug() << "Convolution with the derivative of a Gaussian...";

    // troquei width <> height
    dx = f2d (im->width(), im->height());
    dxy_seperable_convolution (smx,
                               im->width(),
                               im->height(),
                               dgau,
                               width,
                               dx,
                               1);
    free(smx[0]); free(smx);

    // troquei width <> height
    dy = f2d (im->width(), im->height());

    dxy_seperable_convolution (smy,
                               im->width(),
                               im->height(),
                               dgau,
                               width,
                               dy,
                               0);
    free(smy[0]); free(smy);

    /* Create an image of the norm of dx,dy */
    for (i = 0; i < im->width(); i++)
        for (j = 0; j < im->height(); j++) {
            z = norm (dx[i][j], dy[i][j]);
            rgb = (unsigned char)(z*20.0);
            mag->setPixel(QPoint(i, j), qRgb(rgb, rgb, rgb));
        }

    /* Non-maximum suppression - edge pixels should be a local max */
    nonmax_suppress (dx, dy, mag, ori);

    free(dx[0]); free(dx);
    free(dy[0]); free(dy);
}

void EdgeDetectCanny::seperable_convolution (QImage *im, float *gau, int width,
                            float **smx, float **smy)
{
    int i,j,k, I1, I2, nr, nc;
    float x, y;

    nr = im->width();
    nc = im->height();

    for (i = 0; i < nr; i++)
        for (j = 0; j < nc; j++) {
            x = gau[0] * qGray(im->pixel(QPoint(i, j)));
            y = gau[0] * qGray(im->pixel(QPoint(i, j)));
            for (k = 1; k < width; k++) {
                I1 = (i+k)%nr;
                I2 = (i-k+nr)%nr;
                y += gau[k] * qGray(im->pixel(QPoint(I1, j))) +
                     gau[k] * qGray(im->pixel(QPoint(I2, j)));
                I1 = (j+k)%nc;
                I2 = (j-k+nc)%nc;
                x += gau[k] * qGray(im->pixel(QPoint(i, I1))) +
                     gau[k] * qGray(im->pixel(QPoint(i, I2)));
            }
            smx[i][j] = x;
            smy[i][j] = y;
        }
}

void EdgeDetectCanny::dxy_seperable_convolution (float** im, int nr, int nc,
                                            float *gau, int width, float **sm,
                                            int which)
{
    int i,j,k, I1, I2;
    float x;

    for (i = 0; i < nr; i++)
        for (j = 0; j < nc; j++) {
            x = 0.0;
            for (k = 1; k < width; k++) {
                if (which == 0) {
                    I1 = (i+k)%nr;
                    I2 = (i-k+nr)%nr;
                    x += -gau[k]*im[I1][j] + gau[k]*im[I2][j];
                }
                else {
                    I1 = (j+k)%nc;
                    I2 = (j-k+nc)%nc;
                    x += -gau[k]*im[i][I1] + gau[k]*im[i][I2];
                }
            }
            sm[i][j] = x;
        }
}

void EdgeDetectCanny::nonmax_suppress (float **dx, float **dy,
                                  QImage *mag, QImage *ori)
{
    int i,j;
    float xx, yy, g2, g1, g3, g4, g, xc, yc;
    unsigned int rgb;


    for (i = 1; i < mag->width()-1; i++) {
        for (j = 1; j < mag->height()-1; j++) {
            mag->setPixel(QPoint(i, j), qRgb(0, 0, 0));
            /* Treat the x and y derivatives as components of a vector */
            xc = dx[i][j];
            yc = dy[i][j];

            if (fabs(xc) < 0.01 && fabs(yc) < 0.01)
                continue;
            g  = norm (xc, yc);
            /* Follow the gradient direction, as indicated by the direction of
               the vector (xc, yc); retain pixels that are a local maximum. */

            if (fabs(yc) > fabs(xc)) {
                /* The Y component is biggest, so gradient direction is basically UP/DOWN */
                xx = fabs(xc)/fabs(yc);
                yy = 1.0;
                g2 = norm (dx[i-1][j], dy[i-1][j]);
                g4 = norm (dx[i+1][j], dy[i+1][j]);
                if (xc*yc > 0.0) {
                    g3 = norm (dx[i+1][j+1], dy[i+1][j+1]);
                    g1 = norm (dx[i-1][j-1], dy[i-1][j-1]);
                }
                else {
                    g3 = norm (dx[i+1][j-1], dy[i+1][j-1]);
                    g1 = norm (dx[i-1][j+1], dy[i-1][j+1]);
                }
            }
            else {
                /* The X component is biggest, so gradient direction is basically LEFT/RIGHT */
                xx = fabs(yc)/fabs(xc);
                yy = 1.0;
                g2 = norm (dx[i][j+1], dy[i][j+1]);
                g4 = norm (dx[i][j-1], dy[i][j-1]);
                if (xc*yc > 0.0) {
                    g3 = norm (dx[i-1][j-1], dy[i-1][j-1]);
                    g1 = norm (dx[i+1][j+1], dy[i+1][j+1]);
                }
                else {
                    g1 = norm (dx[i-1][j+1], dy[i-1][j+1]);
                    g3 = norm (dx[i+1][j-1], dy[i+1][j-1]);
                }
            }

            /* Compute the interpolated value of the gradient magnitude */
            if ((g > (xx*g1 + (yy-xx)*g2)) && (g > (xx*g3 + (yy-xx)*g4))) {
                if (g*MAG_SCALE <= 255) {
                    rgb = (unsigned char)(g*MAG_SCALE);
                    mag->setPixel(QPoint(i, j), qRgb(rgb, rgb, rgb));
                }
                else
                    mag->setPixel(QPoint(i, j), qRgb(255, 255, 255));
                rgb = (unsigned char) (atan2 (yc, xc) * ORI_SCALE);
                ori->setPixel(QPoint(i, j), qRgb(rgb, rgb, rgb));
            }
            else {
                mag->setPixel(QPoint(i, j), qRgb(0, 0, 0));
                ori->setPixel(QPoint(i, j), qRgb(0, 0, 0));
            }
        }
    }
}

void EdgeDetectCanny::estimate_thresh (QImage *mag, int *hi, int *low)
{
    int i,j,k, hist[256], count;

    /* Build a histogram of the magnitude image. */
    for (k = 0; k < 256; k++)
        hist[k] = 0;

    for (i = WIDTH; i < mag->width()-WIDTH; i++)
        for (j = WIDTH; j < mag->height()-WIDTH; j++)
            hist[qGray(mag->pixel(QPoint(i, j)))]++;

    /* The high threshold should be > 80 or 90% of the pixels
        j = (int)(ratio*mag->info->nr*mag->info->nc);
    */
    j = mag->width();

    if (j < mag->height())
        j = mag->height();

    j = (int)(0.9*j);
    k = 255;
    count = hist[255];

    while (count < j) {
        k--;
        if (k < 0)
            break;
        count += hist[k];
    }

    *hi = k;
    i = 0;

    while (hist[i] == 0)
        i++;

    *low = (*hi+i)/2.0f;
}
