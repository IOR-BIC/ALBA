/*=========================================================================

 Program: MAF2
 Module: mafVMEGroupTest
 Authors: Matteo Giacomoni
 
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
#include "mafVMEGroupTest.h"
#include <iostream>

#include "mafVMEGroup.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafVMESurface.h"

#include "mafSmartPointer.h"

#include "vtkSphereSource.h"

void mafVMEGroupTest::Test()
{
	mafVMEStorage *storage = mafVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();
	mafVMERoot *root=storage->GetRoot();

	mafVMEGroup *group;
	mafNEW(group);
	root->AddChild(group);

	//Check if the Output of the group is a NULL Output
	CPPUNIT_ASSERT(group->GetOutput()->IsA("mafVMEOutputNULL"));

	// create some VTK data
	vtkSphereSource *sphereSource;
	vtkNEW(sphereSource);
	sphereSource->Update();

	mafVMESurface *surface;
	mafNEW(surface);
	surface->SetData(sphereSource->GetOutput(),0);

	group->AddChild(surface);
	CPPUNIT_ASSERT(group->GetNumberOfChildren()==1);

	root->RemoveAllChildren();
	vtkDEL(sphereSource);
	mafDEL(surface);
	mafDEL(group);
	mafDEL(storage);
}

