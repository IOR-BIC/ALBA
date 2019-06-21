/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeCompoundVolumeFixedScalarsTest
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "albaPipeCompoundVolumeFixedScalars.h"
#include "albaPipeCompoundVolumeFixedScalarsTest.h"
#include "albaSceneNode.h"
#include "albaGUIDynamicVP.h"

#include "albaVMEVolumeGray.h"


#include <iostream>

class DummyPipeCompoundVolumeFixedScalars : public albaPipeCompoundVolumeFixedScalars
{
public:
  albaTypeMacro(DummyPipeCompoundVolumeFixedScalars,albaPipeCompoundVolumeFixedScalars);
  DummyPipeCompoundVolumeFixedScalars(){}
  void Create(albaSceneNode *n);
protected:
  // redefinition for avoiding gui components
  albaGUI *CreateGui() {return NULL;}
  void    UpdateGUILayout(bool bForce){}
  void    CreatePageGroups(){}
};

//----------------------------------------------------------------------------
void DummyPipeCompoundVolumeFixedScalars::Create(albaSceneNode *n)
//----------------------------------------------------------------------------
{
  //remove wxCursor busy;

  albaPipe::Create(n);
  m_SceneNode = n; 

  //creates description of groups
  CreatePageGroups();

  //creates initial pages for each group
  CreatePages();

  //AddActor();
}

//----------------------------------------------------------------------------
albaCxxTypeMacro(DummyPipeCompoundVolumeFixedScalars);
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void albaPipeCompoundVolumeFixedScalarsTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaPipeCompoundVolumeFixedScalarsTest::TestAllocation()
//----------------------------------------------------------------------------
{
  DummyPipeCompoundVolumeFixedScalars *pipe = new DummyPipeCompoundVolumeFixedScalars;
  albaDEL(pipe);
}
//----------------------------------------------------------------------------
void albaPipeCompoundVolumeFixedScalarsTest::TestCreateSceneNode()
//----------------------------------------------------------------------------
{
  albaVMEVolumeGray *volume;
  albaNEW(volume);

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,volume, NULL);

  /////////// Pipe Instance and Creation ///////////
  DummyPipeCompoundVolumeFixedScalars *pipe = new DummyPipeCompoundVolumeFixedScalars;
  pipe->Create(sceneNode);

  delete sceneNode;
  albaDEL(volume);
}

//----------------------------------------------------------------------------
void albaPipeCompoundVolumeFixedScalarsTest::TestGetCurrentScalarVisualPipe()
//----------------------------------------------------------------------------
{

  albaVMEVolumeGray *volume;
  albaNEW(volume);

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,volume, NULL);

  /////////// Pipe Instance and Creation ///////////
  DummyPipeCompoundVolumeFixedScalars *pipe = new DummyPipeCompoundVolumeFixedScalars;
  pipe->Create(sceneNode);

  pipe->GetCurrentScalarVisualPipe();

  delete sceneNode;
  albaDEL(volume);

}

