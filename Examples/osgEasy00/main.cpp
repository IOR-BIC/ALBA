// ===================================================
// Esempi OSG : Esempio 1
//
// carica uno scenegraph da un file
// 
// settare la variabile di ambiente OSG_FILE_PATH 
// in modo che punti alla directory con i dati di OSG
//
// ===================================================
#include "common.h"
#include <string>
#include <stdlib.h> // per getenv
#include <osgDB/ReadFile>


//----------------------------------------------------
int main( int argc, char **argv )
//----------------------------------------------------
{
   InitViewer();
   
   std::string filename = getenv("OSG_FILE_PATH");
   
   filename += "\\cow.osg";

   osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile(filename);

   if( !loadedModel.get() ) exit(1);

   SetSceneGraph( loadedModel.get() );

   Loop();
}
