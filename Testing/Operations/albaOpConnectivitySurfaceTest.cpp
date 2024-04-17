/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpConnectivitySurfaceTest
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
#include "albaOpConnectivitySurfaceTest.h"

#include "albaOpConnectivitySurface.h"
#include "albaOpImporterSTL.h"

#include "albaVMEStorage.h"
#include "albaVMERoot.h"
#include "albaVMESurface.h"

#include "vtkPolyData.h"

//-----------------------------------------------------------
void albaOpConnectivitySurfaceTest::Test() 
//-----------------------------------------------------------
{
	//Create storage
	albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

	//Create operation
	albaOpImporterSTL *Importer=new albaOpImporterSTL("importer");
	albaString filename=ALBA_DATA_ROOT;
  filename<<"/STL/GenericSTL.stl";
	Importer->TestModeOn();
	Importer->SetFileName(filename);
	Importer->SetInput(storage->GetRoot());
	Importer->OpRun();
  std::vector<albaVMESurface*> importedSTL;
  Importer->GetImportedSTL(importedSTL);
  albaVMESurface *Data = importedSTL[0];
	Data->Update();

	albaOpConnectivitySurface *connectSurface = new albaOpConnectivitySurface;
  connectSurface->TestModeOn();
	connectSurface->SetInput(Data);
	connectSurface->OpRun();
  connectSurface->SetThresold(20.0);
  connectSurface->OnVtkConnect();
  Data->Update();

  albaString response = connectSurface->GetNumberOfExtractedSurfaces();
  CPPUNIT_ASSERT(response == albaString("2"));

	albaDEL(connectSurface);
	albaDEL(Importer);
	albaDEL(storage);
}
