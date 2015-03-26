#ifndef OBJMESH_H
#define OBJMESH_H

#include <QOpenGLBuffer>
#include <QVector3D>
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
    void setDrawArrays(int count);
    void setDrawMode(GLenum mode);
    void setOritation(GLenum mode);

    void setCentralPoint(const QVector3D &central);
    QVector3D getCentralPoint();
    //get the object's width*height*depth.
    void setObjSize(const QVector3D &size);
    QVector3D getObjSize();

    void draw();

private:
    ObjMeshPrivate *m_private;

};

#endif // OBJMESH_H
