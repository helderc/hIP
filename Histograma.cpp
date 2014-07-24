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

#include <QPen>
#include <QVector>
#include <qwt_plot_layout.h>
#include <qwt_legend.h>
//#include <qwt_legend_item.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_histogram.h>
#include <qwt_column_symbol.h>
#include <qwt_series_data.h>
#include "Histograma.h"

class Histogram: public QwtPlotHistogram
{
public:
    Histogram(const QString &, const QColor &);

    void setColor(const QColor &);
    void setValues(uint numValues, QVector<float>*);
};

Histogram::Histogram(const QString &title, const QColor &symbolColor):
    QwtPlotHistogram(title)
{
    setStyle(QwtPlotHistogram::Columns);
    setColor(symbolColor);
}

void Histogram::setColor(const QColor &symbolColor)
{
    QColor color = symbolColor;
    color.setAlpha(180);

    setPen(QPen(Qt::black));
    setBrush(QBrush(color));

    QwtColumnSymbol *symbol = new QwtColumnSymbol(QwtColumnSymbol::NoStyle);
    symbol->setFrameStyle(QwtColumnSymbol::NoFrame);
    symbol->setLineWidth(0);
    symbol->setPalette(QPalette(color));
    setSymbol(symbol);
}

void Histogram::setValues(uint numValues, QVector<float> *valores)
{
    QVector<QwtIntervalSample> samples(numValues);

    for (uint i = 0; i < numValues; i++ ) {
        QwtInterval interval(double(i), i + 1);
        interval.setBorderFlags(QwtInterval::IncludeBorders);
        
        samples[i] = QwtIntervalSample((double)(valores->at(i)), interval);
    }

    setData(new QwtIntervalSeriesData(samples));
}



Histograma::Histograma(QWidget *parent):
    QwtPlot(parent)
{
    // titulo do widget de histograma
    setTitle("");

    setCanvasBackground(QColor(Qt::gray));
    plotLayout()->setAlignCanvasToScales(true);

    setAxisTitle(QwtPlot::yLeft, "");
    setAxisTitle(QwtPlot::xBottom, "Tons de Cinza");

    // Define os valores Max e Min para o eixo X
    setAxisScale(QwtPlot::xBottom, 0, 255);

    setAutoReplot(true);
}

void Histograma::populate(QImage *img)
{
    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->enableX(false);
    grid->enableY(true);
    grid->enableXMin(true);
    grid->enableYMin(true);
    grid->setMajorPen(QPen(Qt::black, 0, Qt::DotLine));
    grid->setMinorPen(QPen(Qt::gray, 0, Qt::DotLine));
    grid->attach(this);

    int cinza, mQtd = 0;
    int x, y;
    // Quantidade de pontos de 0 a 255
    const int pts = 256;

    QVector<float> valores(pts);

    // inicializa os valores com 0
    for (int i = 0; i < pts; i++)
            valores[i] = 0.0;

    for (x = 0; x < img->width(); x++) {
        for (y = 0; y < img->height(); y++) {
            cinza = qGray(img->pixel(QPoint(x,y)));
            valores[cinza]++;

            if (valores[cinza] > mQtd)
                mQtd = valores[cinza];
        }
    }

    Histogram *hist = new Histogram("", Qt::black);
    hist->setValues(pts, &valores);

    hist->attach(this);
    this->replot();
}

void Histograma::showItem(QwtPlotItem *item, bool on)
{
    item->setVisible(on);
}
