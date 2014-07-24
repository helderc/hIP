#include "classificadores.h"

Classificadores::Classificadores(QObject *parent) :
    QObject(parent)
{
}


/**
 * @brief Distancia Euclidiana entre dois vetores, v1 e v2.
 *          Os dois vetores precisam ter a mesma dimensao.
 *
 *          d(v1, v2) = sqrt( (v1[0,0] - v2[0,0])^2 + (v1[1,0] - v2[1,0])^2);
 *
 * @param v1
 * @param v2
 *
 * @return Quanto menor mais proximo.
 *         Se for 0 os vetores sao iguais! Nunca retorna < 0.
 *         Se retorna NULL eh pq os vetores tem tamanhos diferentes
 */
float Classificadores::classDistEuclidiana(const QVector<float> &v1, const QVector<float> &v2)
{
    if (v1.size() != v2.size()) {
        qDebug() << "Vetores de tamanhos diferentes!";
        return (float)NULL;
    }

    // Somatorio dos quadrados das diferencas.
    // (v1[i,j] - v2[i,j])^2
    float somatorio = 0;
    for (int i = 0; i < v1.size(); i++) {
        float sub_e = v1.at(i) - v2.at(i);
        somatorio += sub_e * sub_e;
    }

    return sqrt(somatorio);
}


/**
 * @brief Distancia de Chebyshev entre dois vetores, v1 e v2.
 *          Os dois vetores precisam ter a mesma dimensao.
 *
 *          d_cheb(v1, v2) = max_i(| v1[i] - v2[i] |)
 *
 * @param v1
 * @param v2
 *
 * @return Quanto menor mais proximo.
 *         Se for 0 os vetores sao iguais! Nunca retorna < 0.
 *         Se retorna NULL eh pq os vetores tem tamanhos diferentes
 */
float Classificadores::classDistChebyshev(const QVector<float> &v1, const QVector<float> &v2)
{
    if (v1.size() != v2.size()) {
        qDebug() << "Vetores de tamanhos diferentes!";
        return (float)NULL;
    }

    // Buscando a maior diferenca
    float maior_diff = 0;
    for (int i = 0; i < v1.size(); i++) {
        float diff_abs = abs(v1.at(i) - v2.at(i));
        if (diff_abs > maior_diff)
            maior_diff = diff_abs;
    }

    return maior_diff;
}


/**
 * @brief Distancia Manhattan entre os vetores v1 e v2.
 *
 *          d_v1v2 = SUM_{i=0}^{n} (|v1[i] - v2[i]|)
 *
 * @param v1
 * @param v2
 *
 * @return Quanto mais proximo de zero mais similar.
 *          Caso len(v1) != len(v2) retorna NULL.
 */
float Classificadores::classDistManhattan(const QVector<float> &v1, const QVector<float> &v2)
{
    if (v1.size() != v2.size()) {
        qDebug() << "Vetores de tamanhos diferentes!";
        return (float)NULL;
    }

    float dist = 0;
    for (int i = 0; i < v1.size(); i++) {
        dist += abs(v1.at(i) - v2.at(i));
    }

    return dist;
}


/**
 * @brief Coeficiente de Correlacao entre os vetores v1 e v2.
 *
 *          coef_v1v2 = SUM_{k=0}^{n}( (v1[k] - v1_m) * (v2[k] - v2_m) )   /
 *                      SQRT( SUM_{k=0}^{n}(v1[k] - v1_m)^2 * SUM_{k=0}^{n}(v2[k] - v2_m)^2 )
 *
 * @param v1
 * @param v2
 *
 * @return O resultado varia entre -1 e 1.
 *          Quanto mais proximo de 1, mais similar são os vetores.
 *          Retorna NULL para indicar o tamanho diferente dos vetores.
 */
float Classificadores::classCoefCorrelacao(const QVector<float> &v1, const QVector<float> &v2)
{
    if (v1.size() != v2.size()) {
        qDebug() << "Vetores de tamanhos diferentes!";
        return (float)NULL;
    }

    float soma_v1 = 0;
    float soma_v2 = 0;
    for (int i = 0; i < v1.size(); i++) {
        soma_v1 += v1.at(i);
        soma_v2 += v2.at(i);
    }

    float media_v1 = soma_v1 / v1.size();
    float media_v2 = soma_v2 / v2.size();

    // Numerador
    float soma_numerador = 0;
    for (int i = 0; i < v1.size(); i++) {
        soma_numerador += (v1.at(i) - media_v1) * (v2.at(i) - media_v2);
    }

    // Denominador
    soma_v1 = soma_v2 = 0;
    for (int i = 0; i < v1.size(); i++) {
        float v1_sub = v1.at(i) - media_v1;
        soma_v1 += v1_sub * v1_sub;

        float v2_sub = v2.at(i) - media_v2;
        soma_v2 += v2_sub * v2_sub;
    }

    // Calculando a raiz do denominador
    float raiz_den = sqrt(soma_v1 * soma_v2);

    return soma_numerador / raiz_den;
}
