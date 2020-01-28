#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>

#include "LightFieldRenderer.h"

#include <QWidget>
#include <QLabel>
#include <QDoubleSpinBox>
#include "LFViewWidget.h"

class LFViewerWindow : public QMainWindow
{
    Q_OBJECT

public:
    LFViewerWindow();

	void loadLFData(QString lf_dir);

public slots:
	void KposChanged(QVector3D newKPos);
	void renderTimeUpdated(int renderTime);
	void fovUpdated(double fov);

private:
    LightFieldRenderer lfRenderer;
	QLabel *cameraPosLabel;
	QDoubleSpinBox *apertureSB, *focalSB, *fovSB;
    QLabel *renderTimeLabel;
	QLabel *focalLengthLabel;
};


#endif // WINDOW_H
