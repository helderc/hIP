#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileDialog>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>
#include <QMouseEvent>
#include <QInputDialog>
#include <QDebug>
#include <QStyleFactory>
#include <QImageReader>
#include <QSettings>
#include <QMdiSubWindow>

#include <QtSql>
#include <QFileInfo>

// Biblioteca OpenCV
#include <opencv2/opencv.hpp>

#include "LabelImage.h"
#include "OpCommons.h"
#include "imgwindow.h"
#include "Funcoes.h"
#include "filtros.h"
#include "EdgeDetectMarrHill.h"
#include "EdgeDetectCanny.h"
#include "EdgeDetectShenCastan.h"
#include "ocr.h"
#include "aba.h"
#include "thinning.h"
#include "ocrwindow.h"

#include "ui_mainwindow.h"

using namespace cv;

class QAction;
class QActionGroup;
class QMenu;
class QGraphicsScene;
class QGraphicsView;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    void msgStatus(QString msg);
    QPointF posMouse; /**< TODO */

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow ui; /**< TODO */
    QString fileName; /**< Arquivo da imagem no disco */
    LabelImage *imgLbl1; /**< E' o LabelImage usado para mostrar a imagem
                         original. */

    CvCapture *camera; /**< Ponteiro para a camera de captura utilizada pelo
                        OpenCv. */
    int intTimer; /**< Contador do timer para tirar a foto. */
    int idTimer; /**< ID do Timer. E' necessario para quando for para-lo. */
    QString ultimoDir; /**< TODO: Guarda o local do ultimo arquivo aberto. */
    QActionGroup *windowActionGroup;
    int qtdFoto;  /**< Quantidade de fotos tiradas com a webcam. */
    QVector<double> *histX;
    OCR *ocr;
    QSqlDatabase mydb;

    void criaHist(); /**< Funcoes para o Histograma */
    void salvaConfigs();
    void leConfigs();
    void imgUpdate();
    void edgeDetectShenCastan (float ratioThresh,
                               int sizeAdapGrad, float smoothFactor,
                               int thinFactor, bool doHysteresis);
    void edgeDetectCanny(int desvPadrao, int thresLow,
                         int thresHigh, bool usarOpenCv);
    void edgeDetectMarrHildreth (float desvioPadrao);

    void timerEvent(QTimerEvent *);
    // gerenciamento das subjanelas
    void addImgWindow(QWidget *win);
    void abrirImg(QString arq);
    void upHistograma();
    ImgWindow *janelaAtiva();
    QImage * imagemAtiva();
    void setMiniatura();



private slots:
    void on_actionSobre_o_Qt_triggered();
    void on_actionShen_Castan_triggered();
    void on_actionSalvar_Imagem_2_triggered();
    void on_actionSobelAmbos_triggered();
    void on_actionSobelY_triggered();
    void on_actionSobelX_triggered();
    void on_btnCapCam_clicked();
    void on_actionDesfazer_triggered();
    void on_actionMarr_Hildreth_triggered();
    void on_actionTons_de_Cinza_triggered();
    void on_actionCanny_triggered();
    void on_actionMedirRuidos_triggered();
    void on_btnSobre_triggered();
    void on_btnAbrirImg_triggered();
    void on_btnSair_triggered();
    void on_actionNegativo_triggered();
    void on_actionCanny_OpenCV_triggered();
    void on_actionEqualizar_Histograma_triggered();
    void on_btnSalvarHist_clicked();
    void on_actionS_pia_triggered();
    void on_actionGaussiana_triggered();
    void subJanelaAtivada();
    void on_actionCarton_triggered();
    void on_actionAnalise_de_Projecao_triggered();
    void on_actionMatriz_de_Coocorrencia_triggered();
    void on_actionChain_Code_de_Freeman_triggered();
    void on_actionHistogramas_triggered();
    void on_actionDistancia_Borda_caractere_triggered();

    void on_actionEntropia_triggered();

    void on_actionUniformidade_triggered();

    void on_actionHomogeniedade_triggered();

    void on_actionContraste_triggered();

    void on_actionCorrela_ao_triggered();

    void on_actionZhang_Suen_triggered();

    void on_actionTeste_2_triggered();

    void on_actionJanela_OCR_triggered();

    void on_actionDistancia_Manhattan_triggered();

public slots:
    void mouseImageMove(int x, int y);
};

#endif // MAINWINDOW_H

