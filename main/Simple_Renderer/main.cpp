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
    QApplication app(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("Image-Based Renderer");
    parser.addHelpOption();
    //parser.addVersionOption();
    parser.addPositionalArgument("training_dir", QString("Directory with training images."));
    parser.addPositionalArgument("output_dir", QString("Directory with output images."));
    parser.addPositionalArgument("rendering_alg", QString("Rendering algorithm to be chosen from: LightFieldInterpolation,ViewDependentTextureMapping,DeferredNeuralRenderer"));
    parser.process(app);

    QSurfaceFormat fmt;
    fmt.setSamples(4);
    QSurfaceFormat::setDefaultFormat(fmt);

    QString data_dir = "";
    QString output_dir = "";
    Renderer *renderer;
    const QStringList args = parser.positionalArguments();
    if( args.length() > 0 )
        data_dir = args[0];
    if( args.length() > 1 )
        output_dir = args[1];
    if( args.length() > 2 ) {
        if(args[2] == "LightFieldInterpolation" ) {
            renderer  = new LightFieldInterpolation(data_dir);
        }
        if(args[2] == "ViewDependentTextureMapping" ) {
            renderer  = new ViewDependentTextureMapping(data_dir);
        }
    } else {
        renderer = new ViewDependentTextureMapping(data_dir);
    }

    //renderer to use
    ViewerWindow window(renderer);
    window.show();
    window.generateEvaluationOutput(data_dir, output_dir);
    return app.exec();
}