/*=========================================================================

 Program: MAF2
 Module: mafVMEVolumeLargeTest
 Authors: Alberto Losi
 
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

#include "mafOpImporterRAWVolume_BES.h"
#include "vtkMAFLargeImageData.h"

#include "mafVMEVolumeLargeTest.h"
#include "mafVMEVolumeLarge.h"


//---------------------------------------------------------
void mafVMEVolumeLargeTest::TestDynamicAllocation()
//---------------------------------------------------------
{
  mafVMEVolumeLarge *volume = NULL;
  mafNEW(volume);
  mafDEL(volume);
}
//---------------------------------------------------------
void mafVMEVolumeLargeTest::TestGetIcon()
//---------------------------------------------------------
{
  mafVMEVolumeLarge *volume = NULL;
  mafNEW(volume);

  char **icon = volume->GetIcon();
  CPPUNIT_ASSERT(icon != NULL);

  mafDEL(volume);
}
//---------------------------------------------------------
void mafVMEVolumeLargeTest::TestSetLargeData()
//---------------------------------------------------------
{
//   mafString filename = MAF_DATA_ROOT;
//   filename << "/Test_VolumeLarge/testVolume.raw";
// 
//   mafOpImporterRAWVolume_BES *importer = new mafOpImporterRAWVolume_BES();
//   importer->TestModeOn();
//   importer->SetFileName(filename);
//   importer->Import();
//  
//   mafVMEGeneric *output = (mafVMEGeneric*)importer->GetOutput();
//   vtkMAFLargeImageData *data = (vtkMAFLargeImageData*)output->GetOutput()->GetVTKData();
// 
//   mafVMEVolumeLarge *volume = NULL;
//   mafNEW(volume);
// 
//   volume->SetLargeData(data,-1);
// 
//   mafDEL(volume);
//   cppDEL(importer);

  mafString filename = MAF_DATA_ROOT;
  mafString dirname = MAF_DATA_ROOT;

  filename << "/Test_VolumeLarge/testVolume.bbf";
  dirname << "/Test_VolumeLarge/";

  mafVolumeLargeReader *reader = new mafVolumeLargeReader();
  reader->SetFileName(filename);

  reader->SetMemoryLimit(16 * 1024);

  int VOI[6];
  VOI[0] = 0;
  VOI[1] = 1;
  VOI[2] = 0;
  VOI[3] = 1;
  VOI[4] = 0;
  VOI[5] = 1;
  reader->SetVOI(VOI);

  reader->Update();

  mafVMEVolumeLarge *volume = NULL;
  mafNEW(volume);

  volume->SetFileName(dirname);
  CPPUNIT_ASSERT(volume->SetLargeData(reader) == MAF_OK);

  mafDEL(volume);
}
//---------------------------------------------------------
void mafVMEVolumeLargeTest::TestSetGetFileNameTest()
//---------------------------------------------------------
{
  mafString filename = MAF_DATA_ROOT;
  filename << "/Test_VolumeLarge/";

  mafVMEVolumeLarge *volume = NULL;
  mafNEW(volume);

  volume->SetFileName(filename);
  CPPUNIT_ASSERT(filename.Equals(volume->GetFileName()));

  mafDEL(volume);
}

//---------------------------------------------------------
void mafVMEVolumeLargeTest::TestUnRegister()
//---------------------------------------------------------
{
  mafVMEVolumeLarge *volume = NULL;
  mafNEW(volume);

  volume->Register(NULL);

  volume->UnRegister(NULL);
  CPPUNIT_ASSERT(volume->GetReferenceCount() == 1);

  mafDEL(volume);
}
