/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpDecimateSurfaceTest.cpp,v $
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
#include "mafOpDecimateSurfaceTest.h"

#include "mafOpImporterSTL.h"
#include "mafOpDecimateSurface.h"

#include "mafString.h"
#include "mafNode.h"
#include "mafVMERoot.h"
#include "mafVMEStorage.h"
#include "mafVMESurface.h"

#include "vtkPolyData.h"

//----------------------------------------------------------------------------
void mafOpDecimateSurfaceTest::Test()
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
  std::vector<mafVMESurface*> importedSTL;
  importer->GetImportedSTL(importedSTL);
	mafVMESurface *surface = importedSTL[0];
  CPPUNIT_ASSERT(surface);

  surface->Update();
	surface->GetOutput()->GetVTKData()->Update();

	mafOpDecimateSurface *decimate=new mafOpDecimateSurface;
	decimate->TestModeOn();
	decimate->SetReduction(20);
	decimate->SetInput(surface);
	decimate->OpRun();
	decimate->OnDecimate();
	decimate->OnPreview();
	surface->Update();
	surface->GetOutput()->GetVTKData()->Update();
	vtkPolyData *data=vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());
  CPPUNIT_ASSERT(data);

  data->Update();
	CPPUNIT_ASSERT(data->GetNumberOfPoints()==94);

	mafDEL(decimate);
	mafDEL(importer);
	mafDEL(storage);
}
