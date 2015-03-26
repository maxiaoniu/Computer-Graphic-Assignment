#include "filereader.h"
#include <QDebug>
#include <QPushButton>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QDir>

FileReader::FileReader(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout;
    m_openFileButton = new QPushButton("Open File",parent);
    layout->addWidget(m_openFileButton);
    parent->setLayout(layout);
    connect(m_openFileButton,&QPushButton::clicked,
            this,&FileReader::on_open_button_clicked);
}

FileReader::~FileReader()
{

}

void FileReader::on_open_button_clicked()
{
    QDir d;
    qDebug()<<d.currentPath();
    QString fileName = QFileDialog::getOpenFileName(
      this,"Select Obj file",
      d.currentPath(),
      "Object File (*.in)");

    if (!fileName.isNull())
    {
        emit fileChanged(fileName);
    }

}

