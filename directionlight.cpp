#include "directionlight.h"

#include <QColor>
#include <QVector3D>
class DirectionLightPrivate
{
public:
    DirectionLightPrivate();
    QVector3D m_direction;
    QColor color;
};

DirectionLight::DirectionLight()
{

}

DirectionLight::~DirectionLight()
{

}

