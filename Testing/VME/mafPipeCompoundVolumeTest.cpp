/*=========================================================================

 Program: MAF2
 Module: mafPipeCompoundVolumeTest
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
#include "mafPipeCompoundVolumeTest.h"
#include "mafPipeCompoundVolume.h"
#include "mafSceneNode.h"
#include "mafGUIDynamicVP.h"

#include "mafVMEVolumeGray.h"
#include "vtkMAFSmartPointer.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkFloatArray.h"

#include <iostream>

class DummyPipeCompoundVolume : public mafPipeCompoundVolume
{
public:
  mafTypeMacro(DummyPipeCompoundVolume,mafPipeCompoundVolume);
  DummyPipeCompoundVolume(){}
  /*virtual*/ void Create(mafSceneNode *n);
  int GetEnumTABCTRL() {return ID_TABCTRL;}
protected:
  // redefinition for avoiding gui components
  /*virtual*/ mafGUI *CreateGui() {return NULL;}
  /*virtual*/ bool CreatePageGroups(const char* szPageName, const mafGUIDynamicVP::SUPPORTED_VP_ENTRY* pPipes) {return true;}
  /*virtual*/ void    UpdateGUILayout(bool bForce){}
  /*virtual*/ void    OnChangeName(){printf("\nID_NAME\n");}
  /*virtual*/ void    OnCreateVP(){printf("\nID_CREATE_VP\n");}
  /*virtual*/ void    OnCloseVP(){printf("\nID_CLOSE_VP\n");}
};

//----------------------------------------------------------------------------
void DummyPipeCompoundVolume::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  //remove wxCursor busy;

  mafPipe::Create(n);
  m_SceneNode = n; 

  //remove GetGui();, used the same code for Create SceneNode

  //creates description of groups
  mafPipeCompoundVolume::CreatePageGroups();

  //creates initial pages for each group
  CreatePages();

  //AddActor();
}

//----------------------------------------------------------------------------
mafCxxTypeMacro(DummyPipeCompoundVolume);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void mafPipeCompoundVolumeTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafPipeCompoundVolumeTest::TestAllocation()
//----------------------------------------------------------------------------
{
  DummyPipeCompoundVolume *pipe = new DummyPipeCompoundVolume;
  mafDEL(pipe);
}
//----------------------------------------------------------------------------
void mafPipeCompoundVolumeTest::TestCreateSceneNode()
//----------------------------------------------------------------------------
{
  mafVMEVolumeGray *volume;
  mafNEW(volume);

  vtkMAFSmartPointer<vtkFloatArray> scalarArray;
  scalarArray->SetName("Scalar");
  scalarArray->InsertNextTuple1(1.0);

  vtkMAFSmartPointer<vtkFloatArray> vectorArray;
  vectorArray->SetNumberOfComponents(3);
  vectorArray->SetName("Vector");
  vectorArray->InsertNextTuple3(1.0,1.0,1.0);

  vtkMAFSmartPointer<vtkFloatArray> tensorArray;
  tensorArray->SetNumberOfComponents(9);
  tensorArray->SetName("Tensor");
  tensorArray->InsertNextTuple9(1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0);

  vtkMAFSmartPointer<vtkImageData> image;
  image->SetDimensions(1,1,1);
  image->SetSpacing(1.,1.,1.);
  
  image->GetPointData()->SetScalars(scalarArray);
  
  volume->SetDataByReference(image, 0.);
  volume->Update();

  image->GetPointData()->AddArray(vectorArray);
  image->GetPointData()->AddArray(tensorArray);

  //Assembly will be create when instancing mafSceneNode
  mafSceneNode *sceneNode;
  sceneNode = new mafSceneNode(NULL,NULL,volume, NULL);

  /////////// Pipe Instance and Creation ///////////
  DummyPipeCompoundVolume *pipe = new DummyPipeCompoundVolume;
  pipe->Create(sceneNode);
  
  delete pipe;
  delete sceneNode;
  mafDEL(volume);
}
//----------------------------------------------------------------------------
void mafPipeCompoundVolumeTest::TestOnEvent()
//----------------------------------------------------------------------------
{
  
  /////////// Pipe Instance and Creation ///////////
  DummyPipeCompoundVolume *pipe = new DummyPipeCompoundVolume;
  pipe->OnEvent(&mafEvent(this, pipe->GetEnumTABCTRL(), (long) mafGUIDynamicVP::ID_NAME));
  pipe->OnEvent(&mafEvent(this, pipe->GetEnumTABCTRL(), (long) mafGUIDynamicVP::ID_CREATE_VP));
  pipe->OnEvent(&mafEvent(this, pipe->GetEnumTABCTRL(), (long) mafGUIDynamicVP::ID_CLOSE_VP));
  
  delete pipe;
}
