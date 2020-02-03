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
    camPosArr.resize(3*(dataPoints+1));
    // View Transformation matrices for cameras
    for (int i = 0; i < dataPoints; i++) {
        QVector4D &w_cam_i = w_cam[i];
        camPosArr[i*3+0] = (float)w_cam_i.x();
        camPosArr[i*3+1] = (float)w_cam_i.y();
        camPosArr[i*3+2] = 0.0f;
    }
        camPosArr[dataPoints*3+0] = (float)w_cam[0].x();
        camPosArr[dataPoints*3+1] = (float)w_cam[0].y();
        camPosArr[dataPoints*3+2] = 0.0f;
    queue.enqueueWriteBuffer(camPos,CL_TRUE,0,sizeof(float)*3*(dataPoints+1),camPosArr.data());
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

    //printf("%s\n", dataFile.toStdString().c_str());
    QFile data(dataFile);
    if (!data.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical("ERROR: Could not open data file!");
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
    // Now  get the training images
    for (int i = 0; i < files.size(); i++) {
        QFileInfo f(files[i]);
        if (f.suffix() == "png") {
            bool flag;
            int camera_index = f.baseName().toInt(&flag)-1;
            if(!flag) {
                qCritical("ERROR: Incorrect image name!");
                std::abort(); 
            }
            QImage img(QString(training_dir) + "/" + files[i]);
            if( imgWidth == -1 ) { // First image loaded
                imgWidth = img.width();
                imgHeight = img.height();
                imageData.resize((dataPoints+1) * imgHeight * imgWidth * 4);
            }
            assert( imgWidth == img.width() && imgHeight == img.height() ); // All images must be the same
            for (int y = 0; y < imgHeight; y++) {
                for (int x = 0; x < imgWidth; x++) {                
                    const QRgb color = img.pixel(x, y); // For compatibility with older Qt, pixel() instead of pixelColor()
                    imageData[((camera_index * imgHeight + y) * imgWidth + x) * 4 + 0] = qRed(color);
                    imageData[((camera_index * imgHeight + y) * imgWidth + x) * 4 + 1] = qGreen(color);
                    imageData[((camera_index * imgHeight + y) * imgWidth + x) * 4 + 2] = qBlue(color);
                    imageData[((camera_index * imgHeight + y) * imgWidth + x) * 4 + 3] = 255;
                    if (camera_index == 0){
                        imageData[((dataPoints * imgHeight + y) * imgWidth + x) * 4 + 0] = qRed(color);
                        imageData[((dataPoints * imgHeight + y) * imgWidth + x) * 4 + 1] = qGreen(color);
                        imageData[((dataPoints * imgHeight + y) * imgWidth + x) * 4 + 2] = qBlue(color);
                        imageData[((dataPoints * imgHeight + y) * imgWidth + x) * 4 + 3] = 255;
                    }
                }
            }
            qDebug() << "loaded: " << i;
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
                      dataPoints+1,
                      0,
                      0,
                      imageData.data());

        camPos = cl::Buffer(context,CL_MEM_READ_WRITE,sizeof(float)*3*(dataPoints+1));
		prepareCamPosArr();

        kernel.setArg(0,renderData);
        kernel.setArg(2,camPos);
    }
    catch(cl::Error err) {
        std::cerr << "ERROR: " << err.what() << "(" << getOCLErrorString(err.err()) << ")" << std::endl;
        exit(1);
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

