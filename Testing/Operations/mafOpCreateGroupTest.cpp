/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpCreateGroupTest.cpp,v $
Language:  C++
Date:      $Date: 2010-10-13 12:36:32 $
Version:   $Revision: 1.1.2.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafOpCreateGroupTest.h"

#include "mafOpCreateGroup.h"
#include "vtkMAFSmartPointer.h"
#include "mafVMESurface.h"
#include "vtkPolyDataReader.h"

//-----------------------------------------------------------
void mafOpCreateGroupTest::Test() 
//-----------------------------------------------------------
{
  mafString fileName = MAF_DATA_ROOT;
  fileName<<"\\Test_FilterSurface\\SurfaceToClean.vtk";
  vtkMAFSmartPointer<vtkPolyDataReader> reader;
  reader->SetFileName(fileName.GetCStr());
  reader->Update();

  mafSmartPointer<mafVMESurface> surface;
  surface->SetData(reader->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->Update();

	mafOpCreateGroup *CreateGroup=new mafOpCreateGroup("Create Group");
	CreateGroup->SetInput(surface);
	CreateGroup->OpRun();

	//Check if VME created is a group
	CPPUNIT_ASSERT(CreateGroup->GetOutput()->IsA("mafVMEGroup"));

	mafDEL(CreateGroup);
}
