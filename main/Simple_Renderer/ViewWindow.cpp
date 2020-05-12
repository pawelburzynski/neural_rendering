#include <QGridLayout>
#include <QTimer>
#include <QShortcut>

#define _USE_MATH_DEFINES
#include <math.h>

#include "ViewWindow.h"


ViewerWindow::ViewerWindow(Renderer* r) : renderer(r)
{
    setWindowTitle(tr("Light Field Renderer"));

	cameraPosLabel = new QLabel();
    cameraPosLabel->setAlignment(Qt::AlignHCenter);

	ViewWidget * viewWidget = new ViewWidget(renderer, this);

	QHBoxLayout *main_layout = new QHBoxLayout;
    main_layout->addWidget(viewWidget,2);
	QVBoxLayout *controls_layout = new QVBoxLayout;
	main_layout->addLayout(controls_layout);
	controls_layout->addWidget(cameraPosLabel);

	QHBoxLayout *fovlayout = new QHBoxLayout;
	QLabel *fovLabel = new QLabel(tr("Field-of-view"));
	fovLabel->setAlignment(Qt::AlignRight);
	fovlayout->addWidget(fovLabel);
	fovSB = new QDoubleSpinBox(this);
	fovSB->setMinimum(2);
	fovSB->setMaximum(180);
	fovlayout->addWidget(fovSB);
	controls_layout->addLayout(fovlayout);
	focalLengthLabel = new QLabel();
	focalLengthLabel->setAlignment(Qt::AlignRight);
	controls_layout->addWidget(focalLengthLabel);

	controls_layout->addStretch();

	QLabel *controlsHelpLabel = new QLabel(tr("To move the camera:\n Hold LMB and move (X/Z);\n"
			"To change fov:\n Scroll wheel;\n"
			));
	controls_layout->addWidget( controlsHelpLabel );
	renderTimeLabel = new QLabel(this);
	controls_layout->addWidget( renderTimeLabel );

	QWidget *central_widget = new QWidget( this );
	central_widget->setLayout(main_layout);
	setCentralWidget(central_widget);

	QShortcut *quit_shortcut = new QShortcut(QKeySequence(tr("Ctrl+Q", "Quit")), this );
	connect(quit_shortcut, &QShortcut::activated, this, &ViewerWindow::close);


    viewWidget->setFocus();

	connect(viewWidget, &ViewWidget::KposChanged, this, &ViewerWindow::KposChanged);
	connect(viewWidget, &ViewWidget::fovChanged, fovSB, &QDoubleSpinBox::setValue);
	connect(viewWidget, &ViewWidget::fovChanged, this, &ViewerWindow::fovUpdated);

	connect(viewWidget, &ViewWidget::renderTimeUpdated, this, &ViewerWindow::renderTimeUpdated);

	connect(fovSB, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), viewWidget, &ViewWidget::setFov);

	// Update labels
	viewWidget->setFov(viewWidget->getFov());
}

void ViewerWindow::KposChanged(QVector3D newKpos)
{
	QString label = QString("Camera Position\n(%1, %2, %3)").arg(QString::number(newKpos.x()), QString::number(newKpos.y()), QString::number(newKpos.z()));
	cameraPosLabel->setText(label);
}

void ViewerWindow::renderTimeUpdated(int renderTime)
{
	QString label = QString("Rendering time %1 ms").arg(QString::number(renderTime));
	renderTimeLabel->setText(label);
}

void ViewerWindow::fovUpdated(double fov)
{
	const double sensorWidth = 36.;
	double focalLength = sensorWidth / 2. / tan(fov / 2. * M_PI / 180.);
	QString label = QString("Focal length: %1 mm").arg(QString::number(focalLength));
	focalLengthLabel->setText(label);
}

void ViewerWindow::generateEvaluationOutput(QString data_dir, QString output_dir) 
{
	renderer->generateEvaluationOutput(data_dir.toLocal8Bit(), output_dir.toLocal8Bit());
}