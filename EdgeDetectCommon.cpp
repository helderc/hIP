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
