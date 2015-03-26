#include "optionedit.h"
#include "colorbutton.h"

OptionEdit::OptionEdit(QWidget *parent) : QWidget(parent)
{
    QGridLayout *layout = new QGridLayout;
    setLayout(layout);
    int row=0;

    layout->addWidget(new QLabel(tr("Rendering Mode:")));
    row++;
    m_renderCombo = new QComboBox;
    m_renderCombo->addItem("Solid");
    m_renderCombo->addItem("Wireframe");
    m_renderCombo->addItem("Points");
    connect(m_renderCombo,SIGNAL(currentIndexChanged(int)),this,SIGNAL(rendermodeChanged(int)));
    layout->addWidget(m_renderCombo);
    row++;

    layout->addWidget(new QLabel(tr("Shader Mode:")));
    row++;
    QComboBox *m_shaderCombo = new QComboBox;
    m_shaderCombo->addItem("Flat");
    m_shaderCombo->addItem("Smooth");
    connect(m_shaderCombo,SIGNAL(currentIndexChanged(int)),this,SIGNAL(shadermodeChanged(int)));
    layout->addWidget(m_shaderCombo);
    row++;

    layout->addWidget(new QLabel(tr("Orientation:")));
    row++;
    m_orientationCombo = new QComboBox;
    m_orientationCombo->addItem("CCW");
    m_orientationCombo->addItem("CW");
    connect(m_orientationCombo,SIGNAL(currentIndexChanged(int)),this,SIGNAL(oritationChanged(int)));
    layout->addWidget(m_orientationCombo);
    row++;

    QLabel* colorLabel=new QLabel(tr("Global Ambient Color:"));
    layout->addWidget(colorLabel);
    row++;
    ColorButton *colorEdit = new ColorButton;
    layout->addWidget(colorEdit);
    connect(colorEdit, &ColorButton::colorChanged, this, &OptionEdit::setColorParameter);
    row++;

    QLabel* ambientColorLabel=new QLabel(tr("Object Color:"));
    layout->addWidget(ambientColorLabel);
    row++;
    ColorButton *ambientColorEdit = new ColorButton;
    layout->addWidget(ambientColorEdit);
    connect(ambientColorEdit, &ColorButton::colorChanged, this, &OptionEdit::setAmbientColorParameter);
    row++;

    layout->addWidget(new QLabel(tr("Projection Parameters:")));
    row++;

    QHBoxLayout *angleLayout = new QHBoxLayout;
    angleLayout->setStretch(1,1);
    angleLayout->setContentsMargins(0, 0, 0, 0);
    angleLayout->addWidget(new QLabel(tr("Angle")));
    angleLayout->setStretch(2,3);
    m_perspectiveAngle = new QSpinBox;
    angleLayout->addWidget(m_perspectiveAngle);
    layout->addLayout(angleLayout,row,0);
    row++;


    QHBoxLayout *nearLayout = new QHBoxLayout;
    nearLayout->setContentsMargins(0, 0, 0, 0);
    nearLayout->addWidget(new QLabel(tr("Near")));
    m_perspectiveNear = new QSpinBox;
    m_perspectiveNear->setRange(1,5000);
    m_perspectiveNear->setValue(1);
    connect(m_perspectiveNear,SIGNAL(valueChanged(int)),this,SIGNAL(nearPerspectiveDistanceChanged(int)));
    nearLayout->addWidget(m_perspectiveNear);
    layout->addLayout(nearLayout,row,0);
    row++;

    QHBoxLayout *farLayout = new QHBoxLayout;
    farLayout->setContentsMargins(0, 0, 0, 0);
    farLayout->addWidget(new QLabel(tr("Far")));
    m_perspectiveFar = new QSpinBox;
    m_perspectiveFar->setRange(1,5000);
    m_perspectiveFar->setValue(3000);
    connect(m_perspectiveFar,SIGNAL(valueChanged(int)),this,SIGNAL(farPerspectiveDistanceChanged(int)));
    farLayout->addWidget(m_perspectiveFar);
    layout->addLayout(farLayout,row,0);
    row++;

    m_resetButton = new QPushButton("Reset Position",parent);
    layout->addWidget(m_resetButton);
    connect(m_resetButton,SIGNAL(clicked()),this,SIGNAL(resetObjectPosSignal()));
    row++;
}

void OptionEdit::setColorParameter(QRgb color)
{
    emit colorChanged(color);
}

void OptionEdit::setAmbientColorParameter(QRgb color)
{
    emit ambientColorChanged(color);
}

OptionEdit::~OptionEdit()
{

}




