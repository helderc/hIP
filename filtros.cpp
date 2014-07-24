#include "filtros.h"


Filtros::Filtros()
{
}


/**
 * @brief Aplica o filtro de Cor Sepia na imagem.
 *
 * @param img Imagem a ser processada.
 */
void Filtros::filtroSepia(QImage *img)
{
    // RGBA -> por isso M[4x4]
    float m[4][4] = {{0.272, 0.534, 0.131, 0},
                     {0.349, 0.686, 0.168, 0},
                     {0.393, 0.769, 0.189, 0},
                     {    0,     0,     0, 0}};

    /*
     * r = (qRed(cor) * 0.393) + (qGreen(cor) * 0.769) + (qBlue(cor) * 0.189);
     * g = (qRed(cor) * 0.349) + (qGreen(cor) * 0.686) + (qBlue(cor) * 0.168);
     * b = (qRed(cor) * 0.272) + (qGreen(cor) * 0.534) + (qBlue(cor) * 0.131);
     */

    // Kernel 4x4
    Mat k(4, 4, CV_32F, &m);

    // QImage -> Mat
    Mat imgMat = OpCommons::QImageToMat(img);

    // Transformacao
    transform(imgMat, imgMat, k);

    // Convertendo para uma estrutura QImage.
    // cv::Mat -> QImage
    img = OpCommons::MatToQImage(imgMat);
}


void Filtros::filtroTomCinza(QImage *img)
{
    int cinza;
    QRgb cor;

    for (int x = 0; x < img->width(); x++)
        for (int y = 0; y < img->height(); y++) {
            cor = img->pixel(QPoint(x,y));
            cinza = qGray(cor);
            img->setPixel(QPoint(x, y), qRgb(cinza, cinza, cinza));
        }
}


/**
 * @brief Suaviza uma imagem utilizando a funcao GaussianBlur do OpenCV.
 *
 * @param img Imagem a ser suavizada.
 * @param ksize Tamanho do Kernel
 * @param sigmax Valor do Sigma em X
 * @param sigmay Valor do Sigma em Y
 */
void Filtros::filtroGauss(QImage *img, int ksize, double sigmax, double sigmay)
{
    Mat cvImg = OpCommons::QImageToMat(img);

    GaussianBlur(cvImg, cvImg, Size(ksize, ksize), sigmax, sigmay);

    img = OpCommons::MatToQImage(cvImg);
}

// FIXME: Nao funciona ainda.
void Filtros::filtroCarton(QImage *img)
{
    qDebug() << "filtroCarton";

    Mat cvImg = OpCommons::QImageToMat(img);
    Mat cvImgGray;

    cvtColor(cvImg, cvImg, CV_BGRA2BGR);

    pyrMeanShiftFiltering(cvImg, cvImg, 2, 40, 2);

    // RGB -> Cinza
    //cvtColor(cvImg, cvImgGray, CV_RGB2GRAY);

    //Canny(cvImgGray, cvImgGray, 150, 150);

    // Cinza -> BGR
    //cvtColor(cvImgGray, cvImgGray, CV_GRAY2BGR);

    //cvImg = cvImg - cvImgGray;

    cvtColor(cvImg, cvImg, CV_BGR2BGRA);

    img = OpCommons::MatToQImage(cvImg);
}

void Filtros::filtroSobel(QImage *img, char op)
{
    int x, y, i, j, SUM, cinza;
    long sumX, sumY;

    int GX[3][3] = {{-1, 0, 1},
                    {-2, 0, 2},
                    {-1, 0, 1}};

    int GY[3][3] = {{1, 2, 1},
                    {0, 0, 0},
                    {-1, -2, -1}};

    // copy the original image
    QImage img1 = *img;

    // TODO: Tratar a situacao onde a img nao tem o RGB (apenas tons de cinza).

    for (y = 0; y <= img->height()-1; y++) {
        for (x = 0; x <= img->width()-1; x++) {
            sumX = sumY = 0;

            // bordas da Imagem
            if (y == 0 || y == img->height()-1)
                SUM = 0;
            else if (x == 0 || x == img->width()-1)
                SUM = 0;
            // Convolucao
            else   {
                // Aproximacao do gradiente em X
                if (op == 'x' || op == 'a') {
                    for (i = -1; i <= 1; i++) {
                        for (j = -1; j <= 1; j++)  {
                            cinza = qGray(img1.pixel(QPoint(x+i, y+j)));
                            sumX += (int)(cinza * GX[i+1][j+1]);
                        }
                    }
                }

                // Aproximacao do gradiente em Y
                if (op == 'y' || op == 'a') {
                    for (i = -1; i <= 1; i++)  {
                        for (j = -1; j <= 1; j++)  {
                            cinza = qGray(img1.pixel(QPoint(x+i, y+j)));
                            sumY = sumY + (int)(cinza * GY[i+1][j+1]);
                        }
                    }
                }

                // Magnitude do Gradiente
                SUM = abs(sumX) + abs(sumY);
            }

            // Threshold
            if (SUM < 0) SUM = 0;
            if (SUM > 255) SUM = 255;

            int corw = 255 - SUM;
            img->setPixel(QPoint(x, y), qRgb(corw, corw, corw));
        }
    }
}

void Filtros::filtroCannyOCV(QImage *img, int lowThresh, int ksize)
{
    // FIXME: Not working yet.
    /// Reduce noise with a kernel 3x3
    //blur( src_gray, detected_edges, Size(3,3) );

    /// Canny detector
    //Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );
}


// TODO: Convolucao baseado na Matriz m e sua dimensao msize
void Filtros::filtroConv(QImage *img, float m[], int msize)
{
    /*int r, g, b;
    int x, y;
    QRgb cor;

    QImage *img;

    // A Matriz eh sempre quadrada, posso sqrt ela!
    int matrixDim = sqrt(sizeof(*m));
    int kernelMid = matrixDim/2;

    for (int i = 0; i <= matrixDim; i++) {
        for (int j = 0; j <= matrixDim; j++) {
            qDebug () << *(m+i+j);
        }
    }

    qDebug() << "matrixDim: " << matrixDim;
    qDebug() << "kernelMid: " << kernelMid;
    */

    /* TODO: Pegar a dimensao da matrix para descobrir a posicao do nucleo
            para fazer o tratamento das bordas.
            Tipos de tratamento de borda:
                * Assume off-image pixels are black.
                * Assume they're white.
                * Assume they're some other color.
                * Assume they're the color of the nearest pixel on the image.
                * Assume the image wraps around, so the row of pixels off the
                    top is the same as the row of pixels on the bottom.
                * Assume... something else very complicated.
                * Ignore the topmost, bottommost, leftmost, and rightmost lines
                    of pixels, and crop (or mask) them out of the result.
      */

    /*for (x = 0; x < img->width(); x++)
        for (y = 0; y < img->height(); y++) {
            cor = img->pixel(QPoint(x,y));
            r = (qRed(cor) * 0.393) + (qGreen(cor) * 0.769) + (qBlue(cor) * 0.189);
            g = (qRed(cor) * 0.349) + (qGreen(cor) * 0.686) + (qBlue(cor) * 0.168);
            b = (qRed(cor) * 0.272) + (qGreen(cor) * 0.534) + (qBlue(cor) * 0.131);

            if (r > 255) r = 255;
            if (g > 255) g = 255;
            if (b > 255) b = 255;

            if (r < 0) r = 0;
            if (g < 0) g = 0;
            if (b < 0) b = 0;

            img->setPixel(QPoint(x, y), qRgb(r, g, b));
        }*/

}
