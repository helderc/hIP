#ifndef HISTOGRAMA_H

#include <QImage>
#include <QVector>
#include <QFileDialog>
#include "qwt_series_data.h"
#include "qwt_plot.h"

class Histograma: public QwtPlot
{
    Q_OBJECT

public:
    Histograma(QWidget * = NULL);
    void populate(QImage *img);


private Q_SLOTS:
    void showItem(QwtPlotItem *, bool on);
};

#endif
