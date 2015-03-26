#ifndef FLOOR_H
#define FLOOR_H

class QOpenGLBuffer;
class QOpenGLVertexArrayObject;
class Floor
{
public:
    Floor(const float width);
    ~Floor();

    void create();
    void draw();


private:
    float m_width;

    QOpenGLBuffer *m_buffer;
    QOpenGLVertexArrayObject *m_vao;
};

#endif // FLOOR_H
