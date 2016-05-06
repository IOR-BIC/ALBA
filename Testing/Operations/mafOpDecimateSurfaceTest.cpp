/*=========================================================================

 Program: MAF2
 Module: mafOpDecimateSurfaceTest
 Authors: Daniele Giunchi - Matteo Giacomoni
 
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
#include "mafOpDecimateSurfaceTest.h"

#include "mafOpImporterSTL.h"
#include "mafOpDecimateSurface.h"

#include "mafString.h"
#include "mafVME.h"
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
