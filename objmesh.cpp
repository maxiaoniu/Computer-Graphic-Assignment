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
    ~ObjMeshPrivate();

    QOpenGLBuffer *m_buffer;
    QOpenGLVertexArrayObject *m_vao;
    int m_count;
    GLenum m_mode;
    GLenum m_oritation;

    QVector3D m_centralPoint;
    QVector3D m_objSize;

};

ObjMeshPrivate::ObjMeshPrivate()
{
    m_mode = GL_LINES;
    m_oritation = GL_CCW;

}

ObjMeshPrivate::~ObjMeshPrivate()
{
    delete m_buffer;
    delete m_vao;

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

}

QOpenGLVertexArrayObject *ObjMesh::getVertexArrayObject()
{
    return m_private->m_vao;

}


void ObjMesh::setDrawArrays(int count)
{
  m_private -> m_count = count;
}


void ObjMesh::setDrawMode(GLenum mode)
{
  m_private -> m_mode = mode;
}

void ObjMesh::setOritation(GLenum mode)
{
  m_private -> m_oritation = mode;
}

void ObjMesh::setCentralPoint(const QVector3D &central)
{
    m_private->m_centralPoint = central;
}

QVector3D ObjMesh::getCentralPoint()
{
    return m_private->m_centralPoint;
}



void ObjMesh::setObjSize(const QVector3D &size)
{
    m_private->m_objSize = size;
}
QVector3D ObjMesh::getObjSize()
{
   return m_private->m_objSize;
}

void ObjMesh::draw()
{
    m_private->m_vao->bind();

    QOpenGLFunctions f(QOpenGLContext::currentContext());

    f.glFrontFace(m_private->m_oritation);
    f.glDrawArrays(m_private->m_mode, 0, m_private->m_count);
    m_private->m_vao->release();

}

ObjMesh::~ObjMesh()
{
    delete m_private;

}

