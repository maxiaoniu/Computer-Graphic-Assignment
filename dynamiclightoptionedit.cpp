#include "dynamiclightoptionedit.h"

DynamicLightOptionEdit::DynamicLightOptionEdit(QWidget *parent) : QWidget(parent)
{
    QGridLayout *layout = new QGridLayout;
    setLayout(layout);
    int row=0;

//    m_onOffCheck = new QCheckBox;
//    layout->addWidget(m_onOffCheck);
//    connect(m_onOffCheck,SIGNAL(stateChanged(int)),this,SIGNAL(lightStateChanged(int)));
//    row++;

    QLabel* ambientColorLabel=new QLabel(tr("Ambient Color:"));
    layout->addWidget(ambientColorLabel);
    row++;
    ColorButton *m_ambientColorEdit = new ColorButton;
    layout->addWidget(m_ambientColorEdit);
    connect(m_ambientColorEdit, &ColorButton::colorChanged, this, &DynamicLightOptionEdit::setAmbientColorParameter);
    row++;

    QLabel* diffuseColorLabel=new QLabel(tr("Diffuse Color:"));
    layout->addWidget(diffuseColorLabel);
    row++;
    ColorButton *m_diffuseColorEdit = new ColorButton;
    layout->addWidget(m_diffuseColorEdit);
    connect(m_diffuseColorEdit, &ColorButton::colorChanged, this, &DynamicLightOptionEdit::setDiffuseColorParameter);
    row++;


    QLabel* specularColorLabel=new QLabel(tr("Specular Color:"));
    layout->addWidget(specularColorLabel);
    row++;
    ColorButton *m_specularColorEdit = new ColorButton;
    layout->addWidget(m_specularColorEdit);
    connect(m_specularColorEdit, &ColorButton::colorChanged, this, &DynamicLightOptionEdit::setSpecularColorParameter);
    row++;

    QCheckBox* m_onOffCheck = new QCheckBox;
    layout->addWidget(m_onOffCheck);
    connect(m_onOffCheck,SIGNAL(stateChanged(int)),this,SIGNAL(lightStateChanged(int)));
    row++;
    QComboBox *m_axis = new QComboBox;
    m_axis->addItem("x-Axis");
    m_axis->addItem("y-Axis");
    m_axis->addItem("z-Axis");
    connect(m_axis,SIGNAL(currentIndexChanged(int)),this,SIGNAL(rotateAxisChanged(int)));
    layout->addWidget(m_axis);
    row++;
    QSlider *slider = new QSlider(Qt::Horizontal);
    layout->addWidget(slider);
    slider->setFocusPolicy(Qt::StrongFocus);
    slider->setTickPosition(QSlider::TicksBothSides);
    slider->setTickInterval(10);
    slider->setSingleStep(1);
    slider->setMaximum(10);
    slider->setMinimum(0);
    connect(slider,SIGNAL(valueChanged(int)),this,SIGNAL(lightRotateSpeedChanged(int)));
    row++;
}

void DynamicLightOptionEdit::setAmbientColorParameter(QRgb color)
{
    emit ambientColorChanged(color);
}

void DynamicLightOptionEdit::setDiffuseColorParameter(QRgb color)
{
    emit diffuseColorChanged(color);
}

void DynamicLightOptionEdit::setSpecularColorParameter(QRgb color)
{
    emit specularColorChanged(color);
}


DynamicLightOptionEdit::~DynamicLightOptionEdit()
{
}
