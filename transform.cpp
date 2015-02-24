
#include <QQuaternion>
#include "transform.h"
Transform::Transform(): m_dirty(true),m_scale(1.0f, 1.0f, 1.0f){}

const QVector3D Transform::LocalForward(0.0f, 0.0f, 1.0f);
const QVector3D Transform::LocalUp(0.0f, 1.0f, 0.0f);
const QVector3D Transform::LocalRight(1.0f, 0.0f, 0.0f);

Transform::~Transform()
{

}

void Transform::translate(const QVector3D& dt)
{
    m_dirty = true;
    m_translation += dt;

}

void Transform::scale(const QVector3D &ds)
{
    m_dirty = true;
    m_scale = m_scale*ds;
}

void Transform::scale(const float factor)
{
    m_dirty = true;
    m_scale = m_scale*QVector3D(factor,factor,factor);
}

void Transform::rotate(float angle, const QVector3D &axis)
{
    m_dirty = true;
    m_rotation = m_rotation*QQuaternion::fromAxisAndAngle(axis,angle);
}

const QVector3D& Transform::getTranslation()
{
    return m_translation;
}

const QVector3D& Transform::getScale()
{
    return m_scale;
}

const QQuaternion &Transform::getRotation()
{
    return m_rotation;
}

const QMatrix4x4& Transform::getMatrix()
{
    if(m_dirty)
    {
        m_dirty = false;
        m_model2world.setToIdentity();
        //must in order translation*rotation*scale
        m_model2world.translate(m_translation);
        m_model2world.rotate(m_rotation);
        m_model2world.scale(m_scale);

    }

    return m_model2world;
}



QVector3D Transform::forward()
{
  return m_rotation.rotatedVector(LocalForward);
}

QVector3D Transform::up()
{
  return m_rotation.rotatedVector(LocalUp);
}

QVector3D Transform::right()
{
  return m_rotation.rotatedVector(LocalRight);
}





