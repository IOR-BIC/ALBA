/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeCompoundVolumeTest
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
#include "albaPipeCompoundVolumeTest.h"
#include "albaPipeCompoundVolume.h"
#include "albaSceneNode.h"
#include "albaGUIDynamicVP.h"

#include "albaVMEVolumeGray.h"
#include "vtkALBASmartPointer.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkFloatArray.h"

#include <iostream>

class DummyPipeCompoundVolume : public albaPipeCompoundVolume
{
public:
  albaTypeMacro(DummyPipeCompoundVolume,albaPipeCompoundVolume);
  DummyPipeCompoundVolume(){}
  /*virtual*/ void Create(albaSceneNode *n);
  int GetEnumTABCTRL() {return ID_TABCTRL;}
protected:
  // redefinition for avoiding gui components
  /*virtual*/ albaGUI *CreateGui() {return NULL;}
  /*virtual*/ bool CreatePageGroups(const char* szPageName, const albaGUIDynamicVP::SUPPORTED_VP_ENTRY* pPipes) {return true;}
  /*virtual*/ void    UpdateGUILayout(bool bForce){}
  /*virtual*/ void    OnChangeName(){printf("\nID_NAME\n");}
  /*virtual*/ void    OnCreateVP(){printf("\nID_CREATE_VP\n");}
  /*virtual*/ void    OnCloseVP(){printf("\nID_CLOSE_VP\n");}
};

//----------------------------------------------------------------------------
void DummyPipeCompoundVolume::Create(albaSceneNode *n)
//----------------------------------------------------------------------------
{
  //remove wxCursor busy;

  albaPipe::Create(n);
  m_SceneNode = n; 

  //remove GetGui();, used the same code for Create SceneNode

  //creates description of groups
  albaPipeCompoundVolume::CreatePageGroups();

  //creates initial pages for each group
  CreatePages();

  //AddActor();
}

//----------------------------------------------------------------------------
albaCxxTypeMacro(DummyPipeCompoundVolume);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void albaPipeCompoundVolumeTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeCompoundVolumeTest::TestAllocation()
//----------------------------------------------------------------------------
{
  DummyPipeCompoundVolume *pipe = new DummyPipeCompoundVolume;
  albaDEL(pipe);
}
//----------------------------------------------------------------------------
void albaPipeCompoundVolumeTest::TestCreateSceneNode()
//----------------------------------------------------------------------------
{
  albaVMEVolumeGray *volume;
  albaNEW(volume);

  vtkALBASmartPointer<vtkFloatArray> scalarArray;
  scalarArray->SetName("Scalar");
  scalarArray->InsertNextTuple1(1.0);

  vtkALBASmartPointer<vtkFloatArray> vectorArray;
  vectorArray->SetNumberOfComponents(3);
  vectorArray->SetName("Vector");
  vectorArray->InsertNextTuple3(1.0,1.0,1.0);

  vtkALBASmartPointer<vtkFloatArray> tensorArray;
  tensorArray->SetNumberOfComponents(9);
  tensorArray->SetName("Tensor");
  tensorArray->InsertNextTuple9(1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0);

  vtkALBASmartPointer<vtkImageData> image;
  image->SetDimensions(1,1,1);
  image->SetSpacing(1.,1.,1.);
  
  image->GetPointData()->SetScalars(scalarArray);
  
  volume->SetDataByReference(image, 0.);
  volume->Update();

  image->GetPointData()->AddArray(vectorArray);
  image->GetPointData()->AddArray(tensorArray);

  //Assembly will be create when instancing albaSceneNode
  albaSceneNode *sceneNode;
  sceneNode = new albaSceneNode(NULL,NULL,volume, NULL);

  /////////// Pipe Instance and Creation ///////////
  DummyPipeCompoundVolume *pipe = new DummyPipeCompoundVolume;
  pipe->Create(sceneNode);
  
  delete sceneNode;
  albaDEL(volume);
}
//----------------------------------------------------------------------------
void albaPipeCompoundVolumeTest::TestOnEvent()
//----------------------------------------------------------------------------
{
  
  /////////// Pipe Instance and Creation ///////////
  DummyPipeCompoundVolume *pipe = new DummyPipeCompoundVolume;
  pipe->OnEvent(&albaEvent(this, pipe->GetEnumTABCTRL(), (long) albaGUIDynamicVP::ID_NAME));
  pipe->OnEvent(&albaEvent(this, pipe->GetEnumTABCTRL(), (long) albaGUIDynamicVP::ID_CREATE_VP));
  pipe->OnEvent(&albaEvent(this, pipe->GetEnumTABCTRL(), (long) albaGUIDynamicVP::ID_CLOSE_VP));
  
  delete pipe;
}
