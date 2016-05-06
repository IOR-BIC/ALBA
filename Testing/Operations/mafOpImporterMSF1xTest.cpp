/*=========================================================================

 Program: MAF2
 Module: mafOpImporterMSF1xTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
#include "mafVMEIterator.h"
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
		mafVME *node = storage->GetRoot()->GetChild(j);
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
			vtkDataSet *data=node->GetOutput()->GetVTKData();
			data->Update();
			int cells=data->GetNumberOfCells();
			CPPUNIT_ASSERT(cells==12);
			int n=node->GetNumberOfChildren();
			CPPUNIT_ASSERT(n==0);
		}
		if(node->IsA("mafVMEVolume"))
		{
			vtkDataSet *data=node->GetOutput()->GetVTKData();
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
  
