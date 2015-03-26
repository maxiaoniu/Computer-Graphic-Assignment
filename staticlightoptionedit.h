#ifndef STATICLIGHTOPTIONEDIT_H
#define STATICLIGHTOPTIONEDIT_H

#include <QtWidgets>
#include "colorbutton.h"

class StaticLightOptionEdit : public QWidget
{
    Q_OBJECT
public:
    explicit StaticLightOptionEdit(QWidget *parent = 0);
    ~StaticLightOptionEdit();
private:
   // QCheckBox *m_onOffCheck;

signals:
    void ambientColorChanged(QRgb color);
    void diffuseColorChanged(QRgb color);
    void specularColorChanged(QRgb color);
    void lightStateChanged(int state);

private slots:
    void setAmbientColorParameter(QRgb color);
    void setDiffuseColorParameter(QRgb color);
    void setSpecularColorParameter(QRgb color);
};

#endif // STATICLIGHTOPTIONEDIT_H
