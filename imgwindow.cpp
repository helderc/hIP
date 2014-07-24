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

#include "imgwindow.h"
#include "ui_imgwindow.h"

ImgWindow::ImgWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImgWindow)
{
    ui->setupUi(this);

    // Action necessaria para a entrada da SubJanela no menu Janela
    action = new QAction(this);
    action->setCheckable(true);

    connect(action, SIGNAL(triggered()), this, SLOT(show()));
    connect(action, SIGNAL(triggered()), this, SLOT(setFocus()));

    // Controla o Clique no botao de fechar aba
    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)),
            this, SLOT(removeTab(int)));

    lblImg = new LabelImage(this);

    // estilo de cor do Background do Label
    lblImg->setBackgroundRole(QPalette::Base);

    // alinha o Label na vertical em relacao ao QScrollBar
    lblImg->setAlignment(Qt::AlignTop);

    ui->tabWidget->setVisible(false);

    histograma = QVector<double>(256, 0);

    // faz a subwin ser liberada quando fechada
    setAttribute(Qt::WA_DeleteOnClose);
}

ImgWindow::~ImgWindow()
{
    delete ui;
}

ImgWindow * ImgWindow::abrirImg(QString arq, QWidget *parent)
{
    ImgWindow *imgwin = new ImgWindow(parent);

    imgwin->setImg(arq);

    return imgwin;
}

ImgWindow * ImgWindow::abrirImg(const QImage &img, QWidget *parent)
{
    ImgWindow *imgwin = new ImgWindow(parent);

    imgwin->setImg(img);

    return imgwin;
}

void ImgWindow::setImg(const QImage &im)
{
    img1 = new QImage(im);
    img2 = new QImage(im);

    // coloca para visualizacao a nova img
    lblImg->setPixmap(QPixmap::fromImage(*img2));

    ui->scrollArea->setWidget(lblImg);

    calculaHistograma();
}

void ImgWindow::setImg(const QString &arq)
{
    nomeArq = arq;

    // remove a img atual
    lblImg->clear();

    // carrega o arquivo na Imagem - img1: img original
    img1 = new QImage(nomeArq);
    // imagem que sofrera alteracoes
    img2 = new QImage(nomeArq);

    // Se o formato nao for RGB32 entao converte
    /*if (img->format() != QImage::Format_RGB32) {
        img = &(img->convertToFormat(QImage::Format_RGB32));
    }*/

    // coloca para visualizacao a nova img
    lblImg->setPixmap(QPixmap::fromImage(*img2));

    ui->scrollArea->setWidget(lblImg);
    action->setText(QFileInfo(nomeArq).fileName());

    // o formato da imagem eh sempre RGB32
    //qDebug() << "Formato da img1: " << QImage::Format_RGB32;

    calculaHistograma();
}

QImage * ImgWindow::getImg1()
{
    return img1;
}

QImage * ImgWindow::getImg2()
{
    return img2;
}

void ImgWindow::upImg()
{
    // limpa a cena
    lblImg->clear();

    // coloca para visualizacao a nova img
    lblImg->setPixmap(QPixmap::fromImage(*img2));


    //ui.lblTam->setText(QString::fromUtf8("<b>Size</b>: ") +
    //                   QString::number(imgTam.width()) +
    //                   "x" + QString::number(imgTam.height()));
    //
    // Atualiza o Histograma
    calculaHistograma();
}

void ImgWindow::closeEvent(QCloseEvent *event)
{
    event->accept();

    // TODO: verificacao para oferecer o "Salvar alteracoes"
    /*if (okToContinue()) {
        event->accept();
    } else {
       event->ignore();
    }*/
}

void ImgWindow::setOcrMode(bool m)
{
    ui->tabWidget->setVisible(m);
}

// Calcula o histograma baseado na imagem que sempre sofrera as alteracoes
void ImgWindow::calculaHistograma()
{
    int cinza;

    histograma = QVector<double>(256, 0);

    for (int x = 0; x < img2->width(); x++) {
        for (int y = 0; y < img2->height(); y++) {
            cinza = qGray(img2->pixel(QPoint(x,y)));
            histograma[cinza] += 1;
        }
    }
}

void ImgWindow::insertNewTab(QWidget *tab, QString title)
{
    ui->tabWidget->addTab(tab, title);
    ui->tabWidget->setVisible(true);
    ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(tab));
}

void ImgWindow::inserirTexto(QString titulo, QString texto)
{
    int qtdade_abas = ui->tabWidget->count();

    Aba *aba = NULL;

    for (int i = 0; i < qtdade_abas; i++) {
        if (ui->tabWidget->tabText(i) == titulo) {
            aba = (Aba*) ui->tabWidget->widget(i);
            break;
        }
    }

    // A aba nao existe, eh necessario cria-la
    if (!aba) {
        aba = new Aba(ui->tabWidget);
        insertNewTab(aba, titulo);
    }

    // Se chegou aqui, a aba ja existe
    aba->inserirTexto(texto);
}

void ImgWindow::removeTab(int i)
{
    QWidget *aba = ui->tabWidget->widget(i);
    ui->tabWidget->removeTab(i);

    // O Widget da Aba nao eh excluido quando ela eh fechada
    delete aba;

    // Oculta o widget quando todas as abas forem fechadas
    if (ui->tabWidget->count() == 0)
        ui->tabWidget->setVisible(false);
}

void ImgWindow::voltarImgOriginal()
{
    *img2 = *img1;
}

void ImgWindow::setActionText(QString str)
{
    action->setText(str);
}

QVector<double> ImgWindow::getHistograma()
{
    return histograma;
}

LabelImage * ImgWindow::getImgLbl()
{
    return lblImg;
}


