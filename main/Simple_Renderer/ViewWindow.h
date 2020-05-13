#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include "Renderer.h"

#include <QWidget>
#include <QLabel>
#include <QDoubleSpinBox>
#include "ViewWidget.h"


class ViewerWindow : public QMainWindow
{
    Q_OBJECT

public:
    ViewerWindow(Renderer* r);

public slots:
	void KposChanged(QVector3D newKPos);
	void renderTimeUpdated(int renderTime);
	void fovUpdated(double fov);

private:
    Renderer* renderer;
	QLabel *cameraPosLabel;
	QDoubleSpinBox *fovSB;
    QLabel *renderTimeLabel;
	QLabel *focalLengthLabel;
};


#endif // WINDOW_H
