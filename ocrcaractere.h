#ifndef OCRCARACTERE_H
#define OCRCARACTERE_H

#include <QObject>
#include <QDebug>
#include <QVector>
#include <QImage>

#include "classificadores.h"

class OCRCaractere : public QObject
{
    Q_OBJECT
public:
    explicit OCRCaractere(QObject *parent = 0);
    explicit OCRCaractere(QImage caractere, QString s_id = "", QObject *parent = 0);

    void setImagem(QImage img);
    QImage imagem();

    void setId(QString s) { s_id = s; }
    QString id() { return s_id; }

    void setHistograma(QVector<double> *hist, bool h = 0);
    const QVector<double>& histograma(bool h = 0);

    void setMatrizCoOcorrencia(QHash<QString, int> m);
    QHash<QString, int> matrizCo() { return matrizCoOcorrencia; }

    void setDistBordaCaractere(int dist) { dist_borda_caractere = dist; }
    int distBordaCaractere() { return dist_borda_caractere; }

    void setEntropia(float f) { f_entropia = f; }
    float entropia() { return f_entropia; }

    void setUniformidade(float f) { f_uniformidade = f; }
    float uniformidade() { return f_uniformidade; }

    void setHomogeniedade(float f) { f_homogeniedade = f; }
    float homogeniedade() { return f_homogeniedade; }

    void setContraste(float f) { f_contraste = f; }
    float contraste() { return f_contraste; }

    void setCorrelacao(float f) { f_correlacao = f; }
    float correlacao() { return f_correlacao; }


private:
    QImage img_c;
    QString s_id;

    QVector<double> *hist_hor;
    QVector<double> *hist_ver;
    QHash<QString, int> matrizCoOcorrencia;

    int dist_borda_caractere;
    float f_entropia;
    float f_uniformidade;
    float f_homogeniedade;
    float f_contraste;
    float f_correlacao;

    
signals:
    
public slots:
    
};

#endif // OCRCARACTERE_H
