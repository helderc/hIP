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
