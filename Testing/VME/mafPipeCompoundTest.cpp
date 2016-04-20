/*=========================================================================

 Program: MAF2
 Module: mafPipeCompoundTest
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
#include "mafPipeCompoundTest.h"
#include "mafPipeCompound.h"
#include "mafSceneNode.h"
#include "mafGUIDynamicVP.h"

#include "mafVMEVolumeGray.h"


#include <iostream>

class DummyPipeCompound : public mafPipeCompound
{
public:
  mafTypeMacro(DummyPipeCompound,mafPipeCompound);
  DummyPipeCompound(){}
  /*virtual*/ void Create(mafSceneNode *n);
  int GetEnumTABCTRL() {return ID_TABCTRL;}
protected:
  // redefinition for avoiding gui components
  /*virtual*/ mafGUI *CreateGui() {return NULL;}
  /*virtual*/ void    CreatePageGroups(){}
  /*virtual*/ void    UpdateGUILayout(bool bForce){}
  /*virtual*/ void    OnChangeName(){printf("\nID_NAME\n");}
  /*virtual*/ void    OnCreateVP(){printf("\nID_CREATE_VP\n");}
  /*virtual*/ void    OnCloseVP(){printf("\nID_CLOSE_VP\n");}
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
void mafPipeCompoundTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafPipeCompoundTest::TestAllocation()
//----------------------------------------------------------------------------
{
  DummyPipeCompound *pipe = new DummyPipeCompound;
  mafDEL(pipe);
}
//----------------------------------------------------------------------------
void mafPipeCompoundTest::TestCreateSceneNode()
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
}
//----------------------------------------------------------------------------
void mafPipeCompoundTest::TestOnEvent()
//----------------------------------------------------------------------------
{
  
  /////////// Pipe Instance and Creation ///////////
  DummyPipeCompound *pipe = new DummyPipeCompound;
  pipe->OnEvent(&mafEvent(this, pipe->GetEnumTABCTRL(), (long) mafGUIDynamicVP::ID_NAME));
  pipe->OnEvent(&mafEvent(this, pipe->GetEnumTABCTRL(), (long) mafGUIDynamicVP::ID_CREATE_VP));
  pipe->OnEvent(&mafEvent(this, pipe->GetEnumTABCTRL(), (long) mafGUIDynamicVP::ID_CLOSE_VP));
  
  delete pipe;
}
