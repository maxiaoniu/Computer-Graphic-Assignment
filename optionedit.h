#ifndef OPTIONEDIT_H
#define OPTIONEDIT_H

#include <QtWidgets>

class OptionEdit : public QWidget
{
    Q_OBJECT
public:
    explicit OptionEdit(QWidget *parent = 0);
    ~OptionEdit();

private:
    QComboBox *m_renderCombo;
    QComboBox *m_orientationCombo;
    QComboBox *m_bumpImage;
    QSpinBox *m_perspectiveAngle;
    QSpinBox *m_perspectiveFar;
    QSpinBox *m_perspectiveNear;

    QPushButton *m_resetButton;

signals:
    void colorChanged(QRgb color);
    void ambientColorChanged(QRgb color);
    void rendermodeChanged(int mode);
    void shadermodeChanged(int mode);
    void oritationChanged(int mode);
    void farPerspectiveDistanceChanged(int value);
    void nearPerspectiveDistanceChanged(int value);
    void resetObjectPosSignal();
    void bumpChanged(int mode);
    void textureStateChanged(int state);

private slots:
    void setColorParameter(QRgb color);
    void setAmbientColorParameter(QRgb color);

};



#endif // OPTIONEDIT_H
