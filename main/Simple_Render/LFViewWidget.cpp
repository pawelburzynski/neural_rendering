#include "LFViewWidget.h"
#include "LightFieldRenderer.h"
#include <algorithm>

#include <QPainter>
#include <QTimer>
#include <QTime>

#define LF_max_focal 15000.f
#define LF_min_focal 1.f

#define LF_min_fov 2.f
#define LF_max_fov 180.f


LFViewWidget::LFViewWidget(LightFieldRenderer *renderer, QWidget *parent)
    : QOpenGLWidget(parent), lfRenderer(renderer)
{
    elapsed = 0;
		setMinimumSize(256, 256);
    setAutoFillBackground(false);
		setFocusPolicy(Qt::StrongFocus);
}

QSize LFViewWidget::sizeHint() const
{
		return QSize(1024,1024);
}

void LFViewWidget::animate()
{
    elapsed = (elapsed + qobject_cast<QTimer*>(sender())->interval()) % 1000;
//    update();
}

void LFViewWidget::setKpos(QVector3D newK_pos)
{
	lfRenderer->K_pos = newK_pos;
	update();
	emit KposChanged(newK_pos);
}

void LFViewWidget::setFocal(double newFocus)
{
	lfRenderer->focus = newFocus;
	update();
	emit focalChanged(newFocus);
}


void LFViewWidget::setAperture(double newAperture)
{
	lfRenderer->apertureSize = newAperture;
	update();
	emit apertureChanged(newAperture);
}

void LFViewWidget::setFov(double newFov)
{
	lfRenderer->camera_fov = newFov;
	update();
	emit fovChanged(newFov);
}

void LFViewWidget::paintEvent(QPaintEvent *event)
{
    QTime time;
    time.start();
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    lfRenderer->paint(&painter, event, elapsed, size());
    painter.end();
		emit renderTimeUpdated( time.elapsed() );
    //qWarning("Time to paint: %d ms", time.elapsed());
}

void LFViewWidget::keyPressEvent(QKeyEvent *event)
{
	QVector3D K_pos = getKpos();
	float focus = getFocal();

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
		setAperture(getAperture() - apertureStep);
    }
    if(event->key() == Qt::Key_Right){
		setAperture(getAperture() + apertureStep);	
    }
    if(event->key() == Qt::Key_Up){
		setFocal(std::min(getFocal() + focusStep, LF_max_focal));
    }
    if(event->key() == Qt::Key_Down){
		setFocal(std::max(getFocal() - focusStep, LF_min_focal));		
    }
	if (event->key() == Qt::Key_Minus) {
		setFov(std::max(getFov() - cameraFovStep, LF_min_fov));
	}
	if (event->key() == Qt::Key_Equal) {
		setFov(std::min(getFov() + cameraFovStep, LF_max_fov));
	}
	event->accept();
}

void LFViewWidget::mousePressEvent(QMouseEvent * event)
{
	mouseDragStart = event->pos();
	mouseDragStartK_pos = lfRenderer->K_pos;
	mouseDragFocal = lfRenderer->focus;
	mouseDragButton = event->button();
	event->accept();
}

void LFViewWidget::mouseMoveEvent(QMouseEvent * event)
{
	QPoint delta = event->pos() - mouseDragStart;

	if (mouseDragButton == Qt::RightButton) {
		float new_focal = mouseDragFocal - (float)delta.y() / 2.f;
		setFocal(std::min(std::max(new_focal, LF_min_focal), LF_max_focal));
	}
	else {
		QVector3D newK_pos;
		if (event->modifiers() == Qt::ShiftModifier) {
			newK_pos = mouseDragStartK_pos + QVector3D(delta.x(), delta.y(), 0.f) / 2.f;
		}
		else {
			newK_pos = mouseDragStartK_pos + QVector3D(delta.x(), 0.f, -delta.y()) / 2.f;
			//setFocal(mouseDragFocal - (float)delta.y());
		}
		setKpos(newK_pos);
	}
	event->accept();
}

void LFViewWidget::wheelEvent(QWheelEvent * event)
{
	QPoint numDegrees = event->angleDelta() / 8 / 15;

	double aperture = getAperture() + numDegrees.y();
	aperture = std::min(100., aperture);
	aperture = std::max(1., aperture);
	setAperture(aperture);	

	event->accept();	
}

QVector3D LFViewWidget::getKpos()
{
	return lfRenderer->K_pos;
}

float LFViewWidget::getFocal()
{
	return lfRenderer->focus;
}

float LFViewWidget::getAperture()
{
	return lfRenderer->apertureSize;
}

float LFViewWidget::getFov()
{
	return lfRenderer->camera_fov;
}
