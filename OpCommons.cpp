#include "OpCommons.h"

/**
 * @brief
 *
 */
OpCommons::OpCommons()
{
}

/**
 * @brief Converte uma imagem em QImage para IplImage.
 *      IplImage e' o formato de imagem do OpenCV, enquanto que QImage
 *      e' formato do Qt.
 * @param qImage
 * @return IplImage *
 */
IplImage * OpCommons::QImageToIplImage(const QImage * qImage)
{
    int width = qImage->width();
    int height = qImage->height();
    CvSize Size;
    Size.height = height;
    Size.width = width;

    IplImage *charIplImageBuffer = cvCreateImage(Size, IPL_DEPTH_8U, 1);
    char *charTemp = (char *) charIplImageBuffer->imageData;

    // converte a img p/ tons de cinza
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = y * width + x;
            charTemp[index] = (char) qGray(qImage->pixel(x, y));
        }
    }

    return charIplImageBuffer;
}


/**
 * @brief
 *
 * @param iplimg
 * @return QImage
 */
QImage OpCommons::IplImageToQImage(IplImage *iplimg)
{
    // Convert it from BGR to RGB. QImage works with RGB and cvQueryFrame returns a BGR IplImage
    cvCvtColor(iplimg, iplimg, CV_BGR2RGB);

    QImage qimg((const uchar *)iplimg->imageData, iplimg->width,
                 iplimg->height, QImage::Format_RGB888);

    return qimg;
}


Mat OpCommons::QImageToMat(const QImage *img)
{
    // QImage -> Mat
    Mat imgMat(img->height(),
               img->width(),
               CV_8UC4,
               (uchar *)img->bits(),
               img->bytesPerLine());

    return imgMat;
}


QImage * OpCommons::MatToQImage(Mat imgMat)
{
    cvtColor(imgMat, imgMat, CV_RGB2BGR);

    // cv::Mat -> QImage
    QImage *img2 = new QImage((const uchar*)(imgMat.data),
                              imgMat.cols,
                              imgMat.rows,
                              QImage::Format_RGB888);

    return img2;
}


/**
 * @brief Medidor de Ruídos
 *
 * @param img
 * @return QString *
 */
QString * OpCommons::noiseMeter(QImage *img)
{
    long i,j,k;
    float x, y, z;
    double mean, sd;
    QString *str = new QString();
    QRgb cor;
    int cinza;

    // Measure
    k = 0;
    x = y = 0.0;

    for (i = 0; i < img->width(); i++)
        for (j = 0; j < img->height(); j++) {
            cor = img->pixel(QPoint(i, j));
            cinza = (float) qGray(cor);
            x += cinza;
            y += cinza * cinza;
            k += 1;
        }

    // Compute estimate - mean noise is 0
    if ((float)(k-1) > 0.0)
        sd = (double)(y - x*x/(float)k)/(float)(k-1);
    else
        sd = 1.0;

    if (k != 0)
        mean = (double)(x/(float)k);
    else
        mean = -1;

    sd = sqrt(sd);
    *str = QString::fromUtf8("Image \nMean: ") +
          QString::number(mean, 'f', 5) +
          QString::fromUtf8(" :: Default deviation: ") +
          QString::number(sd, 'f', 5) + "\n\n";

    // Now assume that the uniform level is the mean, and compute the
    // mean and SD of the differences from that!

    x = y = z = 0.0;

    for (i = 0; i < img->width(); i++)
        for (j = 0; j < img->height(); j++) {
            // pega o RGB do pixel (i,j)
            cor = img->pixel(QPoint(i,j));
            cinza = qGray(cor);

            z = (float)(cinza - mean);
            x += z;
            y += z*z;
        }

    if ((float)(k-1) > 0.0)
        sd = (double)(y - x*x/(float)k)/(float)(k-1);
    else
        sd = 1.0;

    if (k != 0)
        mean = (double)(x/(float)k);
    else
        mean = -1;

    sd = sqrt(sd);
    str->append(QString::fromUtf8("Noise \nMean: ") +
               QString::number(mean, 'f', 5) +
               QString::fromUtf8(" :: Default deviation: ") +
               QString::number(sd, 'f', 5));

    return str;
}

/**
 * @brief Equaliza o histograma de uma imagem
 *      TODO: Suportar imagens coloridas
 * @param img
 */
void OpCommons::eqHist(QImage *img)
{
    int hist[256][3];
    int cinza;
    float somaNi = img->height() * img->width();
    float constante;

    for (int k = 0; k < 256; k++) {
        hist[k][0] = 0;
        hist[k][1] = 0;
        hist[k][2] = 0;
    }

    // cria o histograma
    for (int i = 0; i < img->width(); i++) {
        for (int j = 0; j < img->height(); j++) {
            cinza = qGray(img->pixel(QPoint(i, j)));
            hist[cinza][0]++;
        }
    }

    constante = 255/somaNi;

    // calcula as colunas 2 e 3 de hist
    for (int i = 0; i < 256; i++) {
        // calcula a Frequencia Acumulada dos ni (freqAcumuladaNi)
        for (int j = 0; j < i; j++)
            hist[i][1] += hist[j][0];
        // calcula (1/24)*freqAcumuladaNi
        // arredonda os valores para ter o nivel de cinza equalizado
        hist[i][2] = (int)(constante * hist[i][1]);
    }

    // aplica os novos tons de cinza na imagem final
    for (int i = 0; i < img->width(); i++) {
        for (int j = 0; j < img->height(); j++) {
            cinza = qGray(img->pixel(QPoint(i, j)));
            cinza = hist[cinza][2];
            img->setPixel(QPoint(i,j), qRgb(cinza, cinza, cinza));
        }
    }
}



