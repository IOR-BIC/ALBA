/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterMSF1xTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaDefines.h"

#include <cppunit/config/SourcePrefix.h>
#include "albaOpImporterMSF1xTest.h"
#include "albaOpImporterMSF1x.h"
#include "albaString.h"
#include "albaVMERoot.h"
#include "albaVMEStorage.h"
#include "albaVMELandmarkCloud.h"
#include "albaVME.h"
#include "vtkDataSet.h"
#include "albaVMEIterator.h"
#include "albaAbsMatrixPipe.h"
#include "albaMatrix.h"
#include "albaVMELandmark.h"
#include "albaVMERefSys.h"
#include <iostream>

void albaOpImporterMSF1xTest::Test()
{
	albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
	albaOpImporterMSF1x *importer=new albaOpImporterMSF1x("importer");
	importer->SetInput(storage->GetRoot());
	albaString filename=ALBA_DATA_ROOT;
  filename<<"/MSF1X/MSF1X.msf";
	importer->SetFileName(filename);
	importer->OpRun();

	int i=storage->GetRoot()->GetNumberOfChildren();
	CPPUNIT_ASSERT(i==2);
	for(int j=0;j<i;j++)
	{
		albaVME *node = storage->GetRoot()->GetChild(j);
		switch (j)
		{
		case 0:
			CPPUNIT_ASSERT(node->IsA("albaVMESurface"));
			break;
		case 1:
			CPPUNIT_ASSERT(node->IsA("albaVMEVolume"));
			break;
		default:
			CPPUNIT_ASSERT(false);
			break;
		}

		if(node->IsA("albaVMESurface"))
		{
			vtkDataSet *data=node->GetOutput()->GetVTKData();
			data->Update();
			int cells=data->GetNumberOfCells();
			CPPUNIT_ASSERT(cells==12);
			int n=node->GetNumberOfChildren();
			CPPUNIT_ASSERT(n==0);
		}
		if(node->IsA("albaVMEVolume"))
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
  
