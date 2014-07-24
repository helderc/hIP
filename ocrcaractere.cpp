#include "ocrcaractere.h"

OCRCaractere::OCRCaractere(QObject *parent) :
    QObject(parent)
{
}

OCRCaractere::OCRCaractere(QImage caractere, QString s_id, QObject *parent):
    QObject(parent)
{
    img_c = caractere;
    this->s_id = s_id;
}


void OCRCaractere::setImagem(QImage img)
{
    img_c = img;
}

QImage OCRCaractere::imagem()
{
    return img_c;
}

void OCRCaractere::setHistograma(QVector<double> *hist, bool h)
{
    // Histograma horizontal
    if (h)
        hist_hor = hist;
    else
        hist_ver = hist;
}

const QVector<double> &OCRCaractere::histograma(bool h)
{
    if (h)
        return *hist_hor;
    else
        return *hist_ver;
}

void OCRCaractere::setMatrizCoOcorrencia(QHash<QString, int> m)
{
    matrizCoOcorrencia = m;

    // CHAMAR OS OUTROS CALCULOS QUE DEPENDEM DESSE ///////////////////////////////////////////////////////////////
}
