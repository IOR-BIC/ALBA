/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeCompoundTest
 Authors: Roberto Mucci
 
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
#include "albaPipeCompoundTest.h"
#include "albaPipeCompound.h"
#include "albaSceneNode.h"
#include "albaGUIDynamicVP.h"

#include "albaVMEVolumeGray.h"


#include <iostream>

class DummyPipeCompound : public albaPipeCompound
{
public:
  albaTypeMacro(DummyPipeCompound,albaPipeCompound);
  DummyPipeCompound(){}
  /*virtual*/ void Create(albaSceneNode *n);
  int GetEnumTABCTRL() {return ID_TABCTRL;}
protected:
  // redefinition for avoiding gui components
  /*virtual*/ albaGUI *CreateGui() {return NULL;}
  /*virtual*/ void    CreatePageGroups(){}
  /*virtual*/ void    UpdateGUILayout(bool bForce){}
  /*virtual*/ void    OnChangeName(){printf("\nID_NAME\n");}
  /*virtual*/ void    OnCreateVP(){printf("\nID_CREATE_VP\n");}
  /*virtual*/ void    OnCloseVP(){printf("\nID_CLOSE_VP\n");}
};

//----------------------------------------------------------------------------
void DummyPipeCompound::Create(albaSceneNode *n)
//----------------------------------------------------------------------------
{
  //remove wxCursor busy;

  albaPipe::Create(n);
  m_SceneNode = n; 

  //remove GetGui();, used the same code for Create SceneNode

  //creates description of groups
  CreatePageGroups();

  //creates initial pages for each group
  CreatePages();

  //AddActor();
}

//----------------------------------------------------------------------------
albaCxxTypeMacro(DummyPipeCompound);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void albaPipeCompoundTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaPipeCompoundTest::TestAllocation()
//----------------------------------------------------------------------------
{
  DummyPipeCompound *pipe = new DummyPipeCompound;
  albaDEL(pipe);
}
//----------------------------------------------------------------------------
void albaPipeCompoundTest::TestCreateSceneNode()
//----------------------------------------------------------------------------
{
  albaVMEVolumeGray *volume;
  albaNEW(volume);

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,volume, NULL);

  /////////// Pipe Instance and Creation ///////////
  DummyPipeCompound *pipe = new DummyPipeCompound;
  pipe->Create(sceneNode);
  
  delete sceneNode;
  albaDEL(volume);
}
//----------------------------------------------------------------------------
void albaPipeCompoundTest::TestOnEvent()
//----------------------------------------------------------------------------
{
  
  /////////// Pipe Instance and Creation ///////////
  DummyPipeCompound *pipe = new DummyPipeCompound;
  pipe->OnEvent(&albaEvent(this, pipe->GetEnumTABCTRL(), (long) albaGUIDynamicVP::ID_NAME));
  pipe->OnEvent(&albaEvent(this, pipe->GetEnumTABCTRL(), (long) albaGUIDynamicVP::ID_CREATE_VP));
  pipe->OnEvent(&albaEvent(this, pipe->GetEnumTABCTRL(), (long) albaGUIDynamicVP::ID_CLOSE_VP));
  
  delete pipe;
}
