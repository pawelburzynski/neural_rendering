#ifndef Renderer_H
#define Renderer_H

#include <QBrush>
#include <QFont>
#include <QPen>
#include <QWidget>
#include <QVector3D>
#include <QMatrix4x4>

#include "RendererUtils.h"

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>


class Renderer
{
public:
    Renderer();

public:
    virtual void paint(QPainter *painter, QPaintEvent *event, int elapsed, const QSize &destSize) = 0;
    virtual void generateEvaluationOutput(const char *data_dir, const char* output_dir) = 0;
    void readData(const char *data_dir);

    QVector3D K_pos; // Camera position
    QVector3D K_pos_0; // Initial cmera position
	int viewWidth, viewHeight; // Resolution of the rendered image in pixels

	float camera_fov = 39.6f;
    QString renderer_name;

protected:

    void updateViewSize(int newWidth, int newHeight);

    // OpenCL stuff 
    void initOpenCL();
    void prepareCamPosArr();
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

    // Input data
    void readMetaData(QString dir,  std::vector<QVector4D>* w_cam, std::vector<QString>* data_files, std::vector<QMatrix4x4>* pro_mat);
    std::vector<QString> training_data;
    std::vector<QString> eval_data;
    
    // Training data
    std::vector<QVector4D> w_cam_training; // World coordindates of each camera
	std::vector<QVector4D> w_cam_eval;
    std::vector<float> camPosArr;
    cl::Buffer curPos;
    cl::Buffer camPos;      // World coordindates of each camera

    //Matrix data
    std::vector<QMatrix4x4> pro_Mat_Train; // 4x4 projection matrix for Training Data
    std::vector<QMatrix4x4> pro_Mat_Eval;  // 4x4 projection matrix for Evaluation Data
    std::vector<float> pro_Mat_TrainVec; // Projection Matrices for all training data
    cl::Buffer projectionMats;   //  Buffer for projection Matrices
    std::vector<float> inv_Pro_Mat_Cam_Vec; // Inverted Camera Projection matrix
    cl::Buffer invProMatCam;   //  Buffer for Inverted Camera Projection matrix
};

#endif // Renderer_H