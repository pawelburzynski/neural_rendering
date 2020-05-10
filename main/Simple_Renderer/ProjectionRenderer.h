#ifndef ProjectionRenderer_H
#define ProjectionRenderer_H

#include <QBrush>
#include <QFont>
#include <QPen>
#include <QWidget>
#include <QVector3D>
#include <QMatrix4x4>
#include "Renderer.h"

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>


class ProjectionRenderer : public Renderer
{
public:
    ProjectionRenderer(QString data_dir);

public:
    void paint(QPainter *painter, QPaintEvent *event, int elapsed, const QSize &destSize);
    void generateEvaluationOutput(const char *data_dir, const char* output_dir);

private:
   void init();

    //renderer data
    const int number_closest_points = 10;
    std::vector<int> closestCamArr;
    cl::Buffer closestCam;
    std::vector<float> inv_Pro_Mat_Cam_Vec; // Inverted Camera Projection matrix
    cl::Buffer invProMatCam;   //  Buffer for Inverted Camera Projection matrix

};

#endif // SimpleRenderer_H