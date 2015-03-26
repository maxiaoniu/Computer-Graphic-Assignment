#include "colorbutton.h"

ColorButton::ColorButton()
{
    QHBoxLayout *layout = new QHBoxLayout;
    setLayout(layout);
    layout->setContentsMargins(0, 0, 0, 0);

    //init color
    m_color = 0xffffffff;
    emit colorChanged(m_color);

    m_lineEdit = new QLineEdit(QString::number(m_color, 16));
    layout->addWidget(m_lineEdit);

    m_button = new QFrame;
    QPalette palette = m_button->palette();
    palette.setColor(QPalette::Window, QColor(m_color));
    m_button->setPalette(palette);
    m_button->setAutoFillBackground(true);
    m_button->setMinimumSize(32, 0);
    m_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    m_button->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    layout->addWidget(m_button);
    connect(m_lineEdit, &QLineEdit::editingFinished, this, &ColorButton::editDone);
}

void ColorButton::editDone()
{
    bool ok;
    QRgb newColor = m_lineEdit->text().toUInt(&ok, 16);
    if (ok)
        setColor(newColor);
}

void ColorButton::setColor(QRgb color)
{
    m_color = color;
    m_lineEdit->setText(QString::number(m_color, 16)); // "Clean up" text
    QPalette palette = m_button->palette();
    palette.setColor(QPalette::Window, QColor(m_color));
    m_button->setPalette(palette);
    emit colorChanged(m_color);
}

void ColorButton::mousePressEvent(QMouseEvent * event)
{
    if (event->button() == Qt::LeftButton)
    {
        QColor color(m_color);
        QColorDialog dialog(color, 0);
        //dialog.setOption(QColorDialog::ShowAlphaChannel, true);
        dialog.move(280, 120);
        if (dialog.exec() == QDialog::Rejected)
            return;
        QRgb newColor = dialog.selectedColor().rgba();
        if (newColor == m_color)
            return;
        setColor(newColor);
    }
}

ColorButton::~ColorButton()
{

}

