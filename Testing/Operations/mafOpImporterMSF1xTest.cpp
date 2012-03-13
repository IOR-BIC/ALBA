/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpImporterMSF1xTest.cpp,v $
Language:  C++
Date:      $Date: 2008-03-06 12:01:16 $
Version:   $Revision: 1.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include <cppunit/config/SourcePrefix.h>
#include "mafOpImporterMSF1xTest.h"
#include "mafOpImporterMSF1x.h"
#include "mafString.h"
#include "mafVMERoot.h"
#include "mafVMEStorage.h"
#include "mafVMELandmarkCloud.h"
#include "mafVME.h"
#include "vtkDataSet.h"
#include "mafNodeIterator.h"
#include "mafAbsMatrixPipe.h"
#include "mafMatrix.h"
#include "mafVMELandmark.h"
#include "mafVMERefSys.h"
#include <iostream>

void mafOpImporterMSF1xTest::Test()
{
	mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
	mafOpImporterMSF1x *importer=new mafOpImporterMSF1x("importer");
	importer->SetInput(storage->GetRoot());
	mafString filename=MAF_DATA_ROOT;
  filename<<"/MSF1X/MSF1X.msf";
	importer->SetFileName(filename);
	importer->OpRun();

	int i=storage->GetRoot()->GetNumberOfChildren();
	CPPUNIT_ASSERT(i==2);
	for(int j=0;j<i;j++)
	{
		mafNode *node = storage->GetRoot()->GetChild(j);
		switch (j)
		{
		case 0:
			CPPUNIT_ASSERT(node->IsA("mafVMESurface"));
			break;
		case 1:
			CPPUNIT_ASSERT(node->IsA("mafVMEVolume"));
			break;
		default:
			CPPUNIT_ASSERT(false);
			break;
		}

		if(node->IsA("mafVMESurface"))
		{
			vtkDataSet *data=((mafVME *)node)->GetOutput()->GetVTKData();
			data->Update();
			int cells=data->GetNumberOfCells();
			CPPUNIT_ASSERT(cells==12);
			int n=node->GetNumberOfChildren();
			CPPUNIT_ASSERT(n==0);
		}
		if(node->IsA("mafVMEVolume"))
		{
			vtkDataSet *data=((mafVME *)node)->GetOutput()->GetVTKData();
			data->Update();
			double range[2];
			data->GetScalarRange(range);
			CPPUNIT_ASSERT(range[0]==-1024.00 && range[1]==1698.00);
			int points=data->GetNumberOfPoints();
			CPPUNIT_ASSERT(points==28896);
			int n=node->GetNumberOfChildren();
			CPPUNIT_ASSERT(n==0);
		}
	}
}
  
