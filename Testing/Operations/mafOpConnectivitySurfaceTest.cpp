/*=========================================================================

 Program: MAF2
 Module: mafOpConnectivitySurfaceTest
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
#include "mafOpConnectivitySurfaceTest.h"

#include "mafOpConnectivitySurface.h"
#include "mafOpImporterSTL.h"

#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafVMESurface.h"

#include "vtkPolyData.h"

//-----------------------------------------------------------
void mafOpConnectivitySurfaceTest::Test() 
//-----------------------------------------------------------
{
	//Create storage
	mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

	//Create operation
	mafOpImporterSTL *Importer=new mafOpImporterSTL("importer");
	mafString filename=MAF_DATA_ROOT;
  filename<<"/STL/GenericSTL.stl";
	Importer->TestModeOn();
	Importer->SetFileName(filename);
	Importer->SetInput(storage->GetRoot());
	Importer->OpRun();
  std::vector<mafVMESurface*> importedSTL;
  Importer->GetImportedSTL(importedSTL);
  mafVMESurface *Data = importedSTL[0];
	Data->Update();

	mafOpConnectivitySurface *connectSurface = new mafOpConnectivitySurface;
  connectSurface->TestModeOn();
	connectSurface->SetInput(Data);
	connectSurface->OpRun();
  connectSurface->SetThresold(20.0);
  connectSurface->OnVtkConnect();
  Data->Update();

  mafString response = connectSurface->GetNumberOfExtractedSurfaces();
  CPPUNIT_ASSERT(response == mafString("2"));

	mafDEL(connectSurface);
	mafDEL(Importer);
	mafDEL(storage);
}
