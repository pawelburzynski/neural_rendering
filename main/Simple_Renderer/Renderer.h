#ifndef Renderer_H
#define Renderer_H

#include <QBrush>
#include <QFont>
#include <QPen>
#include <QWidget>
#include <QVector3D>
#include <QMatrix4x4>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>


class Renderer
{
public:
    Renderer(const char* kernel_file_name, const char* kernel_program);

public:
    virtual void paint(QPainter *painter, QPaintEvent *event, int elapsed, const QSize &destSize) = 0;
    virtual void generateEvaluationOutput(const char *data_dir, const char* output_dir) = 0;
    void readData(const char *data_dir);

    float focus = 500.0f;
    float apertureSize = 8.0f;
    QVector3D K_pos; // Camera position
    QVector3D K_pos_0; // Initial cmera position
	int viewWidth, viewHeight; // Resolution of the rendered image in pixels

	float camera_fov = 90.f;

protected:
    // utils
    double angle(QVector4D pos);
    double angle(QVector3D pos);
    double dist(QVector4D p1, QVector4D p2);

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
    const char *kernelFileName;
    const char *kernelProgram;

    //  Renderer Data
    cl::Image3D renderData;
    int imgWidth = -1;
    int imgHeight = -1;
    int training_dataPoints = 0;
    int eval_dataPoints = 0;
    int debug = 0;
 
    // Input data
    void readImages(QString dir,  std::vector<QVector4D>* w_cam, QStringList* data_files);
    QStringList training_data;
    QStringList eval_data;
    
    // Training data
    std::vector<QVector4D> w_cam_training; // World coordindates of each camera
	std::vector<QVector4D> w_cam_eval;
    std::vector<float> camPosArr;
    cl::Buffer curPos;
    cl::Buffer camPos;      // World coordindates of each camera
   
};


#endif // Renderer_H
