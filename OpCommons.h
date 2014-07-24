#ifndef OPCOMMONS_H
#define OPCOMMONS_H

#include <QImage>
#include <QString>
#include <QDebug>
#include <QGenericMatrix>

#include <math.h>

// Biblioteca OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>


using namespace cv;

class OpCommons
{
public:
    OpCommons();
    // cv::IplImage
    static IplImage *QImageToIplImage(const QImage * qImage);
    static QImage IplImageToQImage(IplImage *iplimg);

    // cv::Mat
    static Mat QImageToMat(const QImage *img);
    static QImage * MatToQImage(Mat imgMat);

    static QString * noiseMeter(QImage *img);
    static void eqHist(QImage *img);
};

#endif // OPCOMMONS_H
