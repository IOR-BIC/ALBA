/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEGroupTest
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
#include "albaVMEGroupTest.h"
#include <iostream>

#include "albaVMEGroup.h"
#include "albaVMEStorage.h"
#include "albaVMERoot.h"
#include "albaVMESurface.h"

#include "albaSmartPointer.h"

#include "vtkSphereSource.h"

void albaVMEGroupTest::Test()
{
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();
	albaVMERoot *root=storage->GetRoot();

	albaVMEGroup *group;
	albaNEW(group);
	root->AddChild(group);

	//Check if the Output of the group is a NULL Output
	CPPUNIT_ASSERT(group->GetOutput()->IsA("albaVMEOutputNULL"));

	// create some VTK data
	vtkSphereSource *sphereSource;
	vtkNEW(sphereSource);
	sphereSource->Update();

	albaVMESurface *surface;
	albaNEW(surface);
	surface->SetData(sphereSource->GetOutput(),0);

	group->AddChild(surface);
	CPPUNIT_ASSERT(group->GetNumberOfChildren()==1);

	root->RemoveAllChildren();
	vtkDEL(sphereSource);
	albaDEL(surface);
	albaDEL(group);
	albaDEL(storage);
}

