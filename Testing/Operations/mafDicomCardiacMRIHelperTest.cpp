/*=========================================================================

 Program: MAF2Medical
 Module: mafDicomCardiacMRIHelperTest
 Authors: Stefano Perticoni
 
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
#include "mafDicomCardiacMRIHelperTest.h"

#include <cppunit/config/SourcePrefix.h>

#include "dcmtk/ofstd/ofstdinc.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"
#include "vnl/vnl_math.h"
#include <algorithm>
#include <map>
#include "mafDicomCardiacMRIHelper.h"

void mafDicomCardiacMRIHelperTest::setUp() 
{

}

void mafDicomCardiacMRIHelperTest::tearDown() 
{

}

void mafDicomCardiacMRIHelperTest::TestConstructorDestructor() 
{
  mafDicomCardiacMRIHelper *helper=new mafDicomCardiacMRIHelper();
  cppDEL(helper);
}

void mafDicomCardiacMRIHelperTest::TestDicomGeneralElectricPisa()
{
  mafDicomCardiacMRIHelper* helper = new mafDicomCardiacMRIHelper();

  // GE PISA (P09.rar to be extacted in target path)
  // Uncomment the following line and set your p09 dicom dir abs path 
  wxString dicomDir = "d:\\wip\\DicomConFetteAlContrario\\p09\\";

  helper->SetInputDicomDirectoryABSPath(dicomDir.c_str());
  helper->SetTestMode(true);
  helper->ParseDicomDirectory();
  
  cout << helper->GetFileNumberForPlaneIFrameJMatrix();
  cout << std::endl;

  cout << helper->GetFileNumberForPlaneIFrameJIdPlaneMatrix();
  cout << std::endl;

  cout << helper->GetPositionSingleFrameIdPlaneMatrix();
  cout << std::endl;

  cout << helper->GetXVersorsSingleFrameIdPlaneMatrix();
  cout << std::endl;

  cout << helper->GetYVersorsSingleFrameIdPlaneMatrix();
  cout << std::endl;

  cout << helper->GetImageSizeSingleFrameIdPlaneMatrix();
  cout << std::endl;

  cout << helper->GetNewPositionSingleFrameIdPlaneMatrix();
  cout << std::endl;

  cout << helper->GetNewXVersorsSingleFrameIdPlaneMatrix();
  cout << std::endl;

  cout << helper->GetNewYVersorsSingleFrameIdPlaneMatrix();
  cout << std::endl;

  cout << helper->GetNewImageSizeSingleFrameIdPlaneMatrix();
  cout << std::endl;

  cout << helper->GetRotateFlagIdPlaneMatrix();
  cout << std::endl;

  cout << helper->GetFlipLeftRightFlagIdPlaneMatrix();
  cout << std::endl;

  cout << helper->GetFlipUpDownFlagIdPlaneMatrix();
  cout << std::endl;
  
  cppDEL(helper);
}


void mafDicomCardiacMRIHelperTest::TestDicomSiemensNiguarda()
{
  mafDicomCardiacMRIHelper* helper = new mafDicomCardiacMRIHelper();

  // SIEMENS NIGUARDA (P20.rar to be extacted in target path)
  
  // Uncomment the following line and set your p20 dicom dir abs path 
  wxString dicomDir = "d:\\wip\\DicomConFetteAlContrario\\p20\\";

  helper->SetInputDicomDirectoryABSPath(dicomDir.c_str());
  helper->SetTestMode(true);
  helper->ParseDicomDirectory();

  cout << helper->GetFileNumberForPlaneIFrameJMatrix();
  cout << std::endl;

  cout << helper->GetFileNumberForPlaneIFrameJIdPlaneMatrix();
  cout << std::endl;

  cout << helper->GetPositionSingleFrameIdPlaneMatrix();
  cout << std::endl;

  cout << helper->GetXVersorsSingleFrameIdPlaneMatrix();
  cout << std::endl;

  cout << helper->GetYVersorsSingleFrameIdPlaneMatrix();
  cout << std::endl;

  cout << helper->GetImageSizeSingleFrameIdPlaneMatrix();
  cout << std::endl;

  cout << helper->GetNewPositionSingleFrameIdPlaneMatrix();
  cout << std::endl;

  cout << helper->GetNewXVersorsSingleFrameIdPlaneMatrix();
  cout << std::endl;

  cout << helper->GetNewYVersorsSingleFrameIdPlaneMatrix();
  cout << std::endl;

  cout << helper->GetNewImageSizeSingleFrameIdPlaneMatrix();
  cout << std::endl;

  cout << helper->GetRotateFlagIdPlaneMatrix();
  cout << std::endl;

  cout << helper->GetFlipLeftRightFlagIdPlaneMatrix();
  cout << std::endl;

  cout << helper->GetFlipUpDownFlagIdPlaneMatrix();
  cout << std::endl;

  cppDEL(helper);
}

void mafDicomCardiacMRIHelperTest::TestSetGetInputDicomDirectoryABSPath()
{
  mafDicomCardiacMRIHelper* helper = new mafDicomCardiacMRIHelper();
  CPPUNIT_ASSERT(helper->GetInputDicomDirectoryABSPath() == "UNDEFINED_m_InputDicomDirectoryABSPath");
  
  helper->SetInputDicomDirectoryABSPath("pippo");
  CPPUNIT_ASSERT(helper->GetInputDicomDirectoryABSPath() == "pippo");
  cppDEL(helper);
}