#include <QPainter>
#include <QPaintEvent>
#include <QWidget>
#include <QImage>
#include <iostream>
#include <fstream>
#include <QtWidgets>
#include <string>
#include <assert.h>

#include "Renderer.h"
#include "ocl_utils.hpp"

QMatrix4x4 Renderer::lookAtRH(QVector3D eye, QVector3D center, QVector3D up)
{
	QMatrix4x4 la;
	la.lookAt(eye, center, up);
	QMatrix4x4 inv;
	inv(0, 0) = -1;
	inv(2, 2) = -1;
	return inv*la;
}

QMatrix4x4 Renderer::getCurrInvTransMat() {
     // Intrinsic camera matrix of data camera
	float proj_d = tan(camera_fov / 2.f * M_PI / 180.f);
	QMatrix4x4 K(viewWidth/proj_d, 0.0f, viewWidth / 2.0f, 0.0f,
		0.0f, -viewWidth/proj_d, viewHeight / 2.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f);

    // Projection matrix for cameras in the array
	QMatrix4x4 P(1.0f,     0.0f,      0.0f,  0.0f,
                    0.0f, 1.0f,      0.0f,  0.0f,
                    0.0f,     0.0f,  1.0f,  0.0f,
                    0.0f,      0.0f,      0.0f,  0.0f);
    QVector3D up = QVector3D(0.0f, 0.0f, 1.0f); 
    QVector3D cameraRight = QVector3D::crossProduct(up,K_pos);
    QVector3D cameraUp = QVector3D::crossProduct(K_pos, cameraRight);
    cameraUp.normalize();
    QMatrix4x4 V;  // View matrix for the virtual camera K
	V.lookAt(K_pos, K_pos - K_pos*0.1, cameraUp);
    qDebug() << K;
    qDebug() << P;
    qDebug() << V;
	QMatrix4x4 proMatrix = K * P * V;  // From World to Virtual camera K coordinates
    QVector4D row(K_pos.x(),K_pos.y(),K_pos.z(),-(K_pos.x()+K_pos.y()+K_pos.z()));
    proMatrix.setRow(3,row);
	QMatrix4x4 invProMatrix = proMatrix.inverted();
    qDebug() << proMatrix;
    return invProMatrix;
}

double Renderer::angle(QVector4D pos) {
    double s = asin(pos.y() / sqrt(pos.x()*pos.x()+pos.y()*pos.y()));
    if (pos.x() > - 0.0001) {
        return s + M_PI/2.0;
    } else {
        return 3*M_PI/2.0-s;
    }
}

double Renderer::angle(QVector3D pos) {
    double s = asin(pos.y() / sqrt(pos.x()*pos.x()+pos.y()*pos.y()));
    if (pos.x() > - 0.0001) {
        return s + M_PI/2.0;
    } else {
        return 3*M_PI/2.0-s;
    }
}

double Renderer::dist(QVector4D p1, QVector4D p2) {
   return sqrt((p1.x()-p2.x())*(p1.x()-p2.x())+(p1.y()-p2.y())*(p1.y()-p2.y())+(p1.z()-p2.z())*(p1.z()-p2.z()));
}


Renderer::Renderer(const char* kernel_file_name, const char* kernel_program) :
	viewWidth(0), viewHeight(0), training_dataPoints(0), 
    kernelFileName(kernel_file_name), kernelProgram(kernel_program)
{
    initOpenCL();
}

bool cmp(QString x, QString y) {
    QFileInfo f1(x);
    QFileInfo f2(y);
    int x_v = f1.baseName().toInt();
    int y_v = f2.baseName().toInt();
    return x_v <= y_v;
}

void Renderer::prepareCamPosArr()
{
    camPosArr.resize(3*(training_dataPoints+1));
    // View Transformation matrices for cameras
    for (int i = 0; i < training_dataPoints; i++) {
        QFileInfo f(training_data[i]);
        bool flag;
        int camera_index = f.baseName().toInt(&flag)-1;
        if(!flag) {
            qCritical("ERROR: Incorrect image name!");
            std::abort(); 
        }
        QVector4D &w_cam_i = w_cam_training[camera_index];
        camPosArr[i*3+0] = (float)w_cam_i.x();
        camPosArr[i*3+1] = (float)w_cam_i.y();
        camPosArr[i*3+2] = (float)w_cam_i.z();
    }

    camPosArr[training_dataPoints*3+0] = (float)camPosArr[0];
    camPosArr[training_dataPoints*3+1] = (float)camPosArr[1];
    camPosArr[training_dataPoints*3+2] = (float)camPosArr[2];

    queue.enqueueWriteBuffer(camPos,CL_TRUE,0,sizeof(float)*3*(training_dataPoints+1),camPosArr.data());
}

// parse metadata file
void Renderer::readMetaData(QString dir,  std::vector<QVector4D>* w_cam, std::vector<QString>* data_files, std::vector<QMatrix4x4>* pro_mat) {
    
    QStringList files = QDir(dir).entryList();
    QString dataFile;
    int dataPoints = 0;
    bool foundData = false;

    // Find number of images and metadata
    for (int i = 0; i < files.size(); i++) {
        QFileInfo f(files[i]);
        if (f.suffix() == "png") {
            dataPoints ++;
        }
        if (f.suffix() == "txt") {
            dataFile =  dir + "/" + files[i];
            foundData = true;
        }
    }

    qDebug() << dir << endl;
    QFile data(dataFile);
    if (!data.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical("ERROR: Could not open meta_data file!");
        std::abort();
    }

    w_cam->resize(dataPoints);
    pro_mat->resize(dataPoints);
    while (!data.atEnd()) {
        // read the data file
        QString line = data.readLine();
        int index;
        // read 3D coordinates of camera
        double x,y,z;
        QStringList args = line.split(',');
        if (args.size() == 4 && args[0].toInt() <= dataPoints) {
            index = args[0].toInt();
            x = args[1].toDouble();
            y = args[2].toDouble();
            z = args[3].toDouble();
            K_pos = QVector3D(x,y,z);
            (*w_cam)[index-1] = QVector4D(x,y,z,1);
        } else {
            qDebug() << args.size();
            qDebug() << line;
            qCritical("ERROR: Invalid data line!");
            std::abort();
        }
         // read 3x4 projection matrix for the camera
        QMatrix4x4 proMatrix;
        for(int i=0; i<3; i++) {
            line = data.readLine();
            args = line.split(',');
            if (args.size() == 4) {
                QVector4D row(args[0].toDouble(),args[1].toDouble(),args[2].toDouble(),args[3].toDouble());
                proMatrix.setRow(i,row);
            } else {
                qDebug() << line;
                qCritical("ERROR: Invalid data line!");
                std::abort();
            }
        }
        QVector4D row(x,y,z,-(x+y+z));
        proMatrix.setRow(3,row);
        (*pro_mat)[index-1] = proMatrix;
    }

    if (dataPoints < 1) {
        qCritical("ERROR: No images!");
        std::abort();
    }

    if (!foundData) {
        qCritical("ERROR: No metadata!");
        std::abort();
    }

    qDebug() << "Founded " << dataPoints << "images!";

    for(int i = 0; i < files.size(); i++) {
        QFileInfo f(files[i]);
        if (f.suffix() == "png") {
            data_files->push_back(files[i]);
        }
    }
}

// Read data
void Renderer::readData(const char *data_dir)
{
    K_pos = QVector3D(0.f,0.f,0.f);
    readMetaData(QString(data_dir) + "/training",  &w_cam_training, &training_data, &pro_Mat_Train);
    readMetaData(QString(data_dir) + "/eval",  &w_cam_eval, &eval_data, &pro_Mat_Eval);

    training_dataPoints = training_data.size();
    eval_dataPoints = eval_data.size();
    std::sort(training_data.begin(), training_data.end(),cmp);
    std::sort(eval_data.begin(), eval_data.end(),cmp);
    for(int i=0; i<eval_data.size(); i++) {
        qDebug() << eval_data[i];
    }

    pro_Mat_TrainVec.resize(16*(training_dataPoints+1));
    for(int index = 0; index < training_dataPoints; index++) {
        QMatrix4x4 pro_Mat_Train_trans = pro_Mat_Train[index].transposed();
        for(int k = 0; k < 16; k++){
            pro_Mat_TrainVec[16*index+k] = (float)*(pro_Mat_Train_trans.data()+k);
        }
    }

    std::vector<uint8_t> imageData; 
    qDebug() << training_data.size() << endl;
    // Now  get the training images
    for (int i = 0; i < training_data.size(); i++) {
        QFileInfo f(training_data[i]);
        if (f.suffix() == "png") {
            bool flag;
            int camera_index = f.baseName().toInt(&flag)-1;
            if(!flag) {
                qCritical("ERROR: Incorrect image name!");
                std::abort(); 
            }
            QImage img(QString(data_dir) + "/training/" + training_data[i]);
            if( imgWidth == -1 ) { // First image loaded
                imgWidth = img.width();
                imgHeight = img.height();
                imageData.resize((training_dataPoints+1) * imgHeight * imgWidth * 4);
            }
            assert( imgWidth == img.width() && imgHeight == img.height() ); // All images must be the same
            for (int y = 0; y < imgHeight; y++) {
                for (int x = 0; x < imgWidth; x++) {                
                    const QRgb color = img.pixel(x, y); // For compatibility with older Qt, pixel() instead of pixelColor()
                    imageData[((i * imgHeight + y) * imgWidth + x) * 4 + 0] = qRed(color);
                    imageData[((i * imgHeight + y) * imgWidth + x) * 4 + 1] = qGreen(color);
                    imageData[((i * imgHeight + y) * imgWidth + x) * 4 + 2] = qBlue(color);
                    imageData[((i * imgHeight + y) * imgWidth + x) * 4 + 3] = 255;
                    if (i == 0){
                        imageData[((i * imgHeight + y) * imgWidth + x) * 4 + 0] = qRed(color);
                        imageData[((i * imgHeight + y) * imgWidth + x) * 4 + 1] = qGreen(color);
                        imageData[((i * imgHeight + y) * imgWidth + x) * 4 + 2] = qBlue(color);
                        imageData[((i * imgHeight + y) * imgWidth + x) * 4 + 3] = 255;
                    }
                }
            } 
            qDebug() << "loaded: " << camera_index+1 << ", position: " 
            << w_cam_training[camera_index].x() << " " 
            << w_cam_training[camera_index].y() << " " 
            << w_cam_training[camera_index].z();
        }
    }
    qDebug() << "VCamera" << "at (" << K_pos.x() << ", " << K_pos.y() <<")";
    
    try {
        renderData = cl::Image3D(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                      cl::ImageFormat(CL_RGBA, CL_UNORM_INT8), 
                      imgWidth,
                      imgHeight,
                      training_dataPoints+1,
                      0,
                      0,
                      imageData.data());
        camPos = cl::Buffer(context,CL_MEM_READ_WRITE,sizeof(float)*3*(training_dataPoints+1));
        curPos = cl::Buffer(context,CL_MEM_READ_WRITE,sizeof(float)*4);
        projectionMats = cl::Buffer(context,CL_MEM_READ_ONLY,sizeof(float)*16*(training_dataPoints+1));
        queue.enqueueWriteBuffer(projectionMats,CL_TRUE,0,sizeof(float)*16*(training_dataPoints+1),pro_Mat_TrainVec.data());
        prepareCamPosArr();
        kernel.setArg(0,renderData);
        kernel.setArg(2,camPos);
        kernel.setArg(3,curPos);
    }
    catch(cl::Error err) {
        std::cerr << "ERROR: " << err.what() << "(" << getOCLErrorString(err.err()) << ")" << std::endl;
        exit(1);
    }

}

void Renderer::updateViewSize(int newWidth, int newHeight)
{
    if( newWidth == viewWidth && newHeight == viewHeight )
        return;
    
    viewWidth = newWidth;
    viewHeight = newHeight;

    // Output image
    vcamImage = cl::Image2D(context,
                CL_MEM_WRITE_ONLY,
                cl::ImageFormat(CL_RGBA, CL_UNORM_INT8),
                viewWidth,
                viewHeight,
                0,
                NULL);
                
    kernel.setArg(1,vcamImage);
}

// Initialize OpenCL resources
void Renderer::initOpenCL()
{
	try {
		//get all platforms (drivers)
		std::vector<cl::Platform> all_platforms;
		cl::Platform::get(&all_platforms);
		if (all_platforms.size() == 0) {
			std::cout << " No platforms found. Check OpenCL installation!\n";
			exit(1);
		}
		platform = all_platforms[0];
		std::cout << "Using platform: " << platform.getInfo<CL_PLATFORM_NAME>() << "\n";

		//get default device of the default platform
		std::vector<cl::Device> all_devices;
		platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
		if (all_devices.size() == 0) {
			std::cout << " No devices found. Check OpenCL installation!\n";
			exit(1);
		}
		device = all_devices[0];
		std::cout << "Using device: " << device.getInfo<CL_DEVICE_NAME>() << "\n";

		context = cl::Context({ device });

		cl::Program::Sources sources;

		std::ifstream kernelFile(kernelFileName);
		if (kernelFile.fail()) {
			std::cout << "ERROR: can't read the kernel file\n";
			exit(1);
		}
		std::string src(std::istreambuf_iterator<char>(kernelFile), (std::istreambuf_iterator<char>()));
		sources.push_back(std::make_pair(src.c_str(), src.length()));

        program = cl::Program(context, sources);
	    try {
            program.build({ device });
        } catch(cl::Error err) {
			std::cout << " Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << "\n";
			exit(1);
		}
		queue = cl::CommandQueue(context, device);
		kernel = cl::Kernel(program, kernelProgram);
	}
	catch(cl::Error err) {
		std::cerr << "ERROR: " << err.what() << "(" << getOCLErrorString(err.err()) << ")" << std::endl;
		exit(1);
	}
}