#include <QPainter>
#include <QPaintEvent>
#include <QWidget>
#include <QImage>
#include <iostream>
#include <fstream>
#include <QtWidgets>
#include <string>
#include <assert.h>

#include "LightFieldInterpolation.h"

extern const char *getOCLErrorString(cl_int error);

using namespace renderer;

LightFieldInterpolation::LightFieldInterpolation(QString data_dir):Renderer()
{
    kernelFileName = "light_field_interpolation.cl";
    kernelProgram = "light_field_interpolation";
    rendererName = "light field interpolation";
    initOpenCL();
    readData(data_dir.toLocal8Bit());
    init();
}

void LightFieldInterpolation::init()
{
    try
    {
        closestCam = cl::Buffer(context,CL_MEM_READ_WRITE,sizeof(int)*number_closest_points);
        kernel.setArg(6,closestCam);
        kernel.setArg(7,number_closest_points);
    }
    catch(cl::Error err) {
        std::cerr << "ERROR: " << err.what() << "(" << getOCLErrorString(err.err()) << ")" << std::endl;
        exit(1);
    }
}

void LightFieldInterpolation::paint(QPainter *painter, QPaintEvent *event, int elapsed, const QSize &destSize)
{
    if( trainingDataPoints == 0 ) // No data available
        return;

    unsigned char* data = NULL;
    try {
        updateViewSize( destSize.width(), destSize.height() );
     
        std::vector<std::pair<double, int>> cams;
        for (int j = 0; j < trainingDataPoints; j++) {
            QFileInfo f2(training_data[j]);
            bool flag2;
            int camera_index2 = f2.baseName().toInt(&flag2)-1;
            if(!flag2) {
                qCritical("ERROR: Incorrect image name!");
                std::abort(); 
            }
            QVector4D &w_cam_j = wCamTraining[camera_index2];
            cams.push_back(std::make_pair(dist(w_cam_j,K_pos.toVector4D()),j));
        }
        sort(cams.begin(), cams.end());
        closestCamArr.resize(number_closest_points);
        for (int j=0; j<number_closest_points; j++) {
            closestCamArr[j] = cams[j].second;
        }
        std::vector<float> curPosArr;
        curPosArr.push_back(K_pos.x());
        curPosArr.push_back(K_pos.y());
        curPosArr.push_back(K_pos.z());
        curPosArr.push_back(1);

        invProMatCamVec.resize(16);
        QMatrix4x4 inv_pro_Mat = getCurrInvTransMat(camera_fov, K_pos, viewWidth, viewHeight).transposed();
        for(int k = 0; k < 16; k++){
            invProMatCamVec[k] = *(inv_pro_Mat.data()+k);
        }

        queue.enqueueWriteBuffer(closestCam,CL_TRUE,0,sizeof(int)*(number_closest_points),closestCamArr.data());
        queue.enqueueWriteBuffer(curPos, CL_TRUE, 0, sizeof(float) * 4, curPosArr.data());
        queue.enqueueWriteBuffer(invProMatCam, CL_TRUE, 0, sizeof(float) * 16, invProMatCamVec.data());
		queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(viewWidth, viewHeight, 1), cl::NullRange);
        // Read Image back and display
        data = new unsigned char[viewWidth*viewHeight*4];
        cl::size_t<3> origin;
        origin[0] = 0; origin[1] = 0, origin[2] = 0;
        cl::size_t<3> region;
        region[0] = viewWidth; region[1] = viewHeight; region[2] = 1;
        queue.enqueueReadImage(vcamImage, CL_TRUE, origin, region, 0, 0 , data,  NULL, NULL);
        queue.finish();

        // TODO, we should write to an OpenGL texture and draw it instead of coping data GPU->CPU
        QImage img(data, viewWidth, viewHeight, QImage::Format_RGBA8888);
        painter->drawImage(0, 0, img);        
    } catch(cl::Error err) {
         std::cerr << "ERROR: " << err.what() << "(" << getOCLErrorString(err.err()) << ")" << std::endl;
    }
    delete[] data;
}

void LightFieldInterpolation::generateEvaluationOutput(const char *data_dir, const char* output_dir) {
     for (int i = 0; i < evalDataPoints; i++) {
        QFileInfo f(eval_data[i]);
        bool flag;
        int camera_index = f.baseName().toInt(&flag)-1;
        if(!flag) {
            qCritical("ERROR: Incorrect image name!");
            std::abort(); 
        }
        QVector4D &w_cam_i = wCamEval[camera_index];
        if( evalDataPoints == 0 ) // No data available
        return;

        unsigned char* data = NULL;
        try {
            QImage img_sample(QString(data_dir) + "/eval/" + f.filePath());
            updateViewSize( img_sample.width(), img_sample.height() );
            qDebug() << "Evaluation output #" << i+1;
            
            std::vector<std::pair<double, int>> cams;
            for (int j = 0; j < trainingDataPoints; j++) {
                QFileInfo f2(training_data[j]);
                bool flag2;
                int camera_index2 = f2.baseName().toInt(&flag2)-1;
                if(!flag2) {
                    qCritical("ERROR: Incorrect image name!");
                    std::abort(); 
                }
                QVector4D &w_cam_j = wCamTraining[camera_index2];
                cams.push_back(std::make_pair(dist(w_cam_j,w_cam_i),j));
            }
            sort(cams.begin(), cams.end());
            closestCamArr.resize(number_closest_points);
            for (int j=0; j<number_closest_points; j++) {
                closestCamArr[j] = cams[j].second;
            }
            std::vector<float> curPosArr;
            curPosArr.resize(sizeof(float)*4);
            curPosArr[0] = (w_cam_i.x());
            curPosArr[1] = (w_cam_i.y());
            curPosArr[2] = (w_cam_i.z());
            curPosArr[3] = (w_cam_i.w());

            invProMatCamVec.resize(16);
            QMatrix4x4 inv_pro_Mat = proMatEval[camera_index].inverted().transposed();
            for(int k = 0; k < 16; k++){
                invProMatCamVec[k] = *(inv_pro_Mat.data()+k);
            }

            queue.enqueueWriteBuffer(closestCam,CL_TRUE,0,sizeof(int)*(number_closest_points),closestCamArr.data());
            queue.enqueueWriteBuffer(curPos, CL_TRUE, 0, sizeof(float) * 4, curPosArr.data());
            queue.enqueueWriteBuffer(invProMatCam, CL_TRUE, 0, sizeof(float) * 16, invProMatCamVec.data());
            queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(viewWidth, viewHeight, 1), cl::NullRange);
            // Read Image back and display
            data = new unsigned char[viewWidth*viewHeight*4];
            cl::size_t<3> origin;
            origin[0] = 0; origin[1] = 0, origin[2] = 0;
            cl::size_t<3> region;
            region[0] = viewWidth; region[1] = viewHeight; region[2] = 1;
            queue.enqueueReadImage(vcamImage, CL_TRUE, origin, region, 0, 0 , data,  NULL, NULL);
            queue.finish();

            // write to a file
            QImage img(data, viewWidth, viewHeight, QImage::Format_RGBA8888);
            const QString &fileName_out = QString(output_dir) + "/LightFieldInterpolation/" + QString::number(i+1)+"out.png";
            const QString &fileName_sample = QString(output_dir) + "/LightFieldInterpolation/" + QString::number(i+1)+"sample.png";
            img.save(fileName_out, "PNG");
            img_sample.save(fileName_sample, "PNG");
        } catch(cl::Error err) {
            std::cerr << "ERROR: " << err.what() << "(" << getOCLErrorString(err.err()) << ")" << std::endl;
        }
        delete[] data;
     }
}