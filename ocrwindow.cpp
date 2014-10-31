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

#include "ocrwindow.h"
#include "ui_ocrwindow.h"

OCRWindow::OCRWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OCRWindow)
{
    ui->setupUi(this);

    // Action necessaria para a entrada da SubJanela no menu Janela
    action = new QAction(this);
    action->setCheckable(true);
    action->setText("OCR");

    connect(action, SIGNAL(triggered()), this, SLOT(show()));
    connect(action, SIGNAL(triggered()), this, SLOT(setFocus()));

    ocr = new OCR(this);

    connect(ocr, SIGNAL(texto(QString)), ui->txtLog, SLOT(appendPlainText(QString)));
}

OCRWindow::~OCRWindow()
{
    delete ui;
}

void OCRWindow::setActionText(QString str)
{
    action->setText(str);
}

void OCRWindow::on_btnOCRAbrirImg_clicked()
{
    // caixa de abertura de arquivo
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Abrir Imagem"));

    if (!fileName.isEmpty()) {
        img_teste = new QImage(fileName);
        imgUpdate();
    }
}

void OCRWindow::texto(QString txt)
{
    //ui->txtLog->ins
}

void OCRWindow::imgUpdate()
{
    // limpa a cena
    ui->lblImg->clear();

    // coloca para visualizacao a nova img
    ui->lblImg->setPixmap(QPixmap::fromImage(*img_teste));
}

void OCRWindow::on_btnTreinar_clicked()
{
    chamarExtratores(true);
}

void OCRWindow::on_btnAnaliseProjecao_clicked()
{
    ocr->limparTudo();
    ocr->analisePerfilProjecao(*img_teste);
}

void OCRWindow::on_btnClassificar_clicked()
{
    // caixa de abertura de arquivo
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Abrir Imagem"));

    if (!fileName.isEmpty()) {
        img = new QImage(fileName);

        ocr->analisePerfilProjecao(*img);
        chamarExtratores(false);

        ocr->classificarCaracteres();
    }
}

void OCRWindow::chamarExtratores(bool teste)
{
    ocr->quantidadeCaracteres();

    ///////////////////////////////////////////////////////////
    //////////////////// MATRIZ DE COOCORRENCIA
    ///////////////////////////////////////////////////////////
    if (ui->chkbMatrizCoOcorrencia->isChecked()) {

        qDebug() << "Matriz CoOcorrencia: " << ui->cbbMatrizCoOcorrencia->currentText();

        switch (ui->cbbMatrizCoOcorrencia->currentIndex()) {
        // DIREITA
        case 0: {
            ocr->extrairCaracteristicas(teste, OCR::MATRIZ_COOCORRENCIA, OCR::DIREITA);
        }
            break;

        // ABAIXO
        case 1: {
            ocr->extrairCaracteristicas(teste, OCR::MATRIZ_COOCORRENCIA, OCR::ABAIXO);
        }
            break;

        // DIREITA_ABAIXO
        case 2: {
            ocr->extrairCaracteristicas(teste, OCR::MATRIZ_COOCORRENCIA, OCR::DIREITA_ABAIXO);
        }
            break;

        // DIREITA_ACIMA
        case 3: {
            ocr->extrairCaracteristicas(teste, OCR::MATRIZ_COOCORRENCIA, OCR::DIREITA_ACIMA);
        }

        // DIREITA_ABAIXO_DIAGONAL
        case 4: {
            ocr->extrairCaracteristicas(teste, OCR::MATRIZ_COOCORRENCIA, OCR::DIREITA_ABAIXO_DIAGONAL);
        }

        // DIREITA_ACIMA_DIAGONAL
        case 5: {
            ocr->extrairCaracteristicas(teste, OCR::MATRIZ_COOCORRENCIA, OCR::DIREITA_ACIMA_DIAGONAL);
        }
        default:
            break;
        }
    }


    ///////////////////////////////////////////////////////////
    //////////// DISTANCIA BORDA-CARACTERE
    ///////////////////////////////////////////////////////////
    if (ui->chkbDistBordaCaractere->isChecked()) {

        qDebug() << "Distancia Borda-Caractere: " << ui->cbbDistBordaCaractere->currentText();

        switch (ui->cbbDistBordaCaractere->currentIndex()) {
        // Esquerda -> Direita
        case 0: {
            ocr->extrairCaracteristicas(teste, OCR::DIST_BORDA_CARACTERE, OCR::DIREITA, OCR::BC_DIREITA);
        }
            break;

        // Direita -> Esquerda
        case 1: {
            ocr->extrairCaracteristicas(teste, OCR::DIST_BORDA_CARACTERE, OCR::DIREITA, OCR::BC_ESQUERDA);
        }
            break;

        // Esquerda-Direita Subindo
        case 2: {
            ocr->extrairCaracteristicas(teste, OCR::DIST_BORDA_CARACTERE, OCR::DIREITA, OCR::BC_ESQ_DIR_SUB_DIAGONAL);
        }
            break;

        // Esquerda-Direita Desc
        case 3: {
            ocr->extrairCaracteristicas(teste, OCR::DIST_BORDA_CARACTERE, OCR::DIREITA, OCR::BC_ESQ_DIR_DES_DIAGONAL);
        }
            break;

        // Direita-Esquerda Subindo
        case 4: {
            ocr->extrairCaracteristicas(teste, OCR::DIST_BORDA_CARACTERE, OCR::DIREITA, OCR::BC_DIR_ESQ_SUB_DIAGONAL);
        }
            break;

        // Direita-Esquerda Descendo
        case 5: {
            ocr->extrairCaracteristicas(teste, OCR::DIST_BORDA_CARACTERE, OCR::DIREITA, OCR::BC_DIR_ESQ_SUB_DIAGONAL);
        }
            break;

        // Bottom-Up
        case 6: {
            ocr->extrairCaracteristicas(teste, OCR::DIST_BORDA_CARACTERE, OCR::DIREITA, OCR::BC_ACIMA);
        }
            break;

        // Top-Down
        case 7: {
            ocr->extrairCaracteristicas(teste, OCR::DIST_BORDA_CARACTERE, OCR::DIREITA, OCR::BC_ABAIXO);
        }
            break;

        default:
            break;
        }
    }


    ///////////////////////////////////////////////////////////
    //////////// HISTOGRAMAS
    ///////////////////////////////////////////////////////////
    if (ui->chkbHistogramas->isChecked()) {
        ocr->extrairCaracteristicas(teste, OCR::HISTOGRAMAS);
    }
}

void OCRWindow::on_btnOCRAbrirPasta_clicked()
{
    // caixa de abertura de arquivo
    QString folderName = QFileDialog::getExistingDirectory(this,
                                                           tr("Abrir Imagem"),
                                                           "",
                                                           QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!folderName.isEmpty()) {
        qDebug() << folderName;
        QDir dir(folderName);

        dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
        dir.setSorting(QDir::Name);

        QStringList list = dir.entryList();

        // FIXME: Nao esta aplicando os extratores! Ele diz 0 imagens!
        foreach (QString name, list) {
            qDebug() << name;
            img_teste = new QImage(name);
            chamarExtratores(true);
        }
        //imgUpdate();
    }
}
