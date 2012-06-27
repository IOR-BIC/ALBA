/*=========================================================================

 Program: MAF2
 Module: mafPipeFactoryVMETest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafPipeFactoryVMETest.h"

#include "mafPipe.h"
#include "mafPipeFactoryVME.h"

#include "mafVersion.h"
#include <iostream>

//-------------------------------------------------------------------------
void mafPipeFactoryVMETest::TestGetInstance_Initialize()
//-------------------------------------------------------------------------
{
  // a couple of factories
  mafPipeFactoryVME *pipe_factory = mafPipeFactoryVME::GetInstance();
  CPPUNIT_ASSERT(pipe_factory!=NULL);

  // this custom factory needs only to be initialized
  CPPUNIT_ASSERT(mafPipeFactoryVME::Initialize()==MAF_OK);

  std::list<mafObjectFactory *> list=mafObjectFactory::GetRegisteredFactories();
  CPPUNIT_ASSERT(list.size()==1);

 
  const std::vector<std::string> pipes = pipe_factory->GetPipeNames();
  int s = pipes.size();
  
  enum ID_NUMBER_OF_VME_PIPE
  {
    ID_TOTAL_NUMBER = 21,
  };

  CPPUNIT_ASSERT(s == ID_TOTAL_NUMBER); 

  char *pipeName[ID_TOTAL_NUMBER];
  pipeName[0] = "mafPipeSurface";
  pipeName[1] = "mafPipeSurfaceSlice";
  pipeName[2] = "mafPipeSurfaceTextured";
  pipeName[3] = "mafPipePolylineSlice";
  pipeName[4] = "mafPipeGizmo";
  pipeName[5] = "mafPipeMeter";
  pipeName[6] = "mafPipeVolumeSlice";
  pipeName[7] = "mafPipeVolumeProjected";
  pipeName[8] = "mafPipePointSet";
  pipeName[9] = "mafPipePolyline";
  pipeName[10] = "mafPipeVector";
  pipeName[11] = "mafPipeLandmarkCloud";
  pipeName[12] = "mafPipeIsosurface";
  pipeName[13] = "mafPipeIsosurfaceGPU";
  pipeName[14] = "mafPipeImage3D";
  pipeName[15] = "mafPipeMesh";
  pipeName[16] = "mafPipeMeshSlice";
  pipeName[17] = "mafPipeScalar";
  pipeName[18] = "mafVisualPipeVolumeRayCasting";
  pipeName[19] = "mafPipeScalarMatrix";
  pipeName[20] = "mafPipeBox"; //default pipe plugged by superclass

  bool found;
  for (int i=0;i<pipes.size();i++)
  {
		std::string controlPipeName;
		controlPipeName = pipes[i];
    found = false;
    for(int j=0; j<ID_TOTAL_NUMBER; j++)
    {
      if (controlPipeName==pipeName[j])
      {
        found = true;
      }
    }

    if(found == false)
    {
      break;
    }
    
  }
  
	CPPUNIT_ASSERT(found);
}
//-------------------------------------------------------------------------
void mafPipeFactoryVMETest::TestGetMAFSourceVersion()
//-------------------------------------------------------------------------
{
	mafPipeFactoryVME *pipe_factory = mafPipeFactoryVME::GetInstance();
	CPPUNIT_ASSERT(pipe_factory!=NULL);
	std::string value1 = pipe_factory->GetMAFSourceVersion();
	std::string value2 = std::string(MAF_SOURCE_VERSION);
	CPPUNIT_ASSERT( value1 == value2 );

}
//-------------------------------------------------------------------------
void mafPipeFactoryVMETest::TestGetDescription()
//-------------------------------------------------------------------------
{
	mafPipeFactoryVME *pipe_factory = mafPipeFactoryVME::GetInstance();
	CPPUNIT_ASSERT(pipe_factory!=NULL);
	std::string value1 = pipe_factory->GetDescription();
	std::string value2 = std::string("Factory for MAF Pipes of VME library");
	CPPUNIT_ASSERT( value1 == value2 );
}
  
