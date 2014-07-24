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

#include "EdgeDetectMarrHill.h"

EdgeDetectMarrHill::EdgeDetectMarrHill(QImage *img, int dP):
        EdgeDetectCommon(img)
{
    desvioPadrao = dP;

    edgeDetect();
}

/***
  *
  * TODO: Esta é a chamada feita pelo menu
  *
  **/
void EdgeDetectMarrHill::edgeDetect()
{
    int i, j, cinza1, cinza2;
    QImage im1, im2;

    im1 = im2 = *imgOrig;

    // Aplica o Filtro *
    marr (desvioPadrao-0.8, &im1);
    marr (desvioPadrao+0.8, &im2);

    // analiza o tom de cinza de cada pixel
    for (i = 0; i < im1.width(); i++)
        for (j = 0; j < im1.height(); j++) {
            cinza1 = qGray(im1.pixel(QPoint(i, j)));
            cinza2 = qGray(im2.pixel(QPoint(i, j)));
            if (cinza1 > 0 && cinza2 > 0)
                imgOrig->setPixel(QPoint(i, j), qRgb(0, 0, 0));
            else
                imgOrig->setPixel(QPoint(i, j), qRgb(255, 255, 255));
        }
}

void EdgeDetectMarrHill::marr(float s, QImage *img)
{
    int width;
    float **smx;
    int i, j, n;
    float **lgau;

    /* Create a Gaussian and a derivative of Gaussian filter mask  */
    width = 3.35*s + 0.33;
    n = width + width + 1;

    qDebug() << ">>> Suavizando com Gaussiana de tamanho " << n << "x" << n;

    lgau = f2d (n, n);

    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            lgau[i][j] = LoG (distance((float)i, (float)j,
                                        (float)width, (float)width), s);

    /* Convolution of source image with a Gaussian in X and Y directions  */
    smx = f2d (img->width(), img->height());

    qDebug() << ">>> Convolucao com LoG";

    convolution (img, lgau, n, n, smx, img->height(), img->width());

    /* Locate the zero crossings */
    qDebug() << ">>> Passagens por Zero";
    zero_cross (smx, img);

    /* Clear the boundary */
    for (i = 0; i < img->width(); i++) {
        for (j = 0; j <= width; j++)
            img->setPixel(QPoint(i, j), qRgb(0, 0, 0));
        for (j = img->height() - width - 1; j < img->height(); j++)
            img->setPixel(QPoint(i, j), qRgb(0, 0, 0));
    }

    for (j = 0; j < img->height(); j++) {
        for (i = 0; i <= width; i++)
            img->setPixel(QPoint(i, j), qRgb(0, 0, 0));
        for (i = img->width() - width - 1; i < img->width(); i++)
            img->setPixel(QPoint(i, j), qRgb(0, 0, 0));
    }

    free(smx[0]); free(smx);
    free(lgau[0]); free(lgau);
}

/***
  * Laplaciana da Gaussiana
  ***/
float EdgeDetectMarrHill::LoG (float x, float sigma)
{
    float x1;

    x1 = gauss (x, sigma);

    return (x*x-2*sigma*sigma)/(sigma*sigma*sigma*sigma) * x1;
}

float EdgeDetectMarrHill::distance (float a, float b, float c, float d)
{
    return norm ((a-c), (b-d));
}

/**
  Troquei os NR <-> NC
**/
void EdgeDetectMarrHill::convolution (QImage *img, float **mask, int nr, int nc,
                                      float **res, int NR, int NC)
{
    int i, j, ii, jj, n, m, k, kk;
    float x;

    k = nr/2;
    kk = nc/2;

    for (i = 0; i < NC; i++)
        for (j = 0; j < NR; j++) {
            x = 0.0;
            for (ii = 0; ii < nr; ii++) {
                n = i - k + ii;
                if (n < 0 || n >= NC)
                    continue;
                for (jj = 0; jj < nc; jj++) {
                    m = j - kk + jj;
                    if (m < 0 || m >= NR)
                        continue;
                    x += mask[ii][jj] *
                         (float) qGray((img->pixel(QPoint(n, m))));
                }
            }
            res[i][j] = x;
        }
}

/***
  * Passagens por 0
  **/
void EdgeDetectMarrHill::zero_cross (float **lapim, QImage *img)
{
    int i, j;

    for (i = 1; i < img->width() - 1; i++)
        for (j = 1; j < img->height() - 1; j++) {
            img->setPixel(QPoint(i, j), qRgb(0, 0, 0));

            if (lapim[i-1][j] * lapim[i+1][j] < 0) {
                img->setPixel(QPoint(i, j), qRgb(255, 255, 255));
                continue;
            }

            if (lapim[i][j-1] * lapim[i][j+1] < 0) {
                img->setPixel(QPoint(i, j), qRgb(255, 255, 255));
                continue;
            }

            if (lapim[i+1][j-1] * lapim[i-1][j+1] < 0) {
                img->setPixel(QPoint(i, j), qRgb(255, 255, 255));
                continue;
            }

            if (lapim[i-1][j-1] * lapim[i+1][j+1] < 0) {
                img->setPixel(QPoint(i, j), qRgb(255, 255, 255));
                continue;
            }
      }
}

/***
  * An alternative way to compute a Laplacian
  ***/
void EdgeDetectMarrHill::dolap (float **x, int nr, int nc, float **y)
{
    int i, j;
    float u, v;

    for (i = 1; i < nr-1; i++)
        for (j = 1; j < nc-1; j++) {
            y[i][j] = (x[i][j+1] + x[i][j-1] + x[i-1][j] + x[i+1][j]) - 4*x[i][j];
            if (u > y[i][j])
                u = y[i][j];
            if (v < y[i][j])
                v = y[i][j];
        }
}
