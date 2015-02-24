#ifndef OBJMESH_H
#define OBJMESH_H

#include <QOpenGLBuffer>

class QOpenGLVertexArrayObject;
class ObjMeshPrivate;
class ObjMesh
{
public:
    ObjMesh();
    ~ObjMesh();


    QOpenGLBuffer *getBuffer();
    QOpenGLVertexArrayObject *getVertexArrayObject();
    void createBuffer(QOpenGLBuffer::UsagePattern hint);
    void createVertexArrayObjct();
    void setDrawArrays(GLenum mode, int count);

    void draw();

private:
    ObjMeshPrivate *m_private;

};

#endif // OBJMESH_H
