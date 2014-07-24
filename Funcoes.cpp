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

#include "Funcoes.h"

/**
 * @brief
 *
 * @param size
 * @param align
 * @return int
 */
inline int align(int size, int align)
{
    return (size + align - 1) & -align;
}


/**
 * @brief Define o label da contagem regressiva. Acendendo um numero e apagando
 *  outro.
 *
 * @param num Numero a ser aceso;
 * @return QString O codigo HTML completo para o label.
 */
QString contagemRegressiva (int num)
{
    QString str ("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\""
            "\"http://www.w3.org/TR/REC-html40/strict.dtd\">"
            "<html><head><meta name=\"qrichtext\" content=\"1\" />"
            "<style type=\"text/css\"> p, li { white-space: pre-wrap; } </style>"
            "</head><body style=\" font-family:'Sans'; font-size:9pt;"
            "font-weight:400; font-style:normal;\">"
            "<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px;"
            "margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
            "<span style=\" font-size:12pt; color: %1;\">5</span>"
            "<span style=\" font-size:12pt;\">, </span>"
            "<span style=\" font-size:12pt; color: %2;\">4</span>"
            "<span style=\" font-size:12pt;\">, </span>"
            "<span style=\" font-size:12pt; color: %3;\">3</span>"
            "<span style=\" font-size:12pt;\">, </span>"
            "<span style=\" font-size:12pt; color: %4;\">2</span>"
            "<span style=\" font-size:12pt;\">, </span>"
            "<span style=\" font-size:12pt; color: %5;\">1</span>"
            "</p></body></html>");

    switch (num) {
        case 5:
            str = str.arg("#000000", "#c0c0c0", "#c0c0c0", "#c0c0c0", "#c0c0c0");
            break;
        case 4:
            str = str.arg("#c0c0c0", "#000000", "#c0c0c0", "#c0c0c0", "#c0c0c0");
            break;
        case 3:
            str = str.arg("#c0c0c0", "#c0c0c0", "#000000", "#c0c0c0", "#c0c0c0");
            break;
        case 2:
            str = str.arg("#c0c0c0", "#c0c0c0", "#c0c0c0", "#000000", "#c0c0c0");
            break;
        case 1:
            str = str.arg("#c0c0c0", "#c0c0c0", "#c0c0c0", "#c0c0c0", "#000000");
            break;
        case 0:
            str = str.arg("#c0c0c0", "#c0c0c0", "#c0c0c0", "#c0c0c0", "#c0c0c0");
    }

    return str;
}


void naoImplementado()
{
    QMessageBox::information(NULL, "Nao implementado",
                             "Funcao nao implementada!",
                             QMessageBox::Ok);
}
