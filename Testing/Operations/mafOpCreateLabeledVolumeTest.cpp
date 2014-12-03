/*=========================================================================

 Program: MAF2
 Module: mafOpCreateLabeledVolumeTest
 Authors: Alessandro Chiarini , Matteo Giacomoni
 
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

#include "mafOpCreateLabeledVolumeTest.h"
#include "mafOpCreateLabeledVolume.h"

#include "mafVMEVolumeGray.h"
#include "mafVMELabeledVolume.h"
#include "vtkMAFSmartPointer.h"
#include "mafSmartPointer.h"

#include "vtkCharArray.h"
#include "vtkPointData.h"
#include "vtkImageData.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void mafOpCreateLabeledVolumeTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  mafOpCreateLabeledVolume *create=new mafOpCreateLabeledVolume();
  mafDEL(create);
}
//-----------------------------------------------------------
void mafOpCreateLabeledVolumeTest::TestOpRun() 
//-----------------------------------------------------------
{
  vtkMAFSmartPointer<vtkImageData> data;
  data->SetSpacing(1.0,1.0,1.0);
  data->SetDimensions(10,10,10);
  

  vtkMAFSmartPointer<vtkCharArray> array;
  array->Allocate(10*10*10);

  data->GetPointData()->AddArray(array);
  data->Update();

  mafSmartPointer<mafVMEVolumeGray> volume;
  volume->SetData(data.GetPointer(), 0.0);
  volume->Modified();
  volume->Update();

  mafOpCreateLabeledVolume *create=new mafOpCreateLabeledVolume();
  create->TestModeOn();
  create->SetInput(volume);
  create->OpRun();

  mafVMELabeledVolume *labeledvolume = NULL;
	labeledvolume = mafVMELabeledVolume::SafeDownCast(create->GetOutput());
  
  CPPUNIT_ASSERT(labeledvolume != NULL);

  mafDEL(create);
}
//-----------------------------------------------------------
void mafOpCreateLabeledVolumeTest::TestCopy() 
//-----------------------------------------------------------
{
  mafOpCreateLabeledVolume *create=new mafOpCreateLabeledVolume();
  mafOpCreateLabeledVolume *create2 = mafOpCreateLabeledVolume::SafeDownCast(create->Copy());

  CPPUNIT_ASSERT(create2 != NULL);

  mafDEL(create2);
  mafDEL(create);
}
//-----------------------------------------------------------
void mafOpCreateLabeledVolumeTest::TestAccept() 
//-----------------------------------------------------------
{
  mafSmartPointer<mafVMEVolumeGray> volume;

  mafOpCreateLabeledVolume *create=new mafOpCreateLabeledVolume();
  CPPUNIT_ASSERT(create->Accept(volume));
  CPPUNIT_ASSERT(!create->Accept(NULL));

  mafDEL(create);
}
//-----------------------------------------------------------
void mafOpCreateLabeledVolumeTest::TestOpDo() 
//-----------------------------------------------------------
{
  vtkMAFSmartPointer<vtkImageData> data;
  data->SetSpacing(1.0,1.0,1.0);
  data->SetDimensions(10,10,10);


  vtkMAFSmartPointer<vtkCharArray> array;
  array->Allocate(10*10*10);

  data->GetPointData()->AddArray(array);
  data->Update();

  mafSmartPointer<mafVMEVolumeGray> volume;
  volume->SetData(data.GetPointer(), 0.0);
  volume->Modified();
  volume->Update();

  int numOfChildren = volume->GetNumberOfChildren();
  CPPUNIT_ASSERT(numOfChildren == 0);

  mafOpCreateLabeledVolume *create=new mafOpCreateLabeledVolume();
  create->TestModeOn();
  create->SetInput(volume);
  create->OpRun();

  create->OpDo();
  numOfChildren = volume->GetNumberOfChildren();
  CPPUNIT_ASSERT(numOfChildren == 1);
  mafDEL(create);
}