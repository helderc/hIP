#include "LabelImage.h"
#include "mainwindow.h"

LabelImage::LabelImage(QWidget *parent) : QLabel(parent)
{
}


LabelImage::~LabelImage()
{
}

void LabelImage::mouseMoveEvent(QMouseEvent * event)
{
    emit mouseMoving(event->x(), event->y());
}

void LabelImage::mousePressEvent(QMouseEvent *event)
{
    emit mouseMoving(event->x(), event->y());
}

