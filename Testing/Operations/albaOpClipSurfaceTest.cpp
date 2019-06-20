/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpClipSurfaceTest
 Authors: Daniele Giunchi - Matteo Giacomoni
 
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
#include "albaOpClipSurfaceTest.h"

#include "albaOpClipSurface.h"
#include "albaVMERoot.h"
#include "albaVMEStorage.h"
#include "albaVMESurface.h"
#include "albaOpImporterSTL.h"
#include "albaString.h"
#include "albaNode.h"


#include "vtkPolyData.h"
#include "vtkPlaneSource.h"

//----------------------------------------------------------------------------
void albaOpClipSurfaceTest::Test()
//----------------------------------------------------------------------------
{
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();
	albaOpImporterSTL *importer=new albaOpImporterSTL("importer");
	importer->TestModeOn();
	importer->SetInput(storage->GetRoot());
	albaString filename=ALBA_DATA_ROOT;
	filename<<"/STL/GenericSTL.stl";
	importer->SetFileName(filename.GetCStr());
	importer->OpRun();
	albaNode *node=importer->GetOutput();

	albaVMESurface *surface=albaVMESurface::SafeDownCast(node);
	surface->Update();
	surface->GetOutput()->GetVTKData()->Update();
	CPPUNIT_ASSERT(surface);

  vtkPlaneSource *plane;
	vtkNEW(plane);

	albaVMESurface *clipperSurface;
	albaNEW(clipperSurface);
	clipperSurface->SetData(plane->GetOutput(),0);
	clipperSurface->Update();
	clipperSurface->GetOutput()->GetVTKData()->Update();
	CPPUNIT_ASSERT(clipperSurface);

	albaOpClipSurface *clip=new albaOpClipSurface;
	clip->TestModeOn();
	clip->SetInput(surface);
	clip->OpRun();
	clip->SetClippingModality(albaOpClipSurface::MODE_SURFACE);
	
	clip->SetClippingSurface(clipperSurface);
	clip->Clip();

	vtkPolyData *data = clip->GetResultPolyData();
	vtkPolyData *dataClipped =  clip->GetClippedPolyData();

	data->Update();
	dataClipped->Update();

  int num = data->GetNumberOfPoints();
	int numClipped = dataClipped->GetNumberOfPoints();

	CPPUNIT_ASSERT(data);
	CPPUNIT_ASSERT(dataClipped);

	CPPUNIT_ASSERT(num != 0);
	CPPUNIT_ASSERT(numClipped != 0);

	// test if all points of the result polydata are at side of the vtkPlaneSource, 
	//considering z (=0 for that default plane)
	int count = 0;
	for(int i = 0 ; i< num ; i++)
	{
		double point[3];
		data->GetPoint(i, point);

		if(point[2] <= 0.001) count++;
	}
  CPPUNIT_ASSERT(count == num);

	// test if all points of the clipped polydata are at the other side of the vtkPlaneSource, 
	// considering z (=0 for that default plane)
	count = 0;
	for(int i = 0 ; i< numClipped ; i++)
	{
		double point[3];
		dataClipped->GetPoint(i, point);

		if(point[2] >= -0.001) count++;
	}
	CPPUNIT_ASSERT(count == numClipped);

	node = NULL;
	vtkDEL(plane);
	albaDEL(clipperSurface);
	albaDEL(clip);
	albaDEL(importer);
	albaDEL(storage);
}
