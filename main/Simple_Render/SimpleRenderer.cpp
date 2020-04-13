#include <QPainter>
#include <QPaintEvent>
#include <QWidget>
#include <QImage>
#include <iostream>
#include <fstream>
#include <QtWidgets>
#include <string>
#include <assert.h>

#include "SimpleRenderer.h"
#include "ocl_utils.hpp"

/* 
  A right-handed version of the QMatrix4x4.lookAt method
*/
QMatrix4x4 lookAtRH(QVector3D eye, QVector3D center, QVector3D up)
{
	QMatrix4x4 la;
	la.lookAt(eye, center, up);
	QMatrix4x4 inv;
	inv(0, 0) = -1;
	inv(2, 2) = -1;
	return inv*la;
}

double angle(QVector4D pos) {
    double s = asin(pos.y() / sqrt(pos.x()*pos.x()+pos.y()*pos.y()));
    if (pos.x() > - 0.0001) {
        return s + M_PI/2.0;
    } else {
        return 3*M_PI/2.0-s;
    }
}

double angle(QVector3D pos) {
    double s = asin(pos.y() / sqrt(pos.x()*pos.x()+pos.y()*pos.y()));
    if (pos.x() > - 0.0001) {
        return s + M_PI/2.0;
    } else {
        return 3*M_PI/2.0-s;
    }
}

double dist(QVector4D p1, QVector4D p2) {
   return sqrt((p1.x()-p2.x())*(p1.x()-p2.x())+(p1.y()-p2.y())*(p1.y()-p2.y())+(p1.z()-p2.z())*(p1.z()-p2.z()));
}


SimpleRenderer::SimpleRenderer() :
	viewWidth(0), viewHeight(0), dataPoints(0)
{
    initOpenCL();
}

void SimpleRenderer::paint(QPainter *painter, QPaintEvent *event, int elapsed, const QSize &destSize)
{
    if( dataPoints == 0 ) // No data available
        return;

    unsigned char* data = NULL;
    try {
        updateViewSize( destSize.width(), destSize.height() );
     
        std::vector<std::pair<double, int>> cams;
        for (int j = 0; j < training_dataPoints; j++) {
            QFileInfo f2(evaluation_data[j]);
            bool flag2;
            int camera_index2 = f2.baseName().toInt(&flag2)-1;
            if(!flag2) {
                qCritical("ERROR: Incorrect image name!");
                std::abort(); 
            }
            QVector4D &w_cam_j = w_cam[camera_index2];
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
        queue.enqueueWriteBuffer(closestCam,CL_TRUE,0,sizeof(int)*(number_closest_points),closestCamArr.data());
        queue.enqueueWriteBuffer(curPos, CL_TRUE, 0, sizeof(float) * 4, curPosArr.data());
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

void SimpleRenderer::prepareCamPosArr()
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
        QVector4D &w_cam_i = w_cam[camera_index];
        camPosArr[i*3+0] = (float)w_cam_i.x();
        camPosArr[i*3+1] = (float)w_cam_i.y();
        camPosArr[i*3+2] = (float)w_cam_i.z();
    }

    camPosArr[training_dataPoints*3+0] = (float)camPosArr[0];
    camPosArr[training_dataPoints*3+1] = (float)camPosArr[1];
    camPosArr[training_dataPoints*3+2] = (float)camPosArr[2];

    queue.enqueueWriteBuffer(camPos,CL_TRUE,0,sizeof(float)*3*(training_dataPoints+1),camPosArr.data());
}

// Read training data
void SimpleRenderer::readTrainingData(const char *training_dir)
{

    QStringList files = QDir(training_dir).entryList();
    QString dataFile;
    bool foundData = false;

    // Find number of training images and metadata
    for (int i = 0; i < files.size(); i++) {
        QFileInfo f(files[i]);
        if (f.suffix() == "png") {
            dataPoints ++;
        }
        if (f.suffix() == "txt") {
            dataFile =  QString(training_dir) + "/" + files[i];
            foundData = true;
        }
    }

    qDebug() << training_dir << endl;
    QFile data(dataFile);
    if (!data.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical("ERROR: Could not open meta_data file!");
        std::abort();
    }

    w_cam.resize(dataPoints);
    K_pos = QVector3D(0.f,0.f,0.f);

    while (!data.atEnd()) {
        // read the data file
        QString line = data.readLine();
        int index;
        double x,y,z;
        QStringList args = line.split(',');
        if (args.size() == 4 && index < dataPoints) {
            index = args[0].toInt();
            x = args[1].toDouble();
            y = args[2].toDouble();
            z = args[3].toDouble();
            K_pos = QVector3D(x,y,z);
            w_cam[index-1] = QVector4D(x,y,z,1);
        } else {
            qCritical("ERROR: Invalid data line!");
            std::abort();
        }
    }

    if (dataPoints < 1) {
        qCritical("ERROR: No training images!");
        std::abort();
    }

     if (!foundData) {
        qCritical("ERROR: No training metadata!");
        std::abort();
    }
    
    qDebug() << "Founded " << dataPoints << "training images!";
    std::vector<uint8_t> imageData; 
    imgWidth = -1;
    imgHeight = -1;
    
    //prepare training and evaluation set

    std::random_shuffle(files.begin(), files.end());
    
    for(int i = 0; i < 4*dataPoints/5; i++) {
        QFileInfo f(files[i]);
        if (f.suffix() == "png") {
            training_data.append(files[i]);
        }
    }
    for(int i = 4*dataPoints/5; i < dataPoints; i++ ) {
        QFileInfo f(files[i]);
        if (f.suffix() == "png") {
            evaluation_data.append(files[i]);
        }
    }
    training_dataPoints = training_data.size();
    eval_dataPoints = evaluation_data.size();
    training_data.sort();
    evaluation_data.sort();
    
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
            QImage img(QString(training_dir) + "/" + training_data[i]);
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
            qDebug() << "loaded: " << camera_index << ", angle: " << angle(w_cam[camera_index]);
        }
    }
    qDebug() << "VCamera" << "at (" << K_pos.x() << ", " << K_pos.y() <<")";

    try {
        renderData = cl::Image3D(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
					  // CL_RGBA - because Nvidia does not support RGB, we need to use RGBA
		              // CL_UNORM_INT8 - pixels should be read with read_imagef. The values will be normalized 0-1
                      cl::ImageFormat(CL_RGBA, CL_UNORM_INT8), 
                      imgWidth,
                      imgHeight,
                      training_dataPoints+1,
                      0,
                      0,
                      imageData.data());
        camPos = cl::Buffer(context,CL_MEM_READ_WRITE,sizeof(float)*3*(training_dataPoints+1));
        closestCam = cl::Buffer(context,CL_MEM_READ_WRITE,sizeof(int)*number_closest_points);
        curPos = cl::Buffer(context,CL_MEM_READ_WRITE,sizeof(float)*4);

		prepareCamPosArr();
        kernel.setArg(0,renderData);
        kernel.setArg(2,camPos);
        kernel.setArg(3,closestCam);
        kernel.setArg(4,curPos);
        kernel.setArg(5,number_closest_points);
    }
    catch(cl::Error err) {
        std::cerr << "ERROR: " << err.what() << "(" << getOCLErrorString(err.err()) << ")" << std::endl;
        exit(1);
    }

}

void SimpleRenderer::generateEvaluationOutput(const char *training_dir, const char *output_dir) {
     for (int i = 0; i < eval_dataPoints; i++) {
        QFileInfo f(evaluation_data[i]);
        bool flag;
        int camera_index = f.baseName().toInt(&flag)-1;
        if(!flag) {
            qCritical("ERROR: Incorrect image name!");
            std::abort(); 
        }
        QVector4D &w_cam_i = w_cam[camera_index];
        
        if( dataPoints == 0 ) // No data available
        return;

        unsigned char* data = NULL;
        try {
            QImage img_sample(QString(training_dir) + "/" +f.filePath());
            updateViewSize( img_sample.width(), img_sample.height() );
            qDebug() << "Evaluation output #" << i;
            
            std::vector<std::pair<double, int>> cams;
            for (int j = 0; j < training_dataPoints; j++) {
                QFileInfo f2(training_data[j]);
                bool flag2;
                int camera_index2 = f2.baseName().toInt(&flag2)-1;
                if(!flag2) {
                    qCritical("ERROR: Incorrect image name!");
                    std::abort(); 
                }
                QVector4D &w_cam_j = w_cam[camera_index2];
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
            queue.enqueueWriteBuffer(closestCam,CL_TRUE,0,sizeof(int)*(number_closest_points),closestCamArr.data());
            queue.enqueueWriteBuffer(curPos, CL_TRUE, 0, sizeof(float) * 4, curPosArr.data());
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
            const QString &fileName_out = output_dir + QString::number(i)+"_out.png";
            const QString &fileName_sample = output_dir + QString::number(i)+"sample.png";
            img.save(fileName_out, "PNG");
            img_sample.save(fileName_sample, "PNG");
        } catch(cl::Error err) {
            std::cerr << "ERROR: " << err.what() << "(" << getOCLErrorString(err.err()) << ")" << std::endl;
        }
        delete[] data;
     }
}

void SimpleRenderer::updateViewSize(int newWidth, int newHeight)
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
void SimpleRenderer::initOpenCL()
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

		std::ifstream kernelFile("simple_render.cl");
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
		kernel = cl::Kernel(program, "simple_render");
	}
	catch(cl::Error err) {
		std::cerr << "ERROR: " << err.what() << "(" << getOCLErrorString(err.err()) << ")" << std::endl;
		exit(1);
	}

}

