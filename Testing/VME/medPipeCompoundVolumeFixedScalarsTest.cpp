/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medPipeCompoundVolumeFixedScalarsTest.cpp,v $
Language:  C++
Date:      $Date: 2010-04-26 14:34:19 $
Version:   $Revision: 1.1.2.1 $
Authors:   Eleonora Mambrini
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
#include "medPipeCompoundVolumeFixedScalarsTest.h"
#include "mafSceneNode.h"
#include "medGUIDynamicVP.h"

#include "mafVMEVolumeGray.h"


#include <iostream>

class DummyPipeCompoundVolumeFixedScalars : public medPipeCompoundVolumeFixedScalars
{
public:
  mafTypeMacro(DummyPipeCompoundVolumeFixedScalars,medPipeCompoundVolumeFixedScalars);
  DummyPipeCompoundVolumeFixedScalars(){}
  void Create(mafSceneNode *n);
protected:
  // redefinition for avoiding gui components
  mafGUI *CreateGui() {return NULL;}
  void    CreatePageGroups(){}
};

//----------------------------------------------------------------------------
void DummyPipeCompoundVolumeFixedScalars::Create(mafSceneNode *n)
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
mafCxxTypeMacro(DummyPipeCompoundVolumeFixedScalars);
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void medPipeCompoundVolumeFixedScalarsTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void medPipeCompoundVolumeFixedScalarsTest::setUp()
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
void medPipeCompoundVolumeFixedScalarsTest::tearDown()
//----------------------------------------------------------------------------
{

}

//----------------------------------------------------------------------------
void medPipeCompoundVolumeFixedScalarsTest::TestAllocation()
//----------------------------------------------------------------------------
{
  DummyPipeCompoundVolumeFixedScalars *pipe = new DummyPipeCompoundVolumeFixedScalars;
  mafDEL(pipe);
}
//----------------------------------------------------------------------------
void medPipeCompoundVolumeFixedScalarsTest::TestCreateSceneNode()
//----------------------------------------------------------------------------
{
  mafVMEVolumeGray *volume;
  mafNEW(volume);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,volume, NULL);

  /////////// Pipe Instance and Creation ///////////
  DummyPipeCompoundVolumeFixedScalars *pipe = new DummyPipeCompoundVolumeFixedScalars;
  pipe->Create(sceneNode);

  delete pipe;
  delete sceneNode;
  mafDEL(volume);

  delete wxLog::SetActiveTarget(NULL);
}

//----------------------------------------------------------------------------
void medPipeCompoundVolumeFixedScalarsTest::TestGetCurrentScalarVisualPipe()
//----------------------------------------------------------------------------
{

  mafVMEVolumeGray *volume;
  mafNEW(volume);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,volume, NULL);

  /////////// Pipe Instance and Creation ///////////
  DummyPipeCompoundVolumeFixedScalars *pipe = new DummyPipeCompoundVolumeFixedScalars;
  pipe->Create(sceneNode);

  pipe->GetCurrentScalarVisualPipe();

  delete pipe;
  delete sceneNode;
  mafDEL(volume);

}

