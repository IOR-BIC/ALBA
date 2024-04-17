/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateLabeledVolumeTest
 Authors: Alessandro Chiarini , Matteo Giacomoni
 
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

#include "albaOpCreateLabeledVolumeTest.h"
#include "albaOpCreateLabeledVolume.h"

#include "albaVMEVolumeGray.h"
#include "albaVMELabeledVolume.h"
#include "vtkALBASmartPointer.h"
#include "albaSmartPointer.h"

#include "vtkCharArray.h"
#include "vtkPointData.h"
#include "vtkImageData.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void albaOpCreateLabeledVolumeTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaOpCreateLabeledVolume *create=new albaOpCreateLabeledVolume();
  albaDEL(create);
}
//-----------------------------------------------------------
void albaOpCreateLabeledVolumeTest::TestOpRun() 
//-----------------------------------------------------------
{
  vtkALBASmartPointer<vtkImageData> data;
  data->SetSpacing(1.0,1.0,1.0);
  data->SetDimensions(10,10,10);
  

  vtkALBASmartPointer<vtkCharArray> array;
  array->Allocate(10*10*10);

  data->GetPointData()->AddArray(array);

  albaSmartPointer<albaVMEVolumeGray> volume;
  volume->SetData(data.GetPointer(), 0.0);
  volume->Modified();
  volume->Update();

  albaOpCreateLabeledVolume *create=new albaOpCreateLabeledVolume();
  create->TestModeOn();
  create->SetInput(volume);
  create->OpRun();

  albaVMELabeledVolume *labeledvolume = NULL;
	labeledvolume = albaVMELabeledVolume::SafeDownCast(create->GetOutput());
  
  CPPUNIT_ASSERT(labeledvolume != NULL);

  albaDEL(create);
}
//-----------------------------------------------------------
void albaOpCreateLabeledVolumeTest::TestCopy() 
//-----------------------------------------------------------
{
  albaOpCreateLabeledVolume *create=new albaOpCreateLabeledVolume();
  albaOpCreateLabeledVolume *create2 = albaOpCreateLabeledVolume::SafeDownCast(create->Copy());

  CPPUNIT_ASSERT(create2 != NULL);

  albaDEL(create2);
  albaDEL(create);
}
//-----------------------------------------------------------
void albaOpCreateLabeledVolumeTest::TestAccept() 
//-----------------------------------------------------------
{
  albaSmartPointer<albaVMEVolumeGray> volume;

  albaOpCreateLabeledVolume *create=new albaOpCreateLabeledVolume();
  CPPUNIT_ASSERT(create->Accept(volume));
  CPPUNIT_ASSERT(!create->Accept(NULL));

  albaDEL(create);
}
//-----------------------------------------------------------
void albaOpCreateLabeledVolumeTest::TestOpDo() 
//-----------------------------------------------------------
{
  vtkALBASmartPointer<vtkImageData> data;
  data->SetSpacing(1.0,1.0,1.0);
  data->SetDimensions(10,10,10);


  vtkALBASmartPointer<vtkCharArray> array;
  array->Allocate(10*10*10);

  data->GetPointData()->AddArray(array);

  albaSmartPointer<albaVMEVolumeGray> volume;
  volume->SetData(data.GetPointer(), 0.0);
  volume->Modified();
  volume->Update();

  int numOfChildren = volume->GetNumberOfChildren();
  CPPUNIT_ASSERT(numOfChildren == 0);

  albaOpCreateLabeledVolume *create=new albaOpCreateLabeledVolume();
  create->TestModeOn();
  create->SetInput(volume);
  create->OpRun();

  create->OpDo();
  numOfChildren = volume->GetNumberOfChildren();
  CPPUNIT_ASSERT(numOfChildren == 1);
  albaDEL(create);
}