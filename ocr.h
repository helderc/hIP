#ifndef OCR_H
#define OCR_H

#include <QObject>
#include <QImage>
#include <QVector>
#include <QDebug>
#include <QFileInfo>
#include <QtCore/qmath.h>
#include <cmath>

#include "thinning.h"
#include "classificadores.h"
#include "ocrcaractere.h"

class OCR : public QObject
{
    Q_OBJECT
public:
    explicit OCR(QObject *parent = 0);

    enum extratorCaracteristicas {
        NENHUM, FREEMAN, MATRIZ_COOCORRENCIA,
        ENTROPIA, UNIFORMIDADE, HOMOGENIEDADE,
        CONTRASTE, CORRELACAO, HISTOGRAMAS,
        DIST_BORDA_CARACTERE
    };

    // Operadores da Matriz de CoOcorrencia
    enum matrizOp {
            DIREITA, ABAIXO,
            DIREITA_ACIMA, DIREITA_ABAIXO,
            DIREITA_ABAIXO_DIAGONAL, DIREITA_ACIMA_DIAGONAL
    };

    // Direcoes da Distancia Borda-Caractere
    enum distBC {
                BC_DIREITA, BC_ESQUERDA, BC_ACIMA, BC_ABAIXO,
                BC_ESQ_DIR_SUB_DIAGONAL, BC_ESQ_DIR_DES_DIAGONAL,
                BC_DIR_ESQ_SUB_DIAGONAL, BC_DIR_ESQ_DES_DIAGONAL
    };

    // METODOS
    void analisePerfilProjecao(const QImage &img);
    void setMetodo(extratorCaracteristicas c);
    void extrairCaracteristicas(extratorCaracteristicas metodo, int k);
    void extrairCaracteristicas(bool teste, extratorCaracteristicas metodo,
                                matrizOp matrizOperador = OCR::DIREITA,
                                distBC bordaCaractereDir = OCR::BC_ABAIXO);

    void quantidadeCaracteres();
    void classificarCaracteres();
    void limparTudo();

private:
    QList<QImage> * subImagens(const QImage &img, const QVector<double> &v, bool orientacao);
    QImage subCaracteresImagens(const QImage &img, const QVector<double> &histVert, const QVector<double> &histHor);
    QVector<double> *perfilProjecao(const QImage &img, bool orientacao);
    void normalizar(QList< QList<QImage> > &listaImgCaracteres, int w, int h);
    void caracteristicasHistogramas(OCRCaractere &c);
    QString caracteristicasFreemanChainCode(const QImage &caractere);
    QHash<QString, QPoint> freemanVizValidos(const QPoint &p, const QPoint p_ant, const QImage &img);
    QHash<QString, int> caracteristicasMatrizCoOcorrencia(const QImage &caractere, matrizOp operador, int vizinhanca = 1);
    int caracteristicasDistBordaCaractere(const QImage &caractere, distBC operador);
    float probabilidadeElemMatrizCoOcorrencia(const QHash<QString, int> &matriz, const QString &e_key);
    float caracteristicasEntropia(const QHash<QString, int> &matriz);
    float caracteristicasUniformidade(const QHash<QString, int> &matriz);
    float caracteristicasHomogeniedade(const QHash<QString, int> &matriz);
    float caracteristicasContraste(const QHash<QString, int> &matriz);
    float caracteristicasCorrelacao(const QHash<QString, int> &matriz);

    // Parses para os Resultados
    void parseResultado(QString titulo, float resultado);
    void parseResultado(QString titulo, QHash<QString, int> resultado);
    void parseResultado(QString titulo, QHash<QString, int> resultado, float valor);
    void parseResultado(QString titulo, QVector<double> resultado);
    void parseResultado(QString titulo, QString resultado);

    // ATRIBUTOS
    extratorCaracteristicas metodoExtracao;
    QVector<double> *linhas;
    QList< QVector<double>* > *listaCaracteres;
    QList<QImage> *subImgLinhas;
    QList< QList<QImage> > *subImgCaracteres;
    QList< QList<QString> > *cFreemanChainCode;
    QList<QObject *> *caracteristicas;

    // Lista dos caracteres
    QList<OCRCaractere *>listaOCRCaractere;
    QList<OCRCaractere *>listaOCRCaractereTreinamento;
    QList<OCRCaractere *>listaOCRCaractereTeste;
    
signals:
    void inserirTexto(QString titulo, QString texto);
    void texto(QString txt);
    
public slots:
    
};

#endif // OCR_H
