/***************************************************************************

Copyright (c) 2014, Helder C. R. de Oliveira
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

* Neither the name of the {organization} nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

***************************************************************************/

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
