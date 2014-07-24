#ifndef ABA_H
#define ABA_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpacerItem>

class Aba : public QWidget
{
    Q_OBJECT
public:
    explicit Aba(QWidget *parent = 0);
    void inserirTexto(QString texto);

private:
    QPlainTextEdit *textEdit;
    QPushButton *btnSalvar;
    
signals:
    
public slots:
    
};

#endif // ABA_H
