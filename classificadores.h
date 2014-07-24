#ifndef CLASSIFICADORES_H
#define CLASSIFICADORES_H

#include <QObject>
#include <QVector>
#include <QDebug>

#include <cmath>

class Classificadores : public QObject
{
    Q_OBJECT
public:
    explicit Classificadores(QObject *parent = 0);

    static float classDistEuclidiana(const QVector<float> &v1, const QVector<float> &v2);
    static float classDistChebyshev(const QVector<float> &v1, const QVector<float> &v2);
    static float classDistManhattan(const QVector<float> &v1, const QVector<float> &v2);
    static float classCoefCorrelacao(const QVector<float> &v1, const QVector<float> &v2);

private:
    
signals:
    
public slots:
    
};

#endif // CLASSIFICADORES_H
