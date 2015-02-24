#include "objparser.h"
#include "objmesh.h"
#include <QVector3D>
#include <QDebug>
#include <QString>
#include <QMap>
#include <QTextStream>
#include <QFile>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>

enum ParseToken
{
  PT_ERROR,
  PT_EOF,
  PT_OBJECTNAME,
  PT_VERTEX0,
  PT_VERTEX1,
  PT_VERTEX2,
  PT_FACENORMAL,
  PT_COMMENT1,
  PT_COMMENT2
};

class ParseMap : public QMap<QString, ParseToken>
{
public:
  ParseMap()
  {
    insert("v0",PT_VERTEX0);
    insert("v1",PT_VERTEX1);
    insert("v2",PT_VERTEX2);
    insert("face",PT_FACENORMAL);
    insert("Object",PT_OBJECTNAME);
    insert("#",PT_COMMENT1);
    insert("--",PT_COMMENT2);
  }
};

static ParseMap const sg_reserved;

//ObjParser Private
class ObjParserPrivate
{
public:
    struct Vertex
    {
        QVector3D pos;
        QVector3D normal;
        int color;
    };
    struct Face
    {

    };


    typedef QVector<Face> FaceList;


    ObjParserPrivate(QFile *file);
    bool parse();
    void createObjMesh(ObjMesh* mesh);

private:
    typedef QVector<Vertex> VertexList;
    VertexList m_vertex0List;
    VertexList m_vertex1List;
    VertexList m_vertex2List;
    QVector<QVector3D> m_faceNormalList;

    QFile *m_file;
    QString m_currLine;
    QString m_lexicon;
    QString m_name;

    int m_faceCount;
    int m_vertexCount;
    int m_materialCount;
    int m_currLineCount;
    ParseToken lexToken();

    void parseName();
    Vertex parseSingleVertex();
    void parseVertex(int i);
    void parseFaceNormal();
};

ObjParserPrivate::ObjParserPrivate(QFile *file) :
    m_file(file), m_faceCount(0), m_vertexCount(0),m_materialCount(0), m_currLineCount(0)
{
    file->open(QFile::ReadOnly | QFile::Text);
}

ParseToken ObjParserPrivate::lexToken()
{
    ParseMap::const_iterator it = sg_reserved.find(m_lexicon);

    return it.value();
}


bool ObjParserPrivate::parse()
{
    QTextStream in(m_file);
    while (!in.atEnd())
    {
       m_currLine.clear();
       m_currLine = in.readLine();
       //read the first word of line to m_lexicon
       m_lexicon.clear();
       m_lexicon = m_currLine.section(" ", 0,0);

       switch(lexToken())
       {
       case PT_COMMENT1:
       case PT_COMMENT2:
           break;
       case PT_OBJECTNAME:
           parseName();
           break;
       case PT_VERTEX0:
           parseVertex(0);
           break;
       case PT_VERTEX1:
           parseVertex(1);
           break;
       case PT_VERTEX2:
           parseVertex(2);
           break;
       case PT_FACENORMAL:
           parseFaceNormal();
           break;
       default:
           break;
       }
    }
    qDebug() << m_vertexCount;
    //qDebug() << m_faceNormalList;
    return true;
}


void ObjParserPrivate::parseName()
{
    m_name = m_currLine.section(" ", -1,-1);
    qDebug() << "The name of the object is "<< m_name;
}
//3*[pos(x,y,z) normal(x,y,z) color_index] face_normal(x,y,z)
ObjParserPrivate::Vertex ObjParserPrivate::parseSingleVertex()
{
    Vertex singleVertex;
    QStringList strList = m_currLine.split(' ',QString::SkipEmptyParts);
    singleVertex.pos = QVector3D(strList[1].toFloat(),strList[2].toFloat(),strList[3].toFloat());
    singleVertex.normal= QVector3D(strList[4].toFloat(),strList[5].toFloat(),strList[6].toFloat());
    singleVertex.color = strList[7].toInt();
    return singleVertex;
}


void ObjParserPrivate::parseVertex(int i)
{
    switch(i)
    {
    case 0:
        m_vertex0List.push_back(parseSingleVertex());
        m_vertexCount++;
        break;
    case 1:
        m_vertex1List.push_back(parseSingleVertex());
        m_vertexCount++;
        break;
    case 2:
        m_vertex2List.push_back(parseSingleVertex());
        m_vertexCount++;
        break;
    default:
        break;
    }
}

void ObjParserPrivate::parseFaceNormal()
{
    QStringList strList = m_currLine.split(' ',QString::SkipEmptyParts);
    m_faceNormalList.push_back(QVector3D(strList[2].toFloat(),strList[3].toFloat(),strList[4].toFloat()));
}

void ObjParserPrivate::createObjMesh(ObjMesh *mesh)
{
    mesh->createBuffer(QOpenGLBuffer::StaticDraw);
    mesh->createVertexArrayObjct();
    QOpenGLBuffer *buffer = mesh->getBuffer();
    QOpenGLVertexArrayObject *vao = mesh->getVertexArrayObject();
    QOpenGLFunctions f = QOpenGLFunctions(QOpenGLContext::currentContext());

    buffer->bind();
    buffer->allocate(sizeof(Vertex) * m_vertex0List.size() * 3);

    Vertex *data = (Vertex *)buffer->map(QOpenGLBuffer::WriteOnly);

    //write to buffer

    vao->bind();
    for(size_t i=0;i < (size_t)m_vertex0List.size();i++)
    {
        data[3*i] = m_vertex0List[i];
        qDebug() << data[3*i].pos;
        qDebug() << data[3*i].normal;
        data[3*i+1] = m_vertex1List[i];
        qDebug() << data[3*i+1].pos;
        qDebug() << data[3*i+1].normal;
        data[3*i+2] = m_vertex2List[i];
        qDebug() << data[3*i+2].pos;
        qDebug() << data[3*i+2].normal;
    }
    buffer->unmap();


    // Bind attributes
    f.glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,pos));
    f.glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,normal));
    f.glEnableVertexAttribArray(0);
    f.glEnableVertexAttribArray(1);

    // Initialize mesh
    vao->release();
    buffer->release();
    mesh->setDrawArrays(GL_LINES, m_vertex0List.size()*3);

}

ObjParser::ObjParser(ObjMesh *mesh, QFile *file)
{
    m_private = new ObjParserPrivate(file);
    m_private->parse();
    m_private->createObjMesh(mesh);
}

ObjParser::

ObjParser::~ObjParser()
{

}

