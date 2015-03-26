#include "trackball.h"
#include <math.h>
#include <QVector3D>
#include <QQuaternion>


#define PI 3.1415926
TrackBall::TrackBall()
{
    m_axis = QVector3D(0,1,0);// y axis
    m_rotation = QQuaternion().fromAxisAndAngle(m_axis,0);
}

TrackBall::~TrackBall()
{

}

void TrackBall::push(const QPointF &p)
{
    //m_rotation = rotation();
    m_pressed = true;
    m_lastPos = p;

}

void TrackBall::move(const QPointF &p, const QQuaternion &rotation)
{
    if(!m_pressed)
        return;

    //2D to 3D
    QVector3D lastPos3D = QVector3D(m_lastPos.x(), m_lastPos.y(), 0.0f);
    float sqrZ = 1 - QVector3D::dotProduct(lastPos3D, lastPos3D);
    if (sqrZ > 0)
        lastPos3D.setZ(sqrt(sqrZ));
    else
        lastPos3D.normalize();

    QVector3D currentPos3D = QVector3D(p.x(), p.y(), 0.0f);
    sqrZ = 1 - QVector3D::dotProduct(currentPos3D, currentPos3D);

    if (sqrZ > 0)
        currentPos3D.setZ(sqrt(sqrZ));
    else
        currentPos3D.normalize();


    m_axis = QVector3D::crossProduct(lastPos3D, currentPos3D);
    float angle = 180 / PI * asin(sqrt(QVector3D::dotProduct(m_axis, m_axis)));

    m_axis.normalize();
    m_axis = rotation.rotatedVector(m_axis);
    m_rotation = QQuaternion::fromAxisAndAngle(m_axis,angle)*m_rotation;
    m_lastPos = p;
}

void TrackBall::release(const QPointF &p, const QQuaternion &rotation)
{
    move(p,rotation);
    m_pressed = false;
}

QQuaternion TrackBall::rotation()
{
    //if (m_pressed)
        return m_rotation;
}

void TrackBall::init()
{
    m_axis = QVector3D(0,1,0);// y axis
    m_rotation = QQuaternion().fromAxisAndAngle(m_axis,0);
}

