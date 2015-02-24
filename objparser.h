#ifndef OBJPARSER_H
#define OBJPARSER_H

#include "objmesh.h"
class QFile;
class QString;

class ObjParserPrivate;
class ObjParser
{
public:
    //create a paser which can parse object file to mesh
    ObjParser(ObjMesh *mesh, QFile *file);
    ~ObjParser();


private:
    ObjParserPrivate *m_private;
};

#endif // OBJPARSER_H
