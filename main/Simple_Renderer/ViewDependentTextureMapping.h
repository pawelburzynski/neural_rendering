#ifndef ViewDependentTextureMapping_H
#define ViewDependentTextureMapping_H

#include <QBrush>
#include <QFont>
#include <QPen>
#include <QWidget>
#include <QVector3D>
#include <QMatrix4x4>
#include "Renderer.h"

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>


class ViewDependentTextureMapping : public Renderer
{
public:
    ViewDependentTextureMapping(QString data_dir);

public:
    void paint(QPainter *painter, QPaintEvent *event, int elapsed, const QSize &destSize);
    void generateEvaluationOutput(const char *data_dir, const char* output_dir);

private:
   void init();

    //renderer data
    const int number_closest_points = 10;
    std::vector<int> closestCamArr;
    cl::Buffer closestCam;

};

#endif // view_dependent_texture_mapping_H