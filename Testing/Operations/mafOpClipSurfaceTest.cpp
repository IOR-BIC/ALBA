/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpClipSurfaceTest.cpp,v $
Language:  C++
Date:      $Date: 2008-03-06 12:01:16 $
Version:   $Revision: 1.1 $
Authors:   Daniele Giunchi - Matteo Giacomoni
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
#include "mafOpClipSurfaceTest.h"

#include "mafOpClipSurface.h"
#include "mafVMERoot.h"
#include "mafVMEStorage.h"
#include "mafVMESurface.h"
#include "mafOpImporterSTL.h"
#include "mafString.h"
#include "mafNode.h"


#include "vtkPolyData.h"
#include "vtkPlaneSource.h"

//----------------------------------------------------------------------------
void mafOpClipSurfaceTest::Test()
//----------------------------------------------------------------------------
{
	mafVMEStorage *storage = mafVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();
	mafOpImporterSTL *importer=new mafOpImporterSTL("importer");
	importer->TestModeOn();
	importer->SetInput(storage->GetRoot());
	mafString filename=MAF_DATA_ROOT;
	filename<<"/STL/GenericSTL.stl";
	importer->SetFileName(filename.GetCStr());
	importer->OpRun();
	mafNode *node=importer->GetOutput();

	mafVMESurface *surface=mafVMESurface::SafeDownCast(node);
	surface->Update();
	surface->GetOutput()->GetVTKData()->Update();
	CPPUNIT_ASSERT(surface);

  vtkPlaneSource *plane;
	vtkNEW(plane);

	mafVMESurface *clipperSurface;
	mafNEW(clipperSurface);
	clipperSurface->SetData(plane->GetOutput(),0);
	clipperSurface->Update();
	clipperSurface->GetOutput()->GetVTKData()->Update();
	CPPUNIT_ASSERT(clipperSurface);

	mafOpClipSurface *clip=new mafOpClipSurface;
	clip->TestModeOn();
	clip->SetInput(surface);
	clip->OpRun();
	clip->SetClippingModality(mafOpClipSurface::MODE_SURFACE);
	
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
	mafDEL(clipperSurface);
	mafDEL(clip);
	mafDEL(importer);
	mafDEL(storage);
}
