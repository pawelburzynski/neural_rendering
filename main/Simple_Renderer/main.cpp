#include <QApplication>

#include <iostream>
#include <QApplication>
#include <QSurfaceFormat>
#include <QMatrix4x4>
#include <QVector3D>
#include <QCommandLineParser>
#include "ViewWindow.h"
#include "LightFieldInterpolation.h"
#include "ViewDependentTextureMapping.h"

int main(int argc, char *argv[])
{
//  debugTransform();
    QApplication app(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("Simple Gausian Interpolation Renderer");
    parser.addHelpOption();
    //parser.addVersionOption();
    parser.addPositionalArgument("training_dir", QString("Directory with training images."));
    parser.addPositionalArgument("output_dir", QString("Directory with output images."));
    parser.process(app);


    QSurfaceFormat fmt;
    fmt.setSamples(4);
    QSurfaceFormat::setDefaultFormat(fmt);

    QString data_dir = "../Data/checkboard_plane";
    QString output_dir = "../Output";
    const QStringList args = parser.positionalArguments();
    if( args.length() > 0 )
        data_dir = args[0];
    if( args.length() > 1 )
        output_dir = args[1];
    //renderer to use
    ViewDependentTextureMapping renderer(data_dir);
    ViewerWindow window(&renderer);
    window.show();
    window.generateEvaluationOutput(data_dir, output_dir);
    return app.exec();
}