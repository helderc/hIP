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
