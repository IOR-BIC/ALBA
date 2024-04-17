/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpDecimateSurfaceTest
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
#include "albaOpDecimateSurfaceTest.h"

#include "albaOpImporterSTL.h"
#include "albaOpDecimateSurface.h"

#include "albaString.h"
#include "albaVME.h"
#include "albaVMERoot.h"
#include "albaVMEStorage.h"
#include "albaVMESurface.h"

#include "vtkPolyData.h"

//----------------------------------------------------------------------------
void albaOpDecimateSurfaceTest::Test()
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
  std::vector<albaVMESurface*> importedSTL;
  importer->GetImportedSTL(importedSTL);
	albaVMESurface *surface = importedSTL[0];
  CPPUNIT_ASSERT(surface);

  surface->Update();

	albaOpDecimateSurface *decimate=new albaOpDecimateSurface;
	decimate->TestModeOn();
	decimate->SetReduction(20);
	decimate->SetInput(surface);
	decimate->OpRun();
	decimate->OnDecimate();
	decimate->OnPreview();
	surface->Update();
	vtkPolyData *data=vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());
  CPPUNIT_ASSERT(data);

	CPPUNIT_ASSERT(data->GetNumberOfPoints()==94);

	albaDEL(decimate);
	albaDEL(importer);
	albaDEL(storage);
}
