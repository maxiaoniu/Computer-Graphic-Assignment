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
#include <QRgb>
#include <cmath>

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
    m_staticLight->pos = QVector4D(0.0f,0.0f,-1.0f,0.0f);
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
    m_bumpSel = 0;

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
    m_directlightTransform.setToIdentity();
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
    m_trackballDLight.init();
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
    m_trackballDLight.init();

    //reset dynamic light pos
    m_lightTransform.setToIdentity();
    m_lightTransform.setTranslation(0,0,-m_cameraZ);
    m_dynamicLight->pos = QVector4D(0.0,0.0,0.0,1.0);

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

void Viewer::setBumpImage(int value)
{
    m_bumpSel = value;
}


void Viewer::setTextureState(int state)
{
    if(state == Qt::Checked)
         m_textureSel = 1;
    else
         m_textureSel = 0;
}
void Viewer::initializeGL()
{
    // Initialize OpenGL Backend
    initializeOpenGLFunctions();


    //connect(context(), SIGNAL(aboutToBeDestroyed()), this, SLOT(teardownGL()), Qt::DirectConnection);

    // Set global information
    glEnable(GL_CULL_FACE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    //floor texture
    m_floorTexture = new QOpenGLTexture(QImage(":/shader/floor.png"));
    m_floorTexture->setWrapMode(QOpenGLTexture::Repeat);
    m_floorTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    m_floorTexture->setMagnificationFilter(QOpenGLTexture::Linear);

    //cube texture
    QImage normal;
    normal = convertFromBumpToNormal( QImage(":/shader/orange2.bmp"));
    //m_cubeTexture = new QOpenGLTexture(QImage(":/shader/orange2_n.png"));
    m_cubeTexture = new QOpenGLTexture(normal);
    //m_floorTexture->setWrapMode(QOpenGLTexture::Repeat);
    m_cubeTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    m_cubeTexture->setMagnificationFilter(QOpenGLTexture::Linear);

    QImage normal1;
    normal1 = convertFromBumpToNormal( QImage(":/shader/bump_circle.bmp"));
    //m_cubeTexture = new QOpenGLTexture(QImage(":/shader/orange2_n.png"));
    m_cubeTexture1 = new QOpenGLTexture(normal1);
    //m_floorTexture->setWrapMode(QOpenGLTexture::Repeat);
    m_cubeTexture1->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    m_cubeTexture1->setMagnificationFilter(QOpenGLTexture::Linear);

    m_cubeWallTexture = new QOpenGLTexture(QImage(":/shader/texture_wall.bmp"));
    m_cubeWallTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    m_cubeWallTexture->setMagnificationFilter(QOpenGLTexture::Linear);

    m_cubeWallNormal = new QOpenGLTexture(QImage(":/shader/WallNormal.png"));
    m_cubeWallNormal->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    m_cubeWallNormal->setMagnificationFilter(QOpenGLTexture::Linear);

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
    m_directlightTransform.setRotation(m_trackballDLight.rotation());
    m_lightTransform.rotate(m_dynamicLight->rotateSpeed,m_dynamicLight->rotateAxis);
    //m_camera.setRotation(m_trackballCamera.rotation());
    //m_camera.rotate(m_trackball.rotation().conjugate());
    //m_camera.rotate(1, Camera::LocalUp);
    //draw floor
    //m_floorTexture->bind(0);
    //m_floorProgram->bind();
    //m_floorProgram->setUniformValue("ourTexture", 0);
    //m_floorProgram->setUniformValue(u_worldToCameraFloor, m_camera.getMatrix());
    //m_floorProgram->setUniformValue(u_cameraToViewFloor, m_projection);

    if(m_floor != NULL)
    {
        //m_floor->draw();
    }
    //m_floorTexture->release();
    //m_floorProgram->release();

    //draw object
    m_cubeTexture->bind(0);
    m_cubeTexture1->bind(1);
    m_cubeWallTexture->bind(2);
    m_cubeWallNormal->bind(3);
    m_program->bind();
    m_program->setUniformValue("orange", 0);
    m_program->setUniformValue("circle", 1);
    m_program->setUniformValue("wall", 2);
    m_program->setUniformValue("wallNormal", 3);
    m_program->setUniformValue("basicColor", QColor(m_color));
    m_program->setUniformValue("shadermode", m_shaderMode);
    m_program->setUniformValue("bumpSel", m_bumpSel);
    m_program->setUniformValue("textureSel", m_textureSel);
    m_program->setUniformValue(u_worldToCamera, m_camera.getMatrix());
    m_program->setUniformValue(u_cameraToView, m_projection);
    m_program->setUniformValue(u_modelToWorld, m_transform.getMatrix());
    m_program->setUniformValue(u_normalMatrix, (m_camera.getMatrix()*m_transform.getMatrix()).inverted().transposed());
    m_program->setUniformValue("light.pos",m_directlightTransform.getMatrix()*m_staticLight->pos);
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
    m_cubeTexture1->release();
    m_cubeTexture->release();
    m_cubeWallTexture->release();
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
        m_trackballDLight.push(PosToViewPos(event->pos()));
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
        m_trackballDLight.release(PosToViewPos(event->pos()),QQuaternion());
    }
}

void Viewer::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        m_trackball.move(PosToViewPos(event->pos()),QQuaternion());
    }
    if(event->buttons() & Qt::RightButton)
    {
        m_trackballDLight.move(PosToViewPos(event->pos()),QQuaternion());
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
int Viewer::clamp(int x, int min, int max)
{
    if(x < min)
        return min;
    if(x > max)
        return max;
    return x;
}

QImage Viewer::convertFromBumpToNormal(QImage img)
{
    int width = img.width();
    int height = img.height();

    QImage normalMap(width,height,QImage::Format_RGB32);
    float topLeft, top, topRight;
    float bottomLeft, bottom, bottomRight;
    float left, right;

    float dz = 0.8; //just a random value
    float length;
    for(int x=0; x<width; x++)
    {
        for(int y=0; y<height; y++)
        {
            topLeft = qRed(img.pixel(clamp(x-1,0,width-1),clamp(y-1,0,height-1)))/255.0;
            top = qRed(img.pixel(clamp(x,0,width-1),clamp(y-1,0,height-1)))/255.0;
            topRight = qRed(img.pixel(clamp(x+1,0,width-1),clamp(y-1,0,height-1)))/255.0;

            left = qRed(img.pixel(clamp(x-1,0,width-1),clamp(y,0,height-1)))/255.0;
            right = qRed(img.pixel(clamp(x+1,0,width-1),clamp(y,0,height-1)))/255.0;

            bottomLeft = qRed(img.pixel(clamp(x-1,0,width-1),clamp(y+1,0,height-1)))/255.0;
            bottom = qRed(img.pixel(clamp(x,0,width-1),clamp(y+1,0,height-1)))/255.0;
            bottomRight = qRed(img.pixel(clamp(x+1,0,width-1),clamp(y+1,0,height-1)))/255.0;

            float dx,dy;
            // use scharr operator 3 10 3
            dx = topLeft*3.0 + left*10.0 + bottomLeft*3.0 - topRight*3.0 - right*10.0 - bottomRight*3.0;
            dy = topLeft*3.0 + top*10.0 + topRight*3.0 - bottomLeft*3.0 - bottom*10.0 - bottomRight*3.0;


            length = std::sqrt(dx*dx+dy*dy+dz*dz);
            int r = (dx/length * 0.5 + 0.5) * 255.0; 	// red
            int g = (dy/length * 0.5 + 0.5) * 255.0; 	// green
            int b = dz/length * 255.0;
            normalMap.setPixel(x,y,qRgb(r,g,b));
        }
    }
    return normalMap;
}
