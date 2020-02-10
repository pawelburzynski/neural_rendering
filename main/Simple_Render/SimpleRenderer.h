#ifndef LightFieldRenderer_H
#define LightFieldRenderer_H

#include <QBrush>
#include <QFont>
#include <QPen>
#include <QWidget>
#include <QVector3D>
#include <QMatrix4x4>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

class SimpleRenderer
{
public:
    SimpleRenderer();

public:
    void paint(QPainter *painter, QPaintEvent *event, int elapsed, const QSize &destSize);
    void readTrainingData(const char *training_dir);
    void generateEvaluationOutput(const char *training_dir, const char *output_dir);

    float focus = 500.0f;
    float apertureSize = 8.0f;
    QVector3D K_pos; // Camera position
    QVector3D K_pos_0; // Initial cmera position
	int viewWidth, viewHeight; // Resolution of the rendered image in pixels

	float camera_fov = 90.f;

private:
    void updateViewSize(int newWidth, int newHeight);

    // OpenCL stuff 
    void initOpenCL();
    void prepareCamPosArr();
    void calculateAllTransforms();
    cl::Platform platform;
    cl::Device device;
    cl::Context context;
    cl::Program program;
    cl::CommandQueue queue;
    cl::Kernel kernel;
    cl::Image2D vcamImage;

    //  Renderer Data
    cl::Image3D renderData;
    int imgWidth;
    int imgHeight;
    int dataPoints = 0;
    int training_dataPoints = 0;
    int eval_dataPoints = 0;
    int debug = 0;
    const int number_closest_points = 25;

    // Input data
    QStringList training_data;
    QStringList evaluation_data;
    
    // Tranformation Matrices
    std::vector<QMatrix4x4> Vi;
    std::vector<QVector4D> w_cam; // World coordindates of each camera
	std::vector<float> camPosArr;
    std::vector<int> closestCamArr;
    cl::Buffer curPos;
    cl::Buffer camPos;      // World coordindates of each camera
    cl::Buffer closestCam; 
};


#endif // LightFieldRenderer_H
