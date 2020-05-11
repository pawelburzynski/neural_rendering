#include "ViewWidget.h"
#include <algorithm>
#include <cmath>
#include "Renderer.h"
#include <QPainter>
#include <QTimer>
#include <QTime>

#define max_focal 15000.f
#define min_focal 1.f

#define min_fov 2.f
#define max_fov 180.f


ViewWidget::ViewWidget(Renderer *renderer, QWidget *parent)
    : QOpenGLWidget(parent), renderer(renderer)
{
    elapsed = 0;
		setMinimumSize(256, 256);
    setAutoFillBackground(false);
		setFocusPolicy(Qt::StrongFocus);
}

QSize ViewWidget::sizeHint() const
{
		return QSize(1280,720);
}

void ViewWidget::animate()
{
    elapsed = (elapsed + qobject_cast<QTimer*>(sender())->interval()) % 1000;
//    update();
}

void ViewWidget::setKpos(QVector3D newK_pos)
{
	renderer->K_pos = newK_pos;
	update();
	emit KposChanged(newK_pos);
}

void ViewWidget::setFocal(double newFocus)
{
	renderer->focus = newFocus;
	update();
	emit focalChanged(newFocus);
}


void ViewWidget::setAperture(double newAperture)
{
	renderer->apertureSize = newAperture;
	update();
	emit apertureChanged(newAperture);
}

void ViewWidget::setFov(double newFov)
{
	renderer->camera_fov = newFov;
	update();
	emit fovChanged(newFov);
}

void ViewWidget::paintEvent(QPaintEvent *event)
{
    QTime time;
    time.start();
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    renderer->paint(&painter, event, elapsed, size());
    painter.end();
		emit renderTimeUpdated( time.elapsed() );
    //qWarning("Time to paint: %d ms", time.elapsed());
}

void ViewWidget::keyPressEvent(QKeyEvent *event)
{
	QVector3D K_pos = getKpos();
	float focus = getFocal();

	const float angleStep = 0.1;
	const float posStep = .5f;
	const float focusStep = 10.f;
	const float apertureStep = 1.f;
	const float cameraFovStep = 1.f;
	if(event->key() == Qt::Key_A) {
		K_pos.setX(K_pos.x() - posStep);
		setKpos(K_pos);
    }
	if (event->key() == Qt::Key_D) {
		K_pos.setX(K_pos.x() + posStep);
		setKpos(K_pos);
	}
    if(event->key() == Qt::Key_W){
		K_pos.setY(K_pos.y() + posStep);
		setKpos(K_pos);
    }
    if(event->key() == Qt::Key_S){
		K_pos.setY(K_pos.y() - posStep);
		setKpos(K_pos);
	}
    if(event->key() == Qt::Key_Q){
		K_pos.setZ(K_pos.z() - posStep);
		setKpos(K_pos);
    }
    if(event->key() == Qt::Key_E){
		K_pos.setZ(K_pos.z() + posStep);
		setKpos(K_pos);
    }
    if(event->key() == Qt::Key_Left){
		float x = K_pos.x()*cos(-angleStep/180.0 * M_PI) - K_pos.y()*sin(-angleStep/180.0 * M_PI);
		float y = K_pos.x()*sin(-angleStep/180.0 * M_PI) + K_pos.y()*cos(-angleStep/180.0 * M_PI);
		K_pos.setX(x);
		K_pos.setY(y);
		setKpos(K_pos);
    }
    if(event->key() == Qt::Key_Right){
		float x = K_pos.x()*cos(angleStep/180.0 * M_PI) - K_pos.y()*sin(angleStep/180.0 * M_PI);
		float y = K_pos.x()*sin(angleStep/180.0 * M_PI) + K_pos.y()*cos(angleStep/180.0 * M_PI);
		K_pos.setX(x);
		K_pos.setY(y);
		setKpos(K_pos);
    }
    if(event->key() == Qt::Key_Up){
		setFocal(std::min(getFocal() + focusStep, max_focal));
    }
    if(event->key() == Qt::Key_Down){
		setFocal(std::max(getFocal() - focusStep, min_focal));		
    }
	if (event->key() == Qt::Key_Minus) {
		setFov(std::max(getFov() - cameraFovStep, min_fov));
	}
	if (event->key() == Qt::Key_Equal) {
		setFov(std::min(getFov() + cameraFovStep, max_fov));
	}
	event->accept();
}

void ViewWidget::mousePressEvent(QMouseEvent * event)
{
	mouseDragStart = event->pos();
	mouseDragStartK_pos = renderer->K_pos;
	mouseDragFocal = renderer->focus;
	mouseDragButton = event->button();
	event->accept();
}

void ViewWidget::mouseMoveEvent(QMouseEvent * event)
{
	QPoint delta = event->pos() - mouseDragStart;

	if (mouseDragButton == Qt::RightButton) {
		float new_focal = mouseDragFocal - (float)delta.y() / 2.f;
		setFocal(std::min(std::max(new_focal, min_focal), max_focal));
	}
	else {
		QVector3D newK_pos;
		if (event->modifiers() == Qt::ShiftModifier) {
			newK_pos = mouseDragStartK_pos;
		}
		else {
			float ang = -delta.x();
			float x = mouseDragStartK_pos.x()*cos(ang/180.0 * M_PI) - mouseDragStartK_pos.y()*sin(ang/180.0 * M_PI);
			float y = mouseDragStartK_pos.x()*sin(ang/180.0 * M_PI) + mouseDragStartK_pos.y()*cos(ang/180.0 * M_PI);
			newK_pos = mouseDragStartK_pos + QVector3D(delta.x(), 0.f, -delta.y()) / 2.f;newK_pos = mouseDragStartK_pos;
			newK_pos.setX(x);
			newK_pos.setY(y);
		}
		setKpos(newK_pos);
	}
	event->accept();
}

void ViewWidget::wheelEvent(QWheelEvent * event)
{
	QPoint numDegrees = event->angleDelta() / 8 / 15;

	double camera_fov = getAperture() + numDegrees.y();
	camera_fov = std::min(double(max_fov), camera_fov);
	camera_fov = std::max(double(min_fov), camera_fov);
	setFov(camera_fov);	

	event->accept();	
}

QVector3D ViewWidget::getKpos()
{
	return renderer->K_pos;
}

float ViewWidget::getFocal()
{
	return renderer->focus;
}

float ViewWidget::getAperture()
{
	return renderer->apertureSize;
}

float ViewWidget::getFov()
{
	return renderer->camera_fov;
}
