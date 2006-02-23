// ===================================================
// Esempi OSG : Esempio 3
// 
// Creazione di Oggetti
// 
//===================================================
#include "common.h"
#include <string>
#include <stdlib.h> // per getenv
#include <osgDB/ReadFile>

extern osg::Geode* createTeapot();
extern osg::Group* createShape();
extern osg::Group *createQuad();

//----------------------------------------------------
int main( int argc, char **argv )
//----------------------------------------------------
{
   InitViewer();
   
   // scommenta uno dei seguenti
   
   SetSceneGraph( createShape() );
   //SetSceneGraph( createQuad() );
   //SetSceneGraph( createTeapot() );

   Loop();
}

