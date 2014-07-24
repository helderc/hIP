#ifndef FILTROS_H
#define FILTROS_H

#include <QImage>
#include <QDebug>

// Biblioteca OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include "OpCommons.h"

class Filtros
{
public:
    Filtros();

    static void filtroSepia(QImage *img);
    static void filtroTomCinza(QImage *img);
    static void filtroGauss(QImage *img,
                            int ksize = 3,
                            double sigmax = 2,
                            double sigmay = 2);
    static void filtroCarton(QImage *img);
    static void filtroSobel(QImage *img, char op);
    static void filtroCannyOCV(QImage *img, int lowThresh, int ksize);
    static void filtroConv(QImage *img, float m[], int msize);
};

#endif // FILTROS_H
