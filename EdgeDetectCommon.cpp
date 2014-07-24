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

#include "EdgeDetectCommon.h"


EdgeDetectCommon::EdgeDetectCommon(QImage *img)
{
    setImgPtr(img);
}

/*EdgeDetectCommon::EdgeDetectCommon()
{
}*/

QImage * EdgeDetectCommon::getImgPtr()
{
    return imgOrig;
}

void EdgeDetectCommon::setImgPtr(QImage *img)
{
    imgOrig = img;
}

/***
  * Devo passar sempre: width, height
  ***/
float ** EdgeDetectCommon::f2d (int nr, int nc)
{
    float **x, *y;
    int i;

    // calloc aloca uma regiao da memoria para conter um array de
    // nr elementos de tamanho sizeof....
    x = (float **)calloc (nr, sizeof (float *));
    y = (float *)calloc (nr*nc, sizeof (float));

    if ( (x == 0) || (y == 0) ) {
        qDebug() << "Falta de espaco em: F2D.";
        exit (1);
    }

    for (i = 0; i < nr; i++)
        x[i] = y + i * nc;

    return x;
}

/***
  *	Gaussiana
  ***/
float EdgeDetectCommon::gauss(float x, float sigma)
{
    return (float)exp((double) ((-x*x)/(2*sigma*sigma)));
}

float EdgeDetectCommon::norm (float x, float y)
{
    return (float) sqrt ((double)(x*x + y*y));
}

int EdgeDetectCommon::range (QImage *x, int i, int j)
{
    if ((i >= 0) && (i < x->height()) && (j>=0) && (j < x->width()))
        return 1;
    else
        return 0;
}

float EdgeDetectCommon::meanGauss (float x, float sigma)
{
    float z;

    z = (gauss(x,sigma)+gauss(x+0.5,sigma)+gauss(x-0.5,sigma))/3.0;
    z = z/(M_PI*2.0*sigma*sigma);

    return z;
}
