#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QKeyEvent>
#include "Renderer.h"
#include <QLabel>

class ViewWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    ViewWidget(Renderer *renderer, QWidget *parent);
	QVector3D getKpos();
	float getFocal();
	float getAperture();
	float getFov();

public slots:
    void animate();
	
	void setKpos(QVector3D newK_pos);  // Set a new position of the virtual camera
	void setFocal(double newFocus);  // Set a new focus distance for the virtual camera
	void setAperture(double newAperture);  // Set a new aperture for cameras
	void setFov(double newFov);  // Set a new aperture for cameras

signals:
	void KposChanged(QVector3D newK_pos);
signals:
	void focalChanged(double focus);
signals:
	void apertureChanged(double aperture);
signals:
	void fovChanged(double fov);
signals:
	void renderTimeUpdated(int renderTime);

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event);

	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;
	QSize sizeHint() const override;

private:
    Renderer *renderer;
    int elapsed;
	QPoint mouseDragStart;
	QVector3D mouseDragStartK_pos;
	Qt::MouseButton mouseDragButton;
	double mouseDragFocal;
};

#endif // GLWIDGET_H
