/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpImporterVRMLTest.cpp,v $
Language:  C++
Date:      $Date: 2008-03-06 12:01:16 $
Version:   $Revision: 1.1 $
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
#include "mafOpImporterVRMLTest.h"
#include <iostream>

#include "mafOpImporterVRML.h"
#include "mafString.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafNode.h"
#include "mafVMESurface.h"
#include "mmaMaterial.h"

#include "vtkPolyData.h"

void mafOpImporterVRMLTest::Test()
{
	//Init storage
	mafVMEStorage *storage = mafVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

	//Init importer 
	mafOpImporterVRML *importer=new mafOpImporterVRML("importer");
	importer->TestModeOn();
	importer->SetInput(storage->GetRoot());
	mafString filename=MAF_DATA_ROOT;
	filename<<"/VRML/DatoTestVRMLImporter.wrl";
	importer->SetFileName(filename);
	importer->ImportVRML();

	//Check the output of the importer
	CPPUNIT_ASSERT(importer->GetOutput() != NULL);

	mafNode *node=importer->GetOutput();

	CPPUNIT_ASSERT(node->IsA("mafVMEGroup"));
	CPPUNIT_ASSERT(node->GetNumberOfChildren()==2);
	CPPUNIT_ASSERT(node->GetChild(0)->IsA("mafVMESurface"));
	CPPUNIT_ASSERT(node->GetChild(1)->IsA("mafVMESurface"));

	mafVMESurface *Cone=mafVMESurface::SafeDownCast(node->GetChild(0));
	mafVMESurface *Cube=mafVMESurface::SafeDownCast(node->GetChild(1));

	double bounds[6];
	Cone->GetOutput()->GetBounds(bounds);
	CPPUNIT_ASSERT(bounds[0]==-2 && bounds[1]==2 && bounds[2]==-2 && bounds[3]==2 && bounds[4]==-2 && bounds[5]==2);
	Cone->GetOutput()->GetVTKData()->Update();
	CPPUNIT_ASSERT(Cone->GetMaterial()->m_Diffuse[0]==0.0 && Cone->GetMaterial()->m_Diffuse[1]==0.0 && Cone->GetMaterial()->m_Diffuse[2]==1.0);

	Cube->GetOutput()->GetBounds(bounds);
	CPPUNIT_ASSERT(bounds[0]==-1 && bounds[1]==1 && bounds[2]==-1 && bounds[3]==1 && bounds[4]==-1 && bounds[5]==1);
	Cube->GetOutput()->GetVTKData()->Update();
	CPPUNIT_ASSERT(Cube->GetOutput()->GetVTKData()->GetNumberOfCells()==6);
	CPPUNIT_ASSERT(Cube->GetMaterial()->m_Diffuse[0]==0.0 && Cube->GetMaterial()->m_Diffuse[1]==1.0 && Cube->GetMaterial()->m_Diffuse[2]==0.0);


	node = NULL;
	Cube = NULL;
	Cone = NULL;
	mafDEL(importer);
	mafDEL(storage);
}

