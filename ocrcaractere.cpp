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
