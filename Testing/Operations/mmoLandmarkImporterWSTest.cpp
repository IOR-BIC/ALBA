/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmoLandmarkImporterWSTest.cpp,v $
Language:  C++
Date:      $Date: 2007-03-14 08:44:13 $
Version:   $Revision: 1.3 $
Authors:   Roberto Mucci
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h"
#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mmoLandmarkImporterWSTest.h"
#include "mmoLandmarkImporterWS.h"


#include "mafString.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMERoot.h"
#include "mafVMEStorage.h"

#include <string>
#include <assert.h>

#include <iostream>

//-----------------------------------------------------------
void mmoLandmarkImporterWSTest::TestWSImporter() 
//-----------------------------------------------------------
{
	mmoLandmarkImporterWS *importer=new mmoLandmarkImporterWS("importer");
	importer->TestModeOn();
	mafString filename=MED_DATA_ROOT;
  filename<<"/AL_FileTXT/pbCV1b05al.csv";
	importer->SetFileName(filename.GetCStr());
  importer->SetSkipColumn(0);
	importer->Read();
  mafVMELandmarkCloud *node=(mafVMELandmarkCloud *)importer->GetOutput();
  
  CPPUNIT_ASSERT(node->GetNumberOfLandmarks() == 32);
  cppDEL(importer);
}
//-----------------------------------------------------------
void mmoLandmarkImporterWSTest::TestVisibility() 
//-----------------------------------------------------------
{
  mmoLandmarkImporterWS *importer=new mmoLandmarkImporterWS("importer");
  importer->TestModeOn();
  mafString filename=MED_DATA_ROOT;
  filename<<"/AL_FileTXT/pbCV1b05al.csv";
  importer->SetFileName(filename.GetCStr());
  importer->SetSkipColumn(0);
  importer->Read();
  mafVMELandmarkCloud *node=(mafVMELandmarkCloud *)importer->GetOutput();

  bool visible = node->GetLandmarkVisibility(0,7.21);

  CPPUNIT_ASSERT(visible == false);
  cppDEL(importer);
}

//-----------------------------------------------------------
void mmoLandmarkImporterWSTest::TestCoordinates() 
//-----------------------------------------------------------
{
  mmoLandmarkImporterWS *importer=new mmoLandmarkImporterWS("importer");
  importer->TestModeOn();
  mafString filename=MED_DATA_ROOT;
  filename<<"/AL_FileTXT/pbCV1b05al.csv";
  importer->SetFileName(filename.GetCStr());
  importer->SetSkipColumn(0);
  importer->Read();
  mafVMELandmarkCloud *node=(mafVMELandmarkCloud *)importer->GetOutput();

  double xyz[3], rot[3];
  mafVMELandmark *lm = node->GetLandmark(0);
  lm->GetOutput()->GetPose(xyz,rot,2.94);

  CPPUNIT_ASSERT( fabs(xyz[0] - 36.110333) < 0.01 &&
    fabs(xyz[1] - -1012.215299) < 0.01 &&
    fabs(xyz[2] - 19.816351) < 0.01 );

  //CPPUNIT_ASSERT( mafEquals(xyz[0],144.13) && mafEquals(xyz[1],-1103.8) && mafEquals(xyz[2],1547.2));
  cppDEL(importer);
}


