#include "floor.h"
#include <QVector3D>
#include <QDebug>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>
Floor::Floor(const float width)
{
    m_width = width;

}
void Floor::create()
{
    m_buffer = new QOpenGLBuffer();
    m_buffer->create();
    m_buffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vao = new QOpenGLVertexArrayObject();
    m_vao->create();

    QOpenGLFunctions f = QOpenGLFunctions(QOpenGLContext::currentContext());

    //creat the vertexes
    float floorVertices[] =
    {
        // Positions       // Texture Coords
        //front
        m_width, -m_width/2,  m_width, 5, 5,
        m_width, -m_width/2, -m_width, 5, 0.0f,
       -m_width, -m_width/2,  m_width, 0.0f, 5,

        m_width, -m_width/2, -m_width, 5, 0.0f,
       -m_width, -m_width/2, -m_width, 0.0f, 0.0f,
       -m_width, -m_width/2,  m_width, 0.0f, 5,
        //back
        m_width, -m_width/2, -m_width, 0.0f, 0.0f,
        m_width, -m_width/2,  m_width, 0.0f, 5.0f,
       -m_width, -m_width/2,  m_width, 5.0f, 5.0f,

       -m_width, -m_width/2, -m_width, 5.0f, 0.0f,
        m_width, -m_width/2, -m_width, 0.0f, 0.0f,
       -m_width, -m_width/2,  m_width, 5.0f, 5.0f
    };
    m_buffer->bind();
    m_buffer->allocate(floorVertices, sizeof(floorVertices));
    m_vao->bind();
    f.glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    f.glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    f.glEnableVertexAttribArray(0);
    f.glEnableVertexAttribArray(1);

    m_vao->release();
    m_buffer->release();
}

void Floor::draw()
{
    m_vao->bind();
    QOpenGLFunctions f = QOpenGLFunctions(QOpenGLContext::currentContext());
    f.glDrawArrays(GL_TRIANGLES, 0, 12);
    m_vao->release();

}

Floor::~Floor()
{
    delete m_buffer;
    delete m_vao;

}

