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

//! Classe principal responsavel pela criacao da janela principal.
/*!
 * \author Helder C. R. de Oliveira
 * \since 10/10/2010
 * \version 1.0
 */

#include "mainwindow.h"

/**
 * @brief Construtor da classe MainWindow
 *
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent){
    ui.setupUi(this);

    // faz a leitura das configuracoes
    leConfigs();

    // instancia o LabelImage 1
    imgLbl1 = new LabelImage(this);

    // estilo de cor do Background do Label
    imgLbl1->setBackgroundRole(QPalette::Base);

    // alinha o Label na vertical em relacao ao QScrollBar
    imgLbl1->setAlignment(Qt::AlignTop);

    // define o nome do objeto
    //imgLbl1->setObjectName("imgLbl1");

    // Coloca o Label 1 dentro do ScrollArea 1 - Miniatura
    ui.scrollArea1->setWidget(imgLbl1);

    // Cria o Histograma
    criaHist();

    // Conecta o sinal mouseMoving do imgLbl1 (miniatura) com o slot mouseImageMove
    // isso eh usado para pegar as coordenadas e o pixel que esta sob o mouse
    connect(imgLbl1, SIGNAL(mouseMoving(int, int)),
                 this, SLOT(mouseImageMove(int, int)));

    // Pega o momento quando uma SubJanela e' ativada
    connect(ui.mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow *)),
            this, SLOT(subJanelaAtivada()));

    // disposicao das subjanelas
    connect(ui.actionCascata, SIGNAL(triggered()),
            ui.mdiArea, SLOT(cascadeSubWindows()));

    connect(ui.actionTitulo, SIGNAL(triggered()),
            ui.mdiArea, SLOT(tileSubWindows()));

    // adiciona as entradas no menu para controlar os DockWidgets
    ui.menuInfo->addAction(ui.dockImagem->toggleViewAction());
    ui.menuInfo->addAction(ui.dockHistograma->toggleViewAction());
    ui.menuInfo->addAction(ui.dockPixel->toggleViewAction());

    ui.dockPixel->hide();

    windowActionGroup = new QActionGroup(this);

    // inicializa a quantidade de fotos tiradas
    qtdFoto = 0;

    // mostra os formatos de imagens suportados
    // qDebug() << QImageReader::supportedImageFormats();

    // inicializa os valores do eixo X do histograma. QCustomPlot precisa disso...
    histX = new QVector<double>();
    for (int i = 0; i <= 255; i++)
        histX->append(i);

    // FIXME: Nao eh nada legal instanciar essa classe aqui. E se eu nunca usar o OCR?
    ocr = new OCR(this);

    mydb = QSqlDatabase::addDatabase("QSQLITE");
    mydb.setDatabaseName("/home/helder/hip_database.db");

    if (mydb.open())
        qDebug() << "Conectado ao banco.";
    else
        qDebug() << "Nao connectado;";

    qDebug() << "Dir: " << QDir::currentPath();
}




void MainWindow::closeEvent(QCloseEvent *event)
{
    // salva as configuracoes
    salvaConfigs();

    // fecha todas as subjanelas
    ui.mdiArea->closeAllSubWindows();

    if (!ui.mdiArea->subWindowList().isEmpty()) {
        event->ignore();
        return;
    }

    event->accept();
}



/**
 * @brief Configuracoes iniciais para o histograma.
 *
 */
void MainWindow::criaHist()
{
    // give the axes some labels:
    ui.plotHistograma->xAxis->setLabel("Tons de Cinza");
    ui.plotHistograma->yAxis->setLabel("Quantidade de Pixels");
}


/**
 * @brief Atualiza o histograma baseando-se na imagem selecionada.
 *
 */
void MainWindow::upHistograma()
{
    // atualiza o histograma
    ImgWindow *imgwin = janelaAtiva();

    // TODO: Melhorar a visibilidade do histograma. Com algumas imagens nao eh
    // possivel ver os valores proximos do eixo y.

    if (imgwin) {
        QVector<double> h = imgwin->getHistograma();

        // Caso houver, remove o grafico antigo
        ui.plotHistograma->removeGraph(0);

        QCPGraph *g = ui.plotHistograma->addGraph();
        g->setData(*histX, h);
        g->setPen(QPen(Qt::blue));
        g->setLineStyle(QCPGraph::lsImpulse);

        // set axes ranges, so we see all data:
        ui.plotHistograma->xAxis->setRange(0, 255);

        ui.plotHistograma->rescaleAxes();
        ui.plotHistograma->replot();
    }
}


void MainWindow::salvaConfigs()
{
    // salva as configurações de tela
}




void MainWindow::leConfigs()
{
    // Le as configuracoes salvas.
}




/**
 * @brief Metodo que chama o Detector de Arestas de Shen-Castan.
 *
 * @param img Imagem a ser detectada as arestas.
 * @param ratioThresh Taxa de threshold.
 * @param sizeAdapGrad
 * @param smoothFactor Fator de suavizacao.
 * @param thinFactor
 * @param doHysteresis Fazer Histeresi?
 */
void MainWindow::edgeDetectShenCastan (float ratioThresh,
                                       int sizeAdapGrad, float smoothFactor,
                                       int thinFactor, bool doHysteresis)
{
    ImgWindow *imgwin = janelaAtiva();

    qDebug () << "edgeDetectShenCastan";

    if (imgwin) {
        EdgeDetectShenCastan *edge = new EdgeDetectShenCastan(imgwin->getImg2(),
                                                              ratioThresh,
                                                              sizeAdapGrad,
                                                              smoothFactor,
                                                              thinFactor,
                                                              doHysteresis);
        imgUpdate();
        delete edge;
    }
}




/**
 * @brief Metodo que chama o Detector de Arestas de Marr-Hildreth
 *
 * @param img
 * @param desvioPadrao
 */
void MainWindow::edgeDetectMarrHildreth (float desvioPadrao)
{
    ImgWindow *imgwin = janelaAtiva();

    qDebug () << "edgeDetectMarr";

    if (imgwin) {
        qDebug () << "edgeDetectMarr";
        EdgeDetectMarrHill *edge = new EdgeDetectMarrHill(imgwin->getImg2(),
                                                          desvioPadrao);
        imgUpdate();
        delete edge;
    }
}




/**
 * @brief   Esse e' o slot que pega as coordenadas de movimento do mouse.
 *          As coordenadas sao do mouse sobre o LabelImage.
 *
 * @param x Coordenada x do mouse.
 * @param y Coordenada y do mouse.
 */
void MainWindow::mouseImageMove(int x, int y)
{
    // Pega o sender (quem disparou o sinal)
    LabelImage *imgLbl = qobject_cast<LabelImage *>(sender());

    // caso o cast nao pegue nada...
    if (imgLbl == 0)
        qDebug() << "mouseImageMove: Erro no Cast";

    // verifica se as coordenadas do mouse estao dentro da imagem
    if ((x >= imgLbl->width()) || (x <= 0) || (y >= imgLbl->height()) || (y <= 0))
        return;

    /* ======================================================================
        mostra os valores RGB, CMYK e HSV
       ====================================================================== */

    // pega o valor RGB do pixel onde o mouse estah
    QRgb rgb = imgLbl->pixmap()->toImage().pixel(x, y);
    // e transforma em uma cor
    QColor cor(rgb);

    ui.lblRgb->setText("<b>RGB</b>: (" + QString::number(cor.red()) + ", " +
                       QString::number(cor.green())+ ", " +
                       QString::number(cor.blue()) + ")");

    ui.lblCmyk->setText("<b>CMYK</b>: (" + QString::number(cor.cyan()) + ", " +
                        QString::number(cor.magenta())+ ", " +
                        QString::number(cor.yellow()) + ", " +
                        QString::number(cor.black()) + ")");

    ui.lblHsv->setText("<b>HSV</b>: (" + QString::number(cor.hue()) + ", " +
                       QString::number(cor.saturation())+ ", " +
                       QString::number(cor.value()) + ")");

    ui.wdgPixelCor->setStyleSheet(QString("background-color: rgb(%1, %2, %3);")
                                  .arg(cor.red())
                                  .arg(cor.green())
                                  .arg(cor.blue()));
}



/**
 * @brief Coloca uma MSG na barra de Status.
 *
 * @param msg Mensagem a ser apresentada.
 */
void MainWindow::msgStatus(QString msg)
{
    ui.statusBar->showMessage(msg, 0);
}




/**
 * @brief Chamada para Deteccao de Arestas por Canny
 *
 * @param img
 * @param desvPadrao
 * @param thresLow
 * @param thresHigh
 * @param usarOpenCv
 */
void MainWindow::edgeDetectCanny(int desvPadrao,
                                 int thresLow,
                                 int thresHigh,
                                 bool usarOpenCv)
{
    ImgWindow *imgwin = janelaAtiva();

    qDebug () << "edgeDetectCanny";

    if (imgwin) {
        if (usarOpenCv) {
            // TODO: Acessar o Canny do OpenCV
            //cvCanny();
        }
        else {
            EdgeDetectCanny *edge = new EdgeDetectCanny(imgwin->getImg2(),
                                                        desvPadrao,
                                                        thresLow,
                                                        thresHigh);
            imgUpdate();
            delete edge;
        }
    }
}




/**
 * @brief Faz a atualizacao da imagem de miniatura e a imagem em tamanho natural.
 *      Apos qualquer processamento, essa funcao deve ser chamada para que a
 *      atualizacao seja visivel a nivel de interface.
 */
void MainWindow::imgUpdate()
{
    ImgWindow *imgwin = janelaAtiva();

    if (imgwin) {
        // atualiza a imagem da janela ativa
        imgwin->upImg();

        ui.lblTam->setText(QString::fromUtf8("<b>Size</b>: ") +
                           QString::number(imgwin->getImg2()->width()) +
                           "x" + QString::number(imgwin->getImg2()->height()));

        // Atualiza o Histograma
        upHistograma();
    }

}




/**
 * @brief Evento do botao "Sair"
 *
 */
void MainWindow::on_btnSair_triggered()
{
    close();
}




/**
 * @brief Evento do botao "Abrir Imagem"
 *
 */
void MainWindow::on_btnAbrirImg_triggered()
{
    // caixa de abertura de arquivo
    fileName = QFileDialog::getOpenFileName(this, tr("Abrir Imagem"),
                                            ultimoDir);

    if (!fileName.isEmpty()) {
        QDir dir;
        // guarda o local do arquivo aberto
        ultimoDir = dir.absoluteFilePath(fileName);

        qDebug() << "atualiza a cena 2";
        imgUpdate();

        qDebug() << "abre a imagem em uma subjanela";
        abrirImg(fileName);
    }
}




/**
 * @brief
 *
 */
void MainWindow::on_btnSobre_triggered()
{
    QMessageBox::about(this, tr("hIP"),
                       QString::fromUtf8("<p><b><h1>hIP</h1>"
                          "hiper<br>"
                          "Image<br>"
                          "Processing</b></p><br>"
                          "<center><b>UNESP</b> - Universidade Estadual Paulista \"Julio de Mesquita Filho\"<br>"
                          "Campus de Presidente Prudente<br>"
                          "Faculdade de Ciencias e Tecnologia</center><br><br>"
                          "Escrito por: <b>Helder C. R. Oliveira</b><br>"
                          "sob a orientacao do professor: <b>Marco A. Piteri</b><br>"
                          "<br>"
                          "Visite minha home page para maiores informacoes:<br>"
                          "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"
                          "<a href=\"http://helderc.net\">http://helderc.net</a>"
                          "<br><br>"
                          "Essas pessoas colaboraram com o sistema:<br>"
                          "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; - <b>Eu</b>"));
}




/**
 * @brief Medicao de rui­dos.
 *
 */
void MainWindow::on_actionMedirRuidos_triggered()
{
    ImgWindow *imgwin = janelaAtiva();

    if (imgwin) {
        QString *str = OpCommons::noiseMeter(imgwin->getImg1());

        QMessageBox::information(this,
                                 QString::fromUtf8("hIP - Noise meter"),
                                 *str);
    }
    else
        QMessageBox::critical(this,
                             tr("Erro!"),
                             tr("Ocorreu um erro!"));
}




/**
 * @brief
 *
 */
void MainWindow::on_actionCanny_triggered()
{
    int desvPadrao, thresLow, thresHigh;

    desvPadrao = thresLow = thresHigh = 0;

    // Colocar um regulador para o primeiro parametro
    // int value = 0, int min = -2147483647, int max = 2147483647, int step = 1

    // pede um valor de desvio padrao da gaussiana
    desvPadrao = QInputDialog::getInt(this,
                                      "Default Deviation",
                                      "Choose the default deviation value:",
                                      1, 1, 5);

    // Lower threshold
    thresLow = QInputDialog::getInt(this,
                                        tr("Lower Threshold Value"),
                                        tr("Choose the lower threshold value:"),
                                        0, -5, 5);

    // High threshold
    thresHigh = QInputDialog::getInt(this,
                                         tr("Higher Threshold Value"),
                                         tr("Choose the higher threshold value:"),
                                         -1, -5, 5);

    // chama o algoritmo
    edgeDetectCanny(desvPadrao, thresLow, thresHigh, 0);
}




/**
 * @brief
 *
 */
void MainWindow::on_actionTons_de_Cinza_triggered()
{
    qDebug() << "on_actionTons_de_Cinza_triggered";

    ImgWindow *imgwin = janelaAtiva();

    if (imgwin) {
        Filtros::filtroTomCinza(imgwin->getImg2());

        // atualiza a visualizacao
        imgUpdate();
    }
}




/**
 * @brief
 *
 */
void MainWindow::on_actionMarr_Hildreth_triggered()
{
    int desvPadrao = 2;

    // Colocar um regulador para o primeiro parametro

    // pede um valor de desvio padrao
    desvPadrao = QInputDialog::getInt(this, QString::fromUtf8("Default Deviation"),
                                 QString::fromUtf8("Insert default deviation:"),
                                 2, 1, 5);

    edgeDetectMarrHildreth(desvPadrao);
}




/**
 * @brief Restaura a imagem original.
 *
 */
void MainWindow::on_actionDesfazer_triggered()
{
    ImgWindow *imgwin = janelaAtiva();

    if (imgwin) {
        imgwin->voltarImgOriginal();

        // atualiza a subjanela
        imgUpdate();
    }
}


/**
 * @brief
 *
 * @param
 */
void MainWindow::timerEvent(QTimerEvent *)
{
    IplImage *cvimage = cvQueryFrame(camera);

    QImage image = OpCommons::IplImageToQImage(cvimage);

    // coloca a miniatura da webcam
    imgLbl1->setPixmap(QPixmap::fromImage(image).scaled(QSize(196, 196),
                                                        Qt::KeepAspectRatio));

    intTimer++;

    switch (intTimer) {
        case 10:
            qDebug() << "5 segs";
            ui.lblContagem->setText(contagemRegressiva(5));
            break;
        case 20:
            qDebug() << "4 segs";
            ui.lblContagem->setText(contagemRegressiva(4));
            break;
        case 30:
            qDebug() << "3 segs";
            ui.lblContagem->setText(contagemRegressiva(3));
            break;
        case 40:
            qDebug() << "2 segs";
            ui.lblContagem->setText(contagemRegressiva(2));
            break;
        case 50:
            qDebug() << "1 seg";
            ui.lblContagem->setText(contagemRegressiva(1));
            break;
        case 60:
            // a foto eh tirada aqui
            qDebug() << "0 seg";
            ui.lblContagem->setText(contagemRegressiva(0));
            ui.btnCapCam->setDown(false);

            QImage *im = new QImage(image.convertToFormat(QImage::Format_RGB32));

            // cria a janela da imagem
            ImgWindow *iw = ImgWindow::abrirImg(*im, this);

            iw->setActionText(QString("Foto #%1").arg(qtdFoto));

            // TODO: titulo da janela = nome do arquivo
            iw->setWindowTitle(QString("Foto #%1").arg(qtdFoto));

            // metodo para adicionar a janela no MDI
            addImgWindow(iw);

            // conecta o sinal do movimento do mouse sbre a imagem aberta
            connect(iw->getImgLbl(), SIGNAL(mouseMoving(int, int)),
                    this, SLOT(mouseImageMove(int, int)));

            // o imgUpdate mostra a foto de img2 no lblImg2
            imgUpdate();
            // para o Timer
            killTimer(idTimer);
            // libera a webcam
            cvReleaseCapture(&camera);
    }
}




/**
 * @brief Evento de click do botao para captura da webcam.
 *
 */
void MainWindow::on_btnCapCam_clicked()
{
    // inicializa a cam. CV_CAP_ANY pega a cam default 0
    camera = cvCaptureFromCAM(CV_CAP_ANY);

    // verificando se a cam foi inicializada
    if (camera != NULL) {
        idTimer = startTimer(100);
        ui.btnCapCam->setDown(true);
        intTimer = 0;
        // incrementa a quantidade de fotos
        qtdFoto++;
    }
    else
        QMessageBox::critical(this, "Erro",
                              QString::fromUtf8("O dispositivo pode estar "
                                            "desconectado ou \nesta sendo usado no "
                                            "momento."), QMessageBox::Ok);
}




/**
 * @brief
 *
 */
void MainWindow::on_actionSobelX_triggered()
{
    qDebug() << "Mascara de Sobel: G_x";

    ImgWindow *imgwin = janelaAtiva();

    if (imgwin) {
        Filtros::filtroSobel(imgwin->getImg2(), 'x');
        imgUpdate();
    }
}




/**
 * @brief
 *
 */
void MainWindow::on_actionSobelY_triggered()
{
    qDebug() << "Mascara de Sobel: G_y";

    ImgWindow *imgwin = janelaAtiva();

    if (imgwin) {
        Filtros::filtroSobel(imgwin->getImg2(), 'y');
        imgUpdate();
    }
}




/**
 * @brief
 *
 */
void MainWindow::on_actionSobelAmbos_triggered()
{
    qDebug() << "Mascara de Sobel: G_x e G_y";

    ImgWindow *imgwin = janelaAtiva();

    if (imgwin) {
        Filtros::filtroSobel(imgwin->getImg2(), 'a');
        imgUpdate();
    }
}




/**
 * @brief
 *
 */
void MainWindow::on_actionSalvar_Imagem_2_triggered()
{
    ImgWindow *imgwin = janelaAtiva();

    if (imgwin) {
        QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Salvar Imagem"),
                                                    ultimoDir,
                                                    "Images (*.png)");
        QImage *img2 = imgwin->getImg2();

        if (!(fileName.isNull())) {
            img2->save(fileName + ".png", "PNG");
            qDebug () << "Save: " << fileName << ".png";
        }
    }
}




/**
 * @brief
 *
 */
void MainWindow::on_actionShen_Castan_triggered()
{
    /* Conteudo do arquivo de parametros
    0.035  .91  10   1  1
    */
/*
    // High threshold
    thresHigh = QInputDialog::getInt(this,
                                         "Higher Threshold Value",
                                         "Choose the higher threshold value:",
                                         -1, -5, 5);
*/
    edgeDetectShenCastan(0.035, 0.91, 10, 1, 1);

    qDebug () << "Shen-Castan - FEITO!";
}




/**
 * @brief
 *
 */
void MainWindow::on_actionSobre_o_Qt_triggered()
{
    QMessageBox::aboutQt(this, "Sobre o Qt...");
}




/**
 * @brief
 *
 */
void MainWindow::on_actionNegativo_triggered()
{
    ImgWindow *imgwin = janelaAtiva();

    if (imgwin) {
        imgwin->getImg2()->invertPixels();
        imgUpdate();
    }
}



/**
 * @brief
 *
 */
void MainWindow::on_actionCanny_OpenCV_triggered()
{
    // o imgUpdate mostra a foto de img2 no lblImg2
    imgUpdate();
}




/**
 * @brief Clique da entrada do menu "Equalizar Histograma"
 *
 */
void MainWindow::on_actionEqualizar_Histograma_triggered()
{
    ImgWindow *imgwin = janelaAtiva();

    if (imgwin) {
        OpCommons::eqHist(imgwin->getImg2());
        imgUpdate();
    }
}




/**
 * @brief Clique do botao "Salvar Histograma"
 *
 */
void MainWindow::on_btnSalvarHist_clicked()
{
    QString arqExt = ".png";
    QString arqAssoc = "Imagem PNG (*.png )";

    if (ui.rdbJPG->isChecked()) {
        arqExt = ".jpg";
        arqAssoc = "Imagem JPEG (*.jpg)";
    }
    if (ui.rdbPDF->isChecked()) {
        arqExt = ".pdf";
        arqAssoc = "Imagem PDF (*.pdf)";
    }

    QString arq = QFileDialog::getSaveFileName(this,
                                               tr("Nome do arquivo"),
                                               "arq", arqAssoc);

    // Salva o histograma
    if (!arq.isEmpty()) {
        if (ui.rdbJPG)
            ui.plotHistograma->saveJpg(arq + "_Hist" + arqExt);
        else if (ui.rdbPDF)
            ui.plotHistograma->savePdf(arq + "_Hist" + arqExt);
        else
            ui.plotHistograma->savePng(arq + "_Hist" + arqExt);
    }
}




/**
 * @brief Clique da entrada do menu "Tom Sepia"
 *
 */
void MainWindow::on_actionS_pia_triggered()
{
    ImgWindow *imgwin = janelaAtiva();

    if (imgwin) {
        Filtros::filtroSepia(imgwin->getImg2());
        imgwin->upImg();
    }
}




void MainWindow::on_actionGaussiana_triggered()
{
    ImgWindow *imgwin = janelaAtiva();

    if (imgwin) {
        Filtros::filtroGauss(imgwin->getImg2());
        imgUpdate();
    }
}



void MainWindow::addImgWindow(QWidget *win)
{
    // cria uma subjanela com a imagem aberta
    QMdiSubWindow *subWindow = ui.mdiArea->addSubWindow(win);

    ImgWindow *imgwin = dynamic_cast<ImgWindow *>(win);

    if (imgwin) {
        qDebug() << "Img Window";

        // adiciona a janela ao menu Janelas
        ui.menuJanela->addAction(imgwin->windowMenuAction());
        ui.menuJanela->addAction(imgwin->windowMenuAction());

        // o actionGroup garante que apenas uma janela seja selecionada
        windowActionGroup->addAction(imgwin->windowMenuAction());

        // deixa checado a entrada da janela no menu Janela
        imgwin->windowMenuAction()->setChecked(true);

        upHistograma();

        subWindow->show();
    }
    else {
        qDebug() << "OCR Window";

        OCRWindow *ocrwin = dynamic_cast<OCRWindow *>(win);
        ocrwin->setWindowTitle("OCR");

        // adiciona a janela ao menu Janelas
        ui.menuJanela->addAction(ocrwin->windowMenuAction());
        ui.menuJanela->addAction(ocrwin->windowMenuAction());

        // o actionGroup garante que apenas uma janela seja selecionada
        windowActionGroup->addAction(ocrwin->windowMenuAction());

        // deixa checado a entrada da janela no menu Janela
        ocrwin->windowMenuAction()->setChecked(true);

        subWindow->showMaximized();
    }
}




void MainWindow::abrirImg(QString arq)
{
    // cria a janela da imagem
    ImgWindow *iw = ImgWindow::abrirImg(arq, this);

    // titulo da janela = nome do arquivo
    iw->setWindowTitle(arq);

    // metodo para adicionar a janela no MDI
    addImgWindow(iw);

    // conecta o sinal do movimento do mouse sbre a imagem aberta
    connect(iw->getImgLbl(), SIGNAL(mouseMoving(int, int)),
            this, SLOT(mouseImageMove(int, int)));

    // Adiciona uma nova aba com as caracteristicas extraidas
    connect(this->ocr, SIGNAL(inserirTexto(QString,QString)),
            iw, SLOT(inserirTexto(QString,QString)));
}




/**
 * @brief Retorna o ponteiro para a SubJanela ativa.
 *      Caso nÃ£o haja janela, o retorno eh 0.
 *
 * @return ImgWindow * Ponteiro para a subjanela ativa.
 */
ImgWindow * MainWindow::janelaAtiva()
{
    // currentSubWindow eh Melhor que activeSubWindow
    QMdiSubWindow *subWindow = ui.mdiArea->currentSubWindow();

    if (subWindow)
        return qobject_cast<ImgWindow *>(subWindow->widget());
    return 0;
}




QImage * MainWindow::imagemAtiva()
{
    ImgWindow *img = janelaAtiva();

    if (img)
        return img->getImg2();
    return 0;
}




void MainWindow::setMiniatura()
{
    ImgWindow *imgwin = janelaAtiva();
    QPixmap imgpix;

    if (imgwin) {
        // faz a escala para o widget de tamanho 198x198
        // eh necessario colocar para 196x196 porque 1 pixel eh de cada borda
        imgpix = QPixmap::fromImage(*(imgwin->getImg1()))
                                    .scaled(QSize(196, 196),
                                    Qt::KeepAspectRatio);

        // remove a imagem do Lbl1 (quadro da miniatura)
        imgLbl1->clear();

        // define a nova imagem
        imgLbl1->setPixmap(imgpix);
    }
}



/**
 * @brief Slot acoplado ao sinal de subWindowActivated. Disparado toda
 *      vez que uma subjanela eh ativada.
 */
void MainWindow::subJanelaAtivada()
{
    // atualiza a area de miniatura com a img da janela atual.
    setMiniatura();

    // atualiza o histograma
    upHistograma();
}



void MainWindow::on_actionCarton_triggered()
{
    naoImplementado();
    // TODO: Carton

    /*
     ImgWindow *imgwin = janelaAtiva();

    if (imgwin) {
        Filtros::filtroCarton(imgwin->getImg2());
        imgwin->upImg();
    }*/
}



void MainWindow::on_actionAnalise_de_Projecao_triggered()
{
    ImgWindow *imgwin = janelaAtiva();

    if (imgwin) {
        // envia para analise dos perfis horizonhal e vertical
        ocr->analisePerfilProjecao(*imgwin->getImg2());
    }
}

void MainWindow::on_actionMatriz_de_Coocorrencia_triggered()
{
    //ocr->extrairCaracteristicas(OCR::MATRIZ_COOCORRENCIA);
}

void MainWindow::on_actionChain_Code_de_Freeman_triggered()
{
    //ocr->extrairCaracteristicas(OCR::FREEMAN);
}

void MainWindow::on_actionHistogramas_triggered()
{
    //ocr->extrairCaracteristicas(OCR::HISTOGRAMAS);
}

void MainWindow::on_actionDistancia_Borda_caractere_triggered()
{
    //ocr->extrairCaracteristicas(OCR::DIST_BORDA_CARACTERE);
}


void MainWindow::on_actionEntropia_triggered()
{
    //ocr->extrairCaracteristicas(OCR::ENTROPIA);
}


void MainWindow::on_actionUniformidade_triggered()
{
    //ocr->extrairCaracteristicas(OCR::UNIFORMIDADE);
}


void MainWindow::on_actionHomogeniedade_triggered()
{
    //ocr->extrairCaracteristicas(OCR::HOMOGENIEDADE);
}

void MainWindow::on_actionContraste_triggered()
{
    //ocr->extrairCaracteristicas(OCR::CONTRASTE);
}

void MainWindow::on_actionCorrela_ao_triggered()
{
    //ocr->extrairCaracteristicas(OCR::CORRELACAO);
}

void MainWindow::on_actionZhang_Suen_triggered()
{
    ImgWindow *imgwin = janelaAtiva();

    if (imgwin) {
        Thinning::Binarizacao(imgwin->getImg2());
        Thinning::ZhanSuen(imgwin->getImg2());
        imgUpdate();
    }
}

void MainWindow::on_actionTeste_2_triggered()
{
    QSqlQuery qry;//("select * from A;");

    if (mydb.isOpen())
        if (qry.exec("SELECT * FROM A")) {
            while (qry.next())
                qDebug() << ">>> " + qry.value(0).toString() + " -> " + qry.value(1).toString();
        }
}

void MainWindow::on_actionJanela_OCR_triggered()
{

    OCRWindow *ocrwin = new OCRWindow(this);
    addImgWindow(ocrwin);

    ui.dockPixel->hide();
    ui.dockHistograma->hide();
    ui.dockImagem->hide();
}

void MainWindow::on_actionDistancia_Manhattan_triggered()
{

}
