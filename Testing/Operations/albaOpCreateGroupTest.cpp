/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateGroupTest
 Authors: Matteo Giacomoni
 
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
#include "albaOpCreateGroupTest.h"

#include "albaOpCreateGroup.h"
#include "vtkALBASmartPointer.h"
#include "albaVMESurface.h"
#include "vtkPolyDataReader.h"

//-----------------------------------------------------------
void albaOpCreateGroupTest::Test() 
//-----------------------------------------------------------
{
  albaString fileName = ALBA_DATA_ROOT;
  fileName<<"\\Test_FilterSurface\\SurfaceToClean.vtk";
  vtkALBASmartPointer<vtkPolyDataReader> reader;
  reader->SetFileName(fileName.GetCStr());
  reader->Update();

  albaSmartPointer<albaVMESurface> surface;
  surface->SetData(reader->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->Update();

	albaOpCreateGroup *CreateGroup=new albaOpCreateGroup("Create Group");
	CreateGroup->SetInput(surface);
	CreateGroup->OpRun();

	//Check if VME created is a group
	CPPUNIT_ASSERT(CreateGroup->GetOutput()->IsA("albaVMEGroup"));

	albaDEL(CreateGroup);
}
