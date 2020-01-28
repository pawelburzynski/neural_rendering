#include <QApplication>

#include <iostream>
#include <QApplication>
#include <QSurfaceFormat>
#include <QMatrix4x4>
#include <QVector3D>
#include <QCommandLineParser>

#include "LFViewWindow.h"

void printQMat4x4(QMatrix4x4 mat) {
    mat = mat.transposed();
    for(int i=0;i<16;i++) {
        if(i%4==0)
            printf("\n");
        printf("%f ",*(mat.data()+i));
    }
    printf("\n*******************\n");
}

void debugTransform() {
    // intrinsic camera matrix of data camera
    QMatrix4x4 Kci (512.0f,  0.0f,           0.0f,  512.0f,
                    0.0f,           512.0f,    0.0f,  512.0f,
                    0.0f,           0.0f,           1.0f,  0.0f,
                    0.0f,           0.0f,           0.0f,  1.0f);

    // Projection Matrix for data camera
    QMatrix4x4 Pc (1.0f,     0.0f,      0.0f,  0.0f,
                    0.0f,      1.0f,      0.0f,  0.0f,
                    0.0f,      0.0f,      1.0f,  -8,
                    0.0f,      0.0f,      1.0f,  0.0f);

    // Inverted View Transformation matrix for virtual camera
    QMatrix4x4 Vk;
    Vk.lookAt(QVector3D(1.5, 1, -0.1),QVector3D(1.5,1 ,0.9),QVector3D(0,1,0));
    QMatrix4x4 Vk_inv = Vk.inverted();

    // Inverted Projection matrix for virtual camera
    QMatrix4x4 Pc_inv = Pc.inverted();

    // Inverted intrinsic camera matrix for virtual camera
    QMatrix4x4 Kck_inv = Kci.inverted();

    QMatrix4x4 Vc;
    Vc.lookAt(QVector3D(0, 4, 0),QVector3D(0,4,1),QVector3D(0,1,0));
//    Vc = Vc.transposed();

    QVector4D pk(600,600,0,1);

    printf("Kci=\n");
    printQMat4x4(Kci);

    printf("Pc=\n");
    printQMat4x4(Pc);

    printf("Vc=\n");
    printQMat4x4(Vc);

    printf("Vk_inv=\n");
    printQMat4x4(Vk_inv);

    printf("Pc_inv=\n");
    printQMat4x4(Pc_inv);

    printf("Kck_inv=\n");
    printQMat4x4(Kck_inv);

    QVector4D pi = (Kci * Pc * Vc * Vk_inv * Pc_inv * Kck_inv) * pk;
    pi = pi/pi[3];

    printf("pi = %f %f %f %f\n", pi[0], pi[1], pi[2], pi[3]);
}

int main(int argc, char *argv[])
{
//      debugTransform();
    QApplication app(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("LF Viewer");
    parser.addHelpOption();
    //parser.addVersionOption();
    parser.addPositionalArgument("lf_dir", QString("Directory with light field images."));

    parser.process(app);


    QSurfaceFormat fmt;
    fmt.setSamples(4);
    QSurfaceFormat::setDefaultFormat(fmt);

    QString lf_dir = "lf-images";
    const QStringList args = parser.positionalArguments();
    if( args.length()>0 )
        lf_dir=args[0];

    LFViewerWindow window;
    window.show();
    window.loadLFData(lf_dir);
    return app.exec();

}
