#ifndef OCRWINDOW_H
#define OCRWINDOW_H

#include <QWidget>
#include <QFileDialog>

#include "ocr.h"
#include "ocrcaractere.h"

namespace Ui {
class OCRWindow;
}

class OCRWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit OCRWindow(QWidget *parent = 0);
    ~OCRWindow();

    QAction *windowMenuAction() const { return action; }
    void setActionText(QString str);

    
private slots:
    void on_btnOCRAbrirImg_clicked();
    void texto(QString txt);
    void imgUpdate();

    void on_btnTreinar_clicked();

    void on_btnAnaliseProjecao_clicked();

    void on_btnClassificar_clicked();

private:
    Ui::OCRWindow *ui;
    OCR *ocr;
    QAction *action;
    QImage *img;
    QImage *img_teste;

    void chamarExtratores(bool teste);
};

#endif // OCRWINDOW_H
