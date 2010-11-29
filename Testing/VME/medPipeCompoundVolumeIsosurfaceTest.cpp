/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medPipeCompoundVolumeIsosurfaceTest.cpp,v $
Language:  C++
Date:      $Date: 2010-11-29 15:34:30 $
Version:   $Revision: 1.1.2.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2008
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "medPipeCompoundVolumeFixedScalars.h"
#include "medPipeCompoundVolumeIsosurfaceTest.h"
#include "mafSceneNode.h"
#include "medGUIDynamicVP.h"

#include "mafVMEVolumeGray.h"


#include <iostream>

class DummyPipeCompoundVolumeDRR : public medPipeCompoundVolumeIsosurface
{
public:
  mafTypeMacro(DummyPipeCompoundVolumeDRR,medPipeCompoundVolumeIsosurface);
  DummyPipeCompoundVolumeDRR(){}
  void Create(mafSceneNode *n);
protected:
  // redefinition for avoiding gui components
  mafGUI *CreateGui() {return NULL;}
  void    UpdateGUILayout(bool bForce){}
  void    CreatePageGroups(){}
};

//----------------------------------------------------------------------------
void DummyPipeCompoundVolumeDRR::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  //remove wxCursor busy;

  mafPipe::Create(n);
  m_SceneNode = n; 

  //creates description of groups
  CreatePageGroups();

  //creates initial pages for each group
  CreatePages();

  //AddActor();
}

//----------------------------------------------------------------------------
mafCxxTypeMacro(DummyPipeCompoundVolumeDRR);
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void medPipeCompoundVolumeIsosurfaceTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void medPipeCompoundVolumeIsosurfaceTest::setUp()
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
void medPipeCompoundVolumeIsosurfaceTest::tearDown()
//----------------------------------------------------------------------------
{

}

//----------------------------------------------------------------------------
void medPipeCompoundVolumeIsosurfaceTest::TestAllocation()
//----------------------------------------------------------------------------
{
  DummyPipeCompoundVolumeDRR *pipe = new DummyPipeCompoundVolumeDRR;
  mafDEL(pipe);
}
//----------------------------------------------------------------------------
void medPipeCompoundVolumeIsosurfaceTest::TestCreateSceneNode()
//----------------------------------------------------------------------------
{
  mafVMEVolumeGray *volume;
  mafNEW(volume);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,volume, NULL);

  /////////// Pipe Instance and Creation ///////////
  DummyPipeCompoundVolumeDRR *pipe = new DummyPipeCompoundVolumeDRR;
  pipe->Create(sceneNode);

  delete pipe;
  delete sceneNode;
  mafDEL(volume);

  delete wxLog::SetActiveTarget(NULL);
}

//----------------------------------------------------------------------------
void medPipeCompoundVolumeIsosurfaceTest::TestGetCurrentScalarVisualPipe()
//----------------------------------------------------------------------------
{
  mafVMEVolumeGray *volume;
  mafNEW(volume);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,volume, NULL);

  /////////// Pipe Instance and Creation ///////////
  DummyPipeCompoundVolumeDRR *pipe = new DummyPipeCompoundVolumeDRR;
  pipe->Create(sceneNode);

  //smoke test
  CPPUNIT_ASSERT(pipe->GetCurrentScalarVisualPipe() == NULL);

  delete pipe;
  delete sceneNode;
  mafDEL(volume);

}