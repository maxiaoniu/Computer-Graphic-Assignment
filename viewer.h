#ifndef VIEWER_H
#define VIEWER_H

#include <QOpenGLWindow>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include "trackball.h"
#include "transform.h"
#include "camera.h"
#include "floor.h"

#include <QOpenGLWidget>
#include <QTimer>
#include <QMatrix4x4>
class QOpenGLShaderProgram;
class QOpenGLShader;
class QOpenGLTexture;
class ObjMesh;

class Viewer :  public QOpenGLWidget,
        protected QOpenGLFunctions
{
    Q_OBJECT
public:

    explicit Viewer(QWidget *parent = 0);
    ~Viewer();
protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void teardownGL();

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void keyPressEvent(QKeyEvent *event);
private:
    TrackBall m_trackball;
    TrackBall m_trackballCamera;
    QTimer *m_timer;

    QOpenGLBuffer m_vertex;
    ObjMesh *m_mesh;

    QOpenGLVertexArrayObject m_object;
    QOpenGLShaderProgram *m_program;

    QOpenGLTexture *m_floorTexture;
    QOpenGLTexture *m_cubeTexture;
    QOpenGLShader *m_environmentShader;
    QOpenGLShaderProgram *m_floorProgram;
    Floor *m_floor;

    int u_modelToWorld;
    int u_worldToCamera;
    int u_cameraToView;
    int u_normalMatrix;
    int u_staticLightProerties;

    int u_worldToCameraFloor;
    int u_cameraToViewFloor;

    Camera m_camera;
    Transform m_transform;
    Transform m_lightTransform;
    QMatrix4x4 m_projection;

    QVector3D m_objCentralPoint;
    QVector3D m_objSize;

    float m_cameraZ;
    QRgb m_color;

    float m_perspectiveNear;
    float m_perspectiveFar;

    struct lightProperties{
        bool isEnable;
        QVector4D pos;
        QRgb ambient;
        QRgb diffuse;
        QRgb specular;
        int rotateSpeed;
        QVector3D rotateAxis;
    };
    int m_shaderMode;
    lightProperties *m_staticLight;
    lightProperties *m_dynamicLight;


    QPointF PosToViewPos(const QPoint& p);
    QImage convertFromBumpToNormal(QImage img);

signals:

public slots:
    void recFile(const QString &name);
    void setColorParameter(QRgb color);
    void setRenderMode(int mode);
    void setShaderMode(int mode);
    void setFaceOritation(int mode);
    void setPerspectiveNear(int value);
    void setPerspectiveFar(int value);
    void resetObjectPos();

    //light
    void setStaticLightAmbient(QRgb color);
    void setStaticLightDiffuse(QRgb color);
    void setStaticLightSpecular(QRgb color);
    void setStaticLightState(int state);

    void setDynamicLightAmbient(QRgb color);
    void setDynamicLightDiffuse(QRgb color);
    void setDynamicLightSpecular(QRgb color);
    void setDynamicLightState(int state);
    void setDynamicLightRotateSpeed(int value);
    void setDynamicLightRotateAxis(int value);
};

#endif // VIEWER_H
