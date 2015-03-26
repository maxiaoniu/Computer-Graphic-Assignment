#ifndef DYNAMICLIGHTOPTIONEDIT_H
#define DYNAMICLIGHTOPTIONEDIT_H

#include <QtWidgets>
#include "colorbutton.h"

class DynamicLightOptionEdit : public QWidget
{
    Q_OBJECT
public:
    explicit DynamicLightOptionEdit(QWidget *parent = 0);
    ~DynamicLightOptionEdit();

signals:
    void ambientColorChanged(QRgb color);
    void diffuseColorChanged(QRgb color);
    void specularColorChanged(QRgb color);
    void lightStateChanged(int state);
    void lightRotateSpeedChanged(int value);
    void rotateAxisChanged(int);

private slots:
    void setAmbientColorParameter(QRgb color);
    void setDiffuseColorParameter(QRgb color);
    void setSpecularColorParameter(QRgb color);
};

#endif // DYNAMICLIGHTOPTIONEDIT_H
