#ifndef SimpleRenderer_H
#define SimpleRenderer_H

#include <QBrush>
#include <QFont>
#include <QPen>
#include <QWidget>
#include <QVector3D>
#include <QMatrix4x4>
#include "Renderer.h"

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>


class SimpleRenderer : public Renderer
{
public:
    SimpleRenderer();

public:
    void paint(QPainter *painter, QPaintEvent *event, int elapsed, const QSize &destSize);
    void generateEvaluationOutput(const char *data_dir, const char* output_dir);

private:
   
};


#endif // SimpleRenderer_H
