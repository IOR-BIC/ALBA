/*=========================================================================

 Program: MAF2Medical
 Module: medPipeCompoundTest
 Authors: Roberto Mucci
 
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
#include "medPipeCompoundTest.h"
#include "medPipeCompound.h"
#include "mafSceneNode.h"
#include "medGUIDynamicVP.h"

#include "mafVMEVolumeGray.h"


#include <iostream>

class DummyPipeCompound : public medPipeCompound
{
public:
  mafTypeMacro(DummyPipeCompound,medPipeCompound);
  DummyPipeCompound(){}
  /*virtual*/ void Create(mafSceneNode *n);
  int GetEnumTABCTRL() {return ID_TABCTRL;}
protected:
  // redefinition for avoiding gui components
  /*virtual*/ mafGUI *CreateGui() {return NULL;}
  /*virtual*/ void    CreatePageGroups(){}
  /*virtual*/ void    UpdateGUILayout(bool bForce){}
  /*virtual*/ void    OnChangeName(){printf("\nID_NAME\n");}
  /*virtual*/ void    OnCreateVP(){printf("\ID_CREATE_VP\n");}
  /*virtual*/ void    OnCloseVP(){printf("\ID_CLOSE_VP\n");}
};

//----------------------------------------------------------------------------
void DummyPipeCompound::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  //remove wxCursor busy;

  mafPipe::Create(n);
  m_SceneNode = n; 

  //remove GetGui();, used the same code for Create SceneNode

  //creates description of groups
  CreatePageGroups();

  //creates initial pages for each group
  CreatePages();

  //AddActor();
}

//----------------------------------------------------------------------------
mafCxxTypeMacro(DummyPipeCompound);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void medPipeCompoundTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void medPipeCompoundTest::setUp()
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
void medPipeCompoundTest::tearDown()
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
void medPipeCompoundTest::TestAllocation()
//----------------------------------------------------------------------------
{
  DummyPipeCompound *pipe = new DummyPipeCompound;
  mafDEL(pipe);
}
//----------------------------------------------------------------------------
void medPipeCompoundTest::TestCreateSceneNode()
//----------------------------------------------------------------------------
{
  mafVMEVolumeGray *volume;
  mafNEW(volume);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,volume, NULL);

  /////////// Pipe Instance and Creation ///////////
  DummyPipeCompound *pipe = new DummyPipeCompound;
  pipe->Create(sceneNode);
  
  delete pipe;
  delete sceneNode;
  mafDEL(volume);

  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void medPipeCompoundTest::TestOnEvent()
//----------------------------------------------------------------------------
{
  
  /////////// Pipe Instance and Creation ///////////
  DummyPipeCompound *pipe = new DummyPipeCompound;
  pipe->OnEvent(&mafEvent(this, pipe->GetEnumTABCTRL(), (long) medGUIDynamicVP::ID_NAME));
  pipe->OnEvent(&mafEvent(this, pipe->GetEnumTABCTRL(), (long) medGUIDynamicVP::ID_CREATE_VP));
  pipe->OnEvent(&mafEvent(this, pipe->GetEnumTABCTRL(), (long) medGUIDynamicVP::ID_CLOSE_VP));
  

  delete pipe;
  
  delete wxLog::SetActiveTarget(NULL);
}
