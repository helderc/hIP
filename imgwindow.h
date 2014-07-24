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

#ifndef IMGWINDOW_H
#define IMGWINDOW_H

#include <QWidget>
#include <QImage>
#include <QFileDialog>
#include <QPixmap>
#include <QString>
#include <QCloseEvent>
#include <QDebug>

#include "LabelImage.h"
#include "aba.h"

namespace Ui {
    class ImgWindow;
}

class ImgWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ImgWindow(QWidget *parent = 0);
    ~ImgWindow();

    static ImgWindow * abrirImg(QString arq, QWidget *parent = 0);
    static ImgWindow * abrirImg(const QImage &img, QWidget *parent = 0);
    QImage *getImg1();
    QImage *getImg2();
    LabelImage *getImgLbl();
    QAction *windowMenuAction() const { return action; }
    void upImg();
    void voltarImgOriginal();
    void setActionText(QString str);
    QVector<double> getHistograma();
    void insertNewTab(QWidget *tab, QString title);

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::ImgWindow *ui;

    QString nomeArq;
    LabelImage *lblImg;
    QImage *img1; /**< Imagem original */
    QImage *img2; /**< Imagem que ira sofrer alteracoes. */
    QAction *action;
    QVector<double> histograma;

    void setImg(const QString &arq);
    void setImg(const QImage &im);
    void setOcrMode(bool m);
    void calculaHistograma();


public slots:
    void inserirTexto(QString titulo, QString texto);
    void removeTab(int i);
};

#endif // IMGWINDOW_H
