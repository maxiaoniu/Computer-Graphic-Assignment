#ifndef TRACKBALL_H
#define TRACKBALL_H

#include <QVector3D>
#include <QQuaternion>
#include <QPointF>

class TrackBall
{
public:
    TrackBall();
    ~TrackBall();

    void push(const QPointF& p);
    void move(const QPointF& p, const QQuaternion &rotation);
    void release(const QPointF& p, const QQuaternion &rotation);

    QQuaternion rotation();
    void init();
private:
    QQuaternion m_rotation;
    QVector3D m_axis;

    QPointF m_lastPos;
    bool m_pressed;





};

#endif // TRACKBALL_H
