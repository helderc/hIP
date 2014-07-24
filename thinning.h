#ifndef THINNING_H
#define THINNING_H

#include <QImage>
#include <QString>
#include <QDebug>
#include <QVector>
#include <QPoint>
#include <cmath>

#include "filtros.h"

class Thinning
{
public:
    Thinning();
    static void Binarizacao(QImage *img);
    static void Stentiford(QImage *img);
    static void Holt(QImage *img);
    static void ZhanSuen(QImage *img);
    static bool verificaPixel(QImage *img, int x, int y);
    static bool vizinhanca(QImage *img1, int x,int y);
};

#endif // THINNING_H
