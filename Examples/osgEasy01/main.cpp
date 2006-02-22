// ===================================================
// Esempi OSG : Esempio 2
//
// carica un terreno
//  --- lo trovi in pc-imboden/sharedarea/fanini/grandCanyon
//
// istanzia un manipolatore di esempio
//
// il manipolatore mostra anche un esempio d'uso dell'intersect visitor
//
//===================================================
#include "common.h"
#include "sampleManipulator.h"
#include <string>
#include <stdlib.h> // per getenv
#include <osgDB/ReadFile>

//----------------------------------------------------
int main( int argc, char **argv )
//----------------------------------------------------
{
   InitViewer();
   
   std::string filename = getenv("OSG_FILE_PATH");
   filename += "\\GrandCanyon\\grancanyon.ive";
   osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile(filename);
   if( !loadedModel.get() ) exit(1);
   SetSceneGraph( loadedModel.get() );

   osg::ref_ptr<sampleManipulator> manip = new sampleManipulator();
   unsigned int pos = viewer.addCameraManipulator(manip.get());
   viewer.selectCameraManipulator(pos);
   std::cout << "Sample manipulator added as " << pos << std::endl; 

   Loop();
}

