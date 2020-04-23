#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>

#include "SimpleRenderer.h"

#include <QWidget>
#include <QLabel>
#include <QDoubleSpinBox>
#include "ViewWidget.h"


class ViewerWindow : public QMainWindow
{
    Q_OBJECT

public:
   ViewerWindow(Renderer* r);

	void loadTrainingData(QString training_dir);
	void generateEvaluationOutput(QString training_dir, QString output_dir);

public slots:
	void KposChanged(QVector3D newKPos);
	void renderTimeUpdated(int renderTime);
	void fovUpdated(double fov);

private:
    Renderer* renderer;
	QLabel *cameraPosLabel;
	QDoubleSpinBox *apertureSB, *focalSB, *fovSB;
    QLabel *renderTimeLabel;
	QLabel *focalLengthLabel;
};


#endif // WINDOW_H
