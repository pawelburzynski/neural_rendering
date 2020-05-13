#ifndef RendererUtils_H
#define RendererUtils_H

#include <QVector3D>
#include <QMatrix4x4>

#include <QImage>
#include <QtWidgets>

namespace renderer
{
    inline QMatrix4x4 getCurrInvTransMat(float camera_fov, QVector3D K_pos, int viewWidth, int viewHeight) {
        // Intrinsic camera matrix of data camera
        float proj_d = 2.0*tan(camera_fov / 2.f * M_PI / 180.f);
        QMatrix4x4 K(viewWidth/proj_d, 0.0f, viewWidth / 2.0f, 0.0f,
            0.0f, -viewWidth/proj_d, viewHeight / 2.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f);

        // Projection matrix for cameras in the array
        QMatrix4x4 P(1.0f,     0.0f,      0.0f,  0.0f,
                        0.0f, 1.0f,      0.0f,  0.0f,
                        0.0f,     0.0f,  1.0f,  0.0f,
                        0.0f,      0.0f,      0.0f,  0.0f);
        QVector3D up = QVector3D(0.0f, 0.0f, 1.0f); 
        QVector3D cameraRight = QVector3D::crossProduct(up,K_pos);
        QVector3D cameraUp = QVector3D::crossProduct(K_pos, cameraRight);
        cameraUp.normalize();
        QMatrix4x4 V;  // View matrix for the virtual camera K
        V.lookAt(K_pos, QVector3D(0.0f, 0.0f, 0.0f), -cameraUp);
        QMatrix4x4 proMatrix = K * P * V;  // From World to Virtual camera K coordinates
        QVector4D row(K_pos.x(),K_pos.y(),K_pos.z(),0);
        row.normalize();
        proMatrix.setRow(3,row);
        QMatrix4x4 invProMatrix = proMatrix.inverted();
        return invProMatrix;
    }

    inline double angle(QVector4D pos) {
        double s = asin(pos.y() / sqrt(pos.x()*pos.x()+pos.y()*pos.y()));
        if (pos.x() > - 0.0001) {
            return s + M_PI/2.0;
        } else {
            return 3*M_PI/2.0-s;
        }
    }

    inline double angle(QVector3D pos) {
        double s = asin(pos.y() / sqrt(pos.x()*pos.x()+pos.y()*pos.y()));
        if (pos.x() > - 0.0001) {
            return s + M_PI/2.0;
        } else {
            return 3*M_PI/2.0-s;
        }
    }

    inline double dist(QVector4D p1, QVector4D p2) {
        return sqrt((p1.x()-p2.x())*(p1.x()-p2.x())+(p1.y()-p2.y())*(p1.y()-p2.y())+(p1.z()-p2.z())*(p1.z()-p2.z()));
    }
}

#endif // RendererUtils_H