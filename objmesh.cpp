#include "objmesh.h"

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>
#include <QDebug>
#include <QString>
class ObjMeshPrivate
{
public:
    ObjMeshPrivate();
    QOpenGLBuffer *m_buffer;
    QOpenGLVertexArrayObject *m_vao;
    int m_count;
    GLenum m_mode;

    int teststring;

};

ObjMeshPrivate::ObjMeshPrivate()
{

}

ObjMesh::ObjMesh() : m_private(new ObjMeshPrivate)
{
}


void ObjMesh::createBuffer(QOpenGLBuffer::UsagePattern hint)
{

    m_private->m_buffer = new QOpenGLBuffer();
    m_private->m_buffer->create();
    m_private->m_buffer->setUsagePattern(hint);
}
QOpenGLBuffer* ObjMesh::getBuffer()
{
    return m_private->m_buffer;
}

void ObjMesh::createVertexArrayObjct()
{
    m_private->m_vao = new QOpenGLVertexArrayObject();
    m_private->m_vao->create();
    m_private->teststring = 232;

}

QOpenGLVertexArrayObject *ObjMesh::getVertexArrayObject()
{
    return m_private->m_vao;

}


void ObjMesh::setDrawArrays(GLenum mode, int count)
{

  m_private -> m_mode = mode;
  m_private -> m_count = count;
}


void ObjMesh::draw()
{

    qDebug()<< m_private->teststring;
    m_private->m_vao->bind();
    QOpenGLFunctions f(QOpenGLContext::currentContext());
    f.glDrawArrays(m_private->m_mode, 0, m_private->m_count);
    m_private->m_vao->release();

}

ObjMesh::~ObjMesh()
{

}

