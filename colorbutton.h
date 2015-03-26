#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include <QtWidgets>

class ColorButton : public QWidget
{
    Q_OBJECT
public:
    ColorButton();
    ~ColorButton();
protected:
    virtual void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    QLineEdit *m_lineEdit;
    QFrame *m_button;
    QRgb m_color;
    void setColor(QRgb color);
signals:
    void colorChanged(QRgb color);

private slots:
    void editDone();
};

#endif // COLORBUTTON_H
