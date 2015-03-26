#ifndef DIRECTIONLIGHT_H
#define DIRECTIONLIGHT_H

class QVector3D;
class QColor;

class DirectionLightPrivate;
class DirectionLight
{
public:
    DirectionLight();
    ~DirectionLight();

    //light
    void setDirection(float x, float y, float z);
    void setDirection(QVector3D const &dir);
    QVector3D const &direction() const;
    void setDiffuse(float r, float g, float b);
    void setDiffuse(QColor const &rgb);
    QVector3D const &diffuse() const;
    void setSpecular(float r, float g, float b);
    void setSpecular(QColor const &rgb);
    QColor const &specular();

private:
    DirectionLightPrivate *m_private;

};

#endif // DIRECTIONLIGHT_H
