#include "viewer.h"
#include "objmesh.h"
#include "objparser.h"
#include <QDebug>
#include <QFile>
#include <QString>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QMatrix4x4>
#include "transform.h"
#include <QtWidgets>

Viewer::Viewer(QWidget *parent) : QOpenGLWidget(parent)
{
    m_mesh = NULL;
    m_floor = NULL;
    m_timer = new QTimer(this);
    m_timer->setInterval(30);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
    m_timer->start();

    m_projection.setToIdentity();
    this->setFocusPolicy(Qt::StrongFocus);

    m_color = 0xffffffff;
    m_perspectiveFar = 3000;
    m_perspectiveNear = 1;
    m_staticLight = new lightProperties;
    m_staticLight->pos = QVector4D(0.0f,-1.0f,-1.0f,0.0f);
    m_staticLight->isEnable = false;
    m_staticLight->ambient = 0xffffffff;
    m_staticLight->diffuse = 0xffffffff;
    m_staticLight->specular = 0xffffffff;
    m_staticLight->rotateSpeed = 0;

    m_dynamicLight = new lightProperties;
    m_dynamicLight->pos = QVector4D(0.0f,0.0f,0.0f,1.0f);
    m_dynamicLight->isEnable = false;
    m_dynamicLight->ambient = 0xffffffff;
    m_dynamicLight->diffuse = 0xffffffff;
    m_dynamicLight->specular = 0xffffffff;
    m_dynamicLight->rotateSpeed = 0;
    m_dynamicLight->rotateAxis = QVector3D(1,0,0);

    m_shaderMode = 0;
}

Viewer::~Viewer()
{
    delete m_staticLight;
    delete m_dynamicLight;
}

void Viewer::recFile(const QString &name)
{
    QFile file(name);
    if(m_mesh!=NULL)
    {
        delete m_mesh;
    }
    if(m_floor!=NULL)
    {
        delete m_floor;
    }
    m_mesh = new ObjMesh();
    new ObjParser(m_mesh,&file);
    m_objCentralPoint = m_mesh->getCentralPoint();
    m_objSize = m_mesh->getObjSize();
    qDebug()<<"Object size :"<<m_objSize;
    qDebug()<<"Object central pos:"<<m_objCentralPoint;
    m_transform.setToIdentity();
    m_lightTransform.setToIdentity();
    //calculate the distance between camera and object
    //object's origin point is the zero point of world
    //assume we have 60 degree verticalAngle
    //the object will take half of the screen
    float h;
    h = (m_objSize.x() > m_objSize.y()) ? m_objSize.x() :m_objSize.y();
    m_cameraZ = 0-h*1.732;
    qDebug()<<"camera z:"<<m_cameraZ;

    //3D Transform
    m_projection.setToIdentity();
    m_projection.perspective(60.0f, width() / float(height()), 1.0f, 3000.0f);

    m_camera.setToIdentity();
    m_camera.setTranslation(0,0,m_cameraZ);
    m_lightTransform.setTranslation(0,0,-m_cameraZ);
    m_dynamicLight->pos = QVector4D(0.0,0.0,0.0,1.0);

    //m_camera.setToIdentity();
    //m_camera.translate(0,0,m_cameraZ);
    //m_camera.lookAt(QVector3D(0,0,-m_cameraZ),QVector3D(0,0,0),QVector3D(0,1,0));
    m_transform.setToIdentity();
    m_transform.translate(-m_objCentralPoint);
    m_trackball.init();
    m_trackballCamera.init();
    //draw floor
    m_floor = new Floor(h);
    m_floor->create();
    m_floorTexture->bind(0);
    m_floorProgram->bind();
    m_floorProgram->setUniformValue("ourTexture", 0);
    m_floorProgram->setUniformValue(u_worldToCameraFloor, m_camera.getMatrix());
    m_floorProgram->setUniformValue(u_cameraToViewFloor, m_projection);

    if(m_floor != NULL)
    {
        m_floor->draw();
        qDebug()<<"Dawring floor";
    }
    m_floorTexture->release();
    m_floorProgram->release();


    //update data of GPU
    m_program->bind();
    m_program->setUniformValue(u_worldToCamera, m_camera.getMatrix());
    m_program->setUniformValue(u_cameraToView, m_projection);
    m_program->setUniformValue(u_modelToWorld, m_transform.getMatrix());
    m_program->setUniformValue(u_normalMatrix, (m_camera.getMatrix()*m_transform.getMatrix()).inverted().transposed());
    if(m_mesh != NULL)
    {
        m_mesh->setDrawMode(GL_TRIANGLES);
        m_mesh->draw();
        qDebug()<<"Dawring Object";
    }
    m_program->release();
}

void Viewer::setColorParameter(QRgb color)
{
    m_color = color;
    m_program->bind();
    m_program->setUniformValue("basicColor", QColor(m_color));
    m_program->release();
}

void Viewer::setRenderMode(int mode)
{
    if(m_mesh!=NULL)
    {
        switch(mode)
        {
        case 0:
            m_mesh->setDrawMode(GL_TRIANGLES);
            m_mesh->draw();
            break;
        case 1:
            m_mesh->setDrawMode(GL_LINE_STRIP_ADJACENCY);
            m_mesh->draw();
            break;
        case 2:
            m_mesh->setDrawMode(GL_POINTS);
            m_mesh->draw();
            break;
        default:
            break;
        }
    }
}

void Viewer::setShaderMode(int mode)
{
    m_shaderMode= mode;

}

void Viewer::setFaceOritation(int mode)
{
    if(m_mesh!=NULL)
    {
        switch(mode)
        {
        case 0:
            qDebug() << 0;
            m_mesh->setOritation(GL_CCW);
            m_mesh->draw();
            break;
        case 1:
            qDebug()<< 1;
            m_mesh->setOritation(GL_CW);
            m_mesh->draw();
            break;
        default:
            break;
        }
    }
}

void Viewer::setPerspectiveFar(int value)
{
    m_perspectiveFar = value;
    m_projection.setToIdentity();
    m_projection.perspective(60.0f, width() / float(height()), m_perspectiveNear, m_perspectiveFar);
}

void Viewer::setPerspectiveNear(int value)
{
    m_perspectiveNear = value;
    m_projection.setToIdentity();
    m_projection.perspective(60.0f, width() / float(height()), m_perspectiveNear, m_perspectiveFar);
}

void Viewer::resetObjectPos()
{
    m_projection.setToIdentity();
    m_projection.perspective(60.0f, width() / float(height()), 1.0f, 3000.0f);

    m_camera.setToIdentity();
    m_camera.setTranslation(0,0,m_cameraZ);

    m_transform.setToIdentity();
    m_transform.translate(-m_objCentralPoint);

    m_trackball.init();
    m_trackballCamera.init();

}

void Viewer::setStaticLightAmbient(QRgb color)
{
    m_staticLight->ambient = color;
}

void Viewer::setStaticLightDiffuse(QRgb color)
{
    m_staticLight->diffuse = color;
}

void Viewer::setStaticLightSpecular(QRgb color)
{
    m_staticLight->specular = color;
}
void Viewer::setStaticLightState(int state)
{
    if(state == Qt::Checked)
        m_staticLight->isEnable = true;
    else
        m_staticLight->isEnable = false;
}

void Viewer::setDynamicLightAmbient(QRgb color)
{
    m_dynamicLight->ambient = color;
}

void Viewer::setDynamicLightDiffuse(QRgb color)
{
    m_dynamicLight->diffuse = color;
}

void Viewer::setDynamicLightSpecular(QRgb color)
{
    m_dynamicLight->specular = color;
}
void Viewer::setDynamicLightState(int state)
{
    if(state == Qt::Checked)
        m_dynamicLight->isEnable = true;
    else
        m_dynamicLight->isEnable = false;
}
void Viewer::setDynamicLightRotateSpeed(int value)
{
    m_dynamicLight->rotateSpeed = value;
}

void Viewer::setDynamicLightRotateAxis(int value)
{
    m_lightTransform.setToIdentity();
    m_lightTransform.setTranslation(0,0,-m_cameraZ);
    m_dynamicLight->pos = QVector4D(0.0,0.0,0.0,1.0);
    switch (value) {
    case 0:
        m_dynamicLight->rotateAxis = QVector3D(1,0,0);
        break;
    case 1:
        m_dynamicLight->rotateAxis = QVector3D(0,1,0);
        break;
    case 2:
        m_dynamicLight->rotateAxis = QVector3D(0,0,1);
        break;
    default:
        break;
    }
}



void Viewer::initializeGL()
{
    // Initialize OpenGL Backend
    initializeOpenGLFunctions();


    //connect(context(), SIGNAL(aboutToBeDestroyed()), this, SLOT(teardownGL()), Qt::DirectConnection);

    // Set global information
    //glEnable(GL_CULL_FACE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    //floor texture
    m_floorTexture = new QOpenGLTexture(QImage(":/shader/floor.png"));
    m_floorTexture->setWrapMode(QOpenGLTexture::Repeat);
    m_floorTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    m_floorTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    //shader
    m_program = new QOpenGLShaderProgram();
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/phong.vert");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/phong.frag");

    //geometry
    //m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/simple.vert");
    //m_program->addShaderFromSourceFile(QOpenGLShader::Geometry, ":/shader/simple.geom");
    //m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/simple.frag");

    m_environmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
    m_environmentShader->compileSourceFile(":/shader/floor.frag");

    m_floorProgram = new QOpenGLShaderProgram();
    m_floorProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/floor.vert");
    m_floorProgram->addShader(m_environmentShader);
    m_floorProgram->link();
    m_floorProgram->bind();

    u_worldToCameraFloor = m_floorProgram->uniformLocation("worldToCamera");
    u_cameraToViewFloor  = m_floorProgram->uniformLocation("cameraToView");
    m_floorProgram->release();

    m_program->link();
    m_program->bind();

    u_modelToWorld = m_program->uniformLocation("modelToWorld");
    u_worldToCamera = m_program->uniformLocation("worldToCamera");
    u_cameraToView = m_program->uniformLocation("cameraToView");
    u_normalMatrix = m_program->uniformLocation("normalMatrix");

    m_program->release();

}

void Viewer::resizeGL(int width, int height)
{
    // do not handling width/height changes
    (void)width;
    (void)height;
}

void Viewer::paintGL()
{
    // Clear
    //glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //render using shader

    m_transform.setRotation(m_trackball.rotation());

    m_lightTransform.rotate(m_dynamicLight->rotateSpeed,m_dynamicLight->rotateAxis);
    //m_camera.setRotation(m_trackballCamera.rotation());
    //m_camera.rotate(m_trackball.rotation().conjugate());
    //m_camera.rotate(1, Camera::LocalUp);
    //draw floor
    m_floorTexture->bind(0);
    m_floorProgram->bind();
    m_floorProgram->setUniformValue("ourTexture", 0);
    m_floorProgram->setUniformValue(u_worldToCameraFloor, m_camera.getMatrix());
    m_floorProgram->setUniformValue(u_cameraToViewFloor, m_projection);

    if(m_floor != NULL)
    {
        m_floor->draw();
    }
    m_floorTexture->release();
    m_floorProgram->release();

    //draw object
    m_program->bind();
    m_program->setUniformValue("basicColor", QColor(m_color));
    m_program->setUniformValue("shadermode", m_shaderMode);
    m_program->setUniformValue(u_worldToCamera, m_camera.getMatrix());
    m_program->setUniformValue(u_cameraToView, m_projection);
    m_program->setUniformValue(u_modelToWorld, m_transform.getMatrix());
    m_program->setUniformValue(u_normalMatrix, (m_camera.getMatrix()*m_transform.getMatrix()).inverted().transposed());
    m_program->setUniformValue("light.pos",m_staticLight->pos);
    m_program->setUniformValue("light.isEnable",m_staticLight->isEnable);
    m_program->setUniformValue("light.ambient",QColor(m_staticLight->ambient));
    m_program->setUniformValue("light.diffuse",QColor(m_staticLight->diffuse));
    m_program->setUniformValue("light.specular",QColor(m_staticLight->specular));

    m_program->setUniformValue("dynamiclight.pos",m_lightTransform.getMatrix()*m_dynamicLight->pos);
    //qDebug()<<m_lightTransform.getMatrix()*m_dynamicLight->pos;
    m_program->setUniformValue("dynamiclight.isEnable",m_dynamicLight->isEnable);
    m_program->setUniformValue("dynamiclight.ambient",QColor(m_dynamicLight->ambient));
    //qDebug()<<m_dynamicLight->ambient;
    m_program->setUniformValue("dynamiclight.diffuse",QColor(m_dynamicLight->diffuse));
    m_program->setUniformValue("dynamiclight.specular",QColor(m_dynamicLight->specular));

    if(m_mesh != NULL)
    {
        m_mesh->draw();
    }
    m_program->release();

}

void Viewer::teardownGL()
{
    m_object.destroy();
    m_vertex.destroy();
    delete m_program;
}

//the zero point of widget is left-up corner
//the view's zero point is in the center of the widget and from -1~1
QPointF Viewer::PosToViewPos(const QPoint& p)
{
    return QPointF(2.0 * float(p.x()) / width() - 1.0,
                   1.0 - 2.0 * float(p.y()) / height());
}


void Viewer::mousePressEvent(QMouseEvent *event)
{
    qDebug()<< event->button();
    if(event->button() & Qt::LeftButton)
    {
        m_trackball.push(PosToViewPos(event->pos()));
    }
    //camera
    if(event->button() & Qt::RightButton)
    {
        m_trackballCamera.push(PosToViewPos(event->pos()));
    }
}


void Viewer::mouseReleaseEvent(QMouseEvent *event)
{

    if(event->buttons() & Qt::LeftButton)
    {
        m_trackball.release(PosToViewPos(event->pos()),QQuaternion());
    }

    if(event->buttons() & Qt::RightButton)
    {
        m_trackballCamera.release(PosToViewPos(event->pos()),QQuaternion());
    }
}

void Viewer::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        m_trackball.move(PosToViewPos(event->pos()),QQuaternion());
    }
}

void Viewer::keyPressEvent(QKeyEvent *event)
{
    float  moveSpeed;
    moveSpeed = m_cameraZ/10;
    QVector3D translation;
    QVector3D lightTranslation;
    switch(event->key())
    {
    case Qt::Key_W:
        translation += m_camera.forward();
        lightTranslation += m_camera.forward();
        break;
    case Qt::Key_S:
        translation -= m_camera.forward();
        lightTranslation -= m_camera.forward();
        break;
    case Qt::Key_A:
        translation -= m_camera.right();
        lightTranslation += m_camera.right();
        break;
    case Qt::Key_D:
        translation += m_camera.right();
        lightTranslation -= m_camera.right();
        break;
    case Qt::Key_Q:
        translation += m_camera.up();
        lightTranslation -= m_camera.up();
        break;
    case Qt::Key_E:
        translation -= m_camera.up();
        lightTranslation += m_camera.up();
        break;
    case Qt::Key_Up:
        m_camera.rotate(5.0f, m_camera.right());
        break;
    case Qt::Key_Down:
        m_camera.rotate(-5.0f, m_camera.right());
        break;
    case Qt::Key_Right:
        m_camera.rotate(-5.0f, m_camera.up());
        break;
    case Qt::Key_Left:
        m_camera.rotate(5.0f,m_camera.up());
        break;
    case Qt::Key_Comma:
        m_camera.rotate(-5.0f, m_camera.forward());
        break;
    case Qt::Key_Period:
        m_camera.rotate(5.0f,m_camera.forward());
        break;
    default:
        break;
    }
    m_lightTransform.translate(moveSpeed*lightTranslation);
    m_camera.translate(moveSpeed*translation);
    //m_camera.translate(translation);
}
