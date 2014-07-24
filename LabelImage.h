#ifndef LABELIMAGE_H
#define LABELIMAGE_H

#include <QLabel>

// Classe que herda o componente QLabel
// para acessar as coordenadas de movimento do mouse dentro do
// objeto Janela Principal
class LabelImage : public QLabel
{
    Q_OBJECT

public:
    LabelImage(QWidget *parent);
    ~LabelImage();

signals:
    // emite esse sinal toda vez que for gerado o evento mouseMoveEvent
    void mouseMoving(int x, int y);

protected:
    //evento gerado toda vez que o mouse for pressionado e movimento dentro desse componente
    virtual void mouseMoveEvent (QMouseEvent *event);
    // evento gerado quando o mouse for Pressionado
    virtual void mousePressEvent(QMouseEvent *event);

private:

};

#endif // LABELIMAGE_H
