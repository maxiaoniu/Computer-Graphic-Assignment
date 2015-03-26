#include "staticlightoptionedit.h"

StaticLightOptionEdit::StaticLightOptionEdit(QWidget *parent) : QWidget(parent)
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
    connect(m_ambientColorEdit, &ColorButton::colorChanged, this, &StaticLightOptionEdit::setAmbientColorParameter);
    row++;

    QLabel* diffuseColorLabel=new QLabel(tr("Diffuse Color:"));
    layout->addWidget(diffuseColorLabel);
    row++;
    ColorButton *m_diffuseColorEdit = new ColorButton;
    layout->addWidget(m_diffuseColorEdit);
    connect(m_diffuseColorEdit, &ColorButton::colorChanged, this, &StaticLightOptionEdit::setDiffuseColorParameter);
    row++;


    QLabel* specularColorLabel=new QLabel(tr("Specular Color:"));
    layout->addWidget(specularColorLabel);
    row++;
    ColorButton *m_specularColorEdit = new ColorButton;
    layout->addWidget(m_specularColorEdit);
    connect(m_specularColorEdit, &ColorButton::colorChanged, this, &StaticLightOptionEdit::setSpecularColorParameter);
    row++;

    QCheckBox* m_onOffCheck = new QCheckBox;
    layout->addWidget(m_onOffCheck);
    connect(m_onOffCheck,SIGNAL(stateChanged(int)),this,SIGNAL(lightStateChanged(int)));
    row++;
}

void StaticLightOptionEdit::setAmbientColorParameter(QRgb color)
{
    emit ambientColorChanged(color);
}

void StaticLightOptionEdit::setDiffuseColorParameter(QRgb color)
{
    emit diffuseColorChanged(color);
}

void StaticLightOptionEdit::setSpecularColorParameter(QRgb color)
{
    emit specularColorChanged(color);
}


StaticLightOptionEdit::~StaticLightOptionEdit()
{
}

