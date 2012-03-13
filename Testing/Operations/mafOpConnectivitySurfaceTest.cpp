/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpConnectivitySurfaceTest.cpp,v $
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
  Data->GetOutput()->GetVTKData()->Update();
	Data->Update();

	mafOpConnectivitySurface *connectSurface = new mafOpConnectivitySurface;
  connectSurface->TestModeOn();
	connectSurface->SetInput(Data);
	connectSurface->OpRun();
  connectSurface->SetThresold(1.0);
  connectSurface->OnVtkConnect();
  Data->Update();

  mafString response = connectSurface->GetNumberOfExtractedSurfaces();
  CPPUNIT_ASSERT(response == mafString("2"));

	mafDEL(connectSurface);
	mafDEL(Importer);
	mafDEL(storage);
}
