#include "objparser.h"
#include "objmesh.h"
#include <QVector3D>
#include <QVector2D>
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
  PT_COMMENT2,
  PT_MATERIAL_AMBIENT,
  PT_MATERIAL_DIFFUSE,
  PT_MATERIAL_SPECULAR,
  PT_MATERIAL_SHINE
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
    insert("ambient",PT_MATERIAL_AMBIENT);
    insert("diffuse",PT_MATERIAL_DIFFUSE);
    insert("specular",PT_MATERIAL_SPECULAR);
    insert("material",PT_MATERIAL_SHINE);
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
        QVector3D ambient;
        QVector3D diffuse;
        QVector3D specular;
        float shininess;
        QVector3D faceNormal;
        QVector2D textureCoor;
        QVector3D tangent;
        QVector3D bitangent;
    };
    struct Face
    {

    };
    typedef QVector<Face> FaceList;

    ObjParserPrivate(QFile *file);
    bool parse();
    void createObjMesh(ObjMesh* mesh);
    QVector3D calculateMeshCentralPoint();
    QVector3D calculateMeshSize();

private:
    typedef QVector<Vertex> VertexList;
    VertexList m_vertex0List;
    VertexList m_vertex1List;
    VertexList m_vertex2List;

    QVector<QVector3D> m_faceNormalList;
    QVector<QVector3D> m_materialAmient;
    QVector<QVector3D> m_materialDiffuse;
    QVector<QVector3D> m_materialSpecular;
    QVector<float> m_materialShine;

    QVector3D m_minXYZ;
    QVector3D m_maxXYZ;
    QFile *m_file;
    QString m_currLine;
    QString m_lexicon;
    QString m_name;

    bool m_texture_flag;

    int m_faceCount;
    int m_vertexCount;
    int m_materialCount;
    int m_currLineCount;
    ParseToken lexToken();

    void parseName();
    Vertex parseSingleVertex();
    void parseVertex(int i);
    void parseFaceNormal();
    void parseMaterial(ParseToken material);

    void computeTangent(int index);
};

ObjParserPrivate::ObjParserPrivate(QFile *file) :
    m_file(file), m_faceCount(0), m_vertexCount(0),m_materialCount(0), m_currLineCount(0),m_texture_flag(false)
{
    //just a init number can be compareed
    m_maxXYZ = QVector3D(-1000,-1000,-1000);
    m_minXYZ = QVector3D(1000,1000,1000);
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
       case PT_MATERIAL_AMBIENT:
           parseMaterial(PT_MATERIAL_AMBIENT);
           break;
       case PT_MATERIAL_DIFFUSE:
           parseMaterial(PT_MATERIAL_DIFFUSE);
           break;
       case PT_MATERIAL_SPECULAR:
           parseMaterial(PT_MATERIAL_SPECULAR);
           break;
       case PT_MATERIAL_SHINE:
           parseMaterial(PT_MATERIAL_SHINE);
           break;
       default:
           break;
       }
    }
    qDebug() << m_vertexCount;
    //qDebug() << m_faceNormalList;
    qDebug() << m_materialAmient;
    qDebug() << m_materialDiffuse;
    qDebug() << m_materialShine;
    qDebug() << m_materialSpecular;
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
    int materialIndex;
    QStringList strList = m_currLine.split(' ',QString::SkipEmptyParts);

    singleVertex.pos = QVector3D(strList[1].toFloat(),strList[2].toFloat(),strList[3].toFloat());
    singleVertex.normal= QVector3D(strList[4].toFloat(),strList[5].toFloat(),strList[6].toFloat());

    materialIndex = strList[7].toInt();
    singleVertex.ambient = m_materialAmient[materialIndex];
    singleVertex.diffuse = m_materialDiffuse[materialIndex];
    singleVertex.specular = m_materialSpecular[materialIndex];
    singleVertex.shininess = m_materialShine[materialIndex];

    //if this is object have a texture coordinate
    //3*[pos(x,y,z) normal(x,y,z) color_index text_coord]
    if(strList.size()>8)
    {
        singleVertex.textureCoor = QVector2D(strList[8].toFloat(),strList[9].toFloat());
        m_texture_flag = true;
    }
    else
        m_texture_flag = false;

    //find the max and min x y z of object
    if(singleVertex.pos.x() > m_maxXYZ.x())
        m_maxXYZ.setX(singleVertex.pos.x());
    if(singleVertex.pos.y() > m_maxXYZ.y())
        m_maxXYZ.setY(singleVertex.pos.y());
    if(singleVertex.pos.z() > m_maxXYZ.z())
        m_maxXYZ.setZ(singleVertex.pos.z());

    if(singleVertex.pos.x() < m_minXYZ.x())
        m_minXYZ.setX(singleVertex.pos.x());
    if(singleVertex.pos.y() < m_minXYZ.y())
        m_minXYZ.setY(singleVertex.pos.y());
    if(singleVertex.pos.z() < m_minXYZ.z())
        m_minXYZ.setZ(singleVertex.pos.z());

    //qDebug()<< m_maxXYZ;
    //qDebug()<< m_minXYZ;
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

void ObjParserPrivate::parseMaterial(ParseToken material)
{
    QStringList strList = m_currLine.split(' ',QString::SkipEmptyParts);
    switch(material)
    {
    case PT_MATERIAL_AMBIENT:
        m_materialAmient.push_back(QVector3D(strList[2].toFloat(),strList[3].toFloat(),strList[4].toFloat()));
        break;
    case PT_MATERIAL_DIFFUSE:
        m_materialDiffuse.push_back(QVector3D(strList[2].toFloat(),strList[3].toFloat(),strList[4].toFloat()));
        break;
    case PT_MATERIAL_SPECULAR:
        m_materialSpecular.push_back(QVector3D(strList[2].toFloat(),strList[3].toFloat(),strList[4].toFloat()));
        break;
    case PT_MATERIAL_SHINE:

        m_materialShine.push_back(strList[2].toFloat()==0.0?1:strList[2].toFloat()*128);
        break;
    default:
        break;
    }


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
        if(m_texture_flag)
            computeTangent(i);
        m_vertex0List[i].faceNormal = m_faceNormalList[i];
        m_vertex1List[i].faceNormal = m_faceNormalList[i];
        m_vertex2List[i].faceNormal = m_faceNormalList[i];
        data[3*i] = m_vertex0List[i];
        //qDebug() << data[3*i].pos;
        //qDebug() << data[3*i].normal;
        data[3*i+1] = m_vertex1List[i];
        //qDebug() << data[3*i+1].pos;
        //qDebug() << data[3*i+1].normal;
        data[3*i+2] = m_vertex2List[i];
        //qDebug() << data[3*i+2].pos;
        //qDebug() << data[3*i+2].normal;
    }
    buffer->unmap();


    // Bind attributes
    f.glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,pos));
    f.glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,normal));
    f.glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,ambient));
    f.glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,diffuse));
    f.glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,specular));
    f.glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,shininess));
    f.glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,faceNormal));
    f.glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,textureCoor));
    f.glVertexAttribPointer(8, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,tangent));
    f.glVertexAttribPointer(9, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,bitangent));
    f.glEnableVertexAttribArray(0);
    f.glEnableVertexAttribArray(1);
    f.glEnableVertexAttribArray(2);
    f.glEnableVertexAttribArray(3);
    f.glEnableVertexAttribArray(4);
    f.glEnableVertexAttribArray(5);
    f.glEnableVertexAttribArray(6);
    f.glEnableVertexAttribArray(7);
    f.glEnableVertexAttribArray(8);
    f.glEnableVertexAttribArray(9);
    // Initialize mesh
    vao->release();
    buffer->release();
    mesh->setDrawArrays(m_vertex0List.size()*3);
}

QVector3D ObjParserPrivate::calculateMeshCentralPoint()
{
    return (m_maxXYZ+m_minXYZ)/2;
}

QVector3D ObjParserPrivate::calculateMeshSize()
{
    QVector3D size;
    size.setX(abs(m_minXYZ.x())+abs(m_maxXYZ.x()));
    size.setY(abs(m_minXYZ.y())+abs(m_maxXYZ.y()));
    size.setZ(abs(m_minXYZ.z())+abs(m_maxXYZ.z()));
    return size;
}

ObjParser::ObjParser(ObjMesh *mesh, QFile *file)
{
    m_private = new ObjParserPrivate(file);
    m_private->parse();
    m_private->createObjMesh(mesh);

    mesh->setObjSize(m_private->calculateMeshSize());
    mesh->setCentralPoint(m_private->calculateMeshCentralPoint());
}

void ObjParserPrivate::computeTangent(int index)
{
    QVector3D v0 = m_vertex0List[index].pos;
    QVector3D v1 = m_vertex1List[index].pos;
    QVector3D v2 = m_vertex2List[index].pos;

    QVector2D uv0 = m_vertex0List[index].textureCoor;
    QVector2D uv1 = m_vertex1List[index].textureCoor;
    QVector2D uv2 = m_vertex2List[index].textureCoor;

    QVector3D deltaPos1 = v1-v0;
    QVector3D deltaPos2 = v2-v0;

    QVector2D deltaUV1 = uv1-uv0;
    QVector2D deltaUV2 = uv2-uv0;
    float r = 1.0f / (deltaUV1.x() * deltaUV2.y() - deltaUV1.y() * deltaUV2.x());
    m_vertex0List[index].tangent = (deltaPos1 * deltaUV2.y() - deltaPos2 * deltaUV1.y())*r;
    m_vertex0List[index].bitangent = (deltaPos2 * deltaUV1.x() - deltaPos1 * deltaUV2.x())*r;
    m_vertex1List[index].tangent = m_vertex0List[index].tangent;
    m_vertex1List[index].bitangent = m_vertex0List[index].bitangent;
    m_vertex2List[index].tangent = m_vertex0List[index].tangent;
    m_vertex2List[index].bitangent = m_vertex0List[index].bitangent;
}

ObjParser::~ObjParser()
{

}

