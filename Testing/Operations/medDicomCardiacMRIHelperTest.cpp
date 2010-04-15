/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medDicomCardiacMRIHelperTest.cpp,v $
Language:  C++
Date:      $Date: 2010-04-15 11:20:30 $
Version:   $Revision: 1.1.2.2 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#include "mafDefines.h"
#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "medDicomCardiacMRIHelperTest.h"

#include <cppunit/config/SourcePrefix.h>

#include "dcmtk/ofstd/ofstdinc.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"
#include "vnl/vnl_math.h"
#include <algorithm>
#include <map>
#include "medDicomCardiacMRIHelper.h"

void medDicomCardiacMRIHelperTest::setUp() 
{

}

void medDicomCardiacMRIHelperTest::tearDown() 
{

}

void medDicomCardiacMRIHelperTest::TestConstructorDestructor() 
{
  medDicomCardiacMRIHelper *helper=new medDicomCardiacMRIHelper();
  cppDEL(helper);
}

void medDicomCardiacMRIHelperTest::TestDicomGeneralElectricPisa()
{
  medDicomCardiacMRIHelper* helper = new medDicomCardiacMRIHelper();

  // GE PISA (P09.rar to be extacted in target path)
  // Uncomment the following line and set your p09 dicom dir abs path 
  wxString dicomDir = "d:\\wip\\DicomConFetteAlContrario\\p09\\";

  helper->SetInputDicomDirectoryABSPath(dicomDir.c_str());
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


void medDicomCardiacMRIHelperTest::TestDicomSiemensNiguarda()
{
  medDicomCardiacMRIHelper* helper = new medDicomCardiacMRIHelper();

  // SIEMENS NIGUARDA (P20.rar to be extacted in target path)
  
  // Uncomment the following line and set your p20 dicom dir abs path 
  wxString dicomDir = "d:\\wip\\DicomConFetteAlContrario\\p20\\";

  helper->SetInputDicomDirectoryABSPath(dicomDir.c_str());
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

void medDicomCardiacMRIHelperTest::TestSetGetInputDicomDirectoryABSPath()
{
  medDicomCardiacMRIHelper* helper = new medDicomCardiacMRIHelper();
  CPPUNIT_ASSERT(helper->GetInputDicomDirectoryABSPath() == "UNDEFINED_m_InputDicomDirectoryABSPath");
  
  helper->SetInputDicomDirectoryABSPath("pippo");
  CPPUNIT_ASSERT(helper->GetInputDicomDirectoryABSPath() == "pippo");
  cppDEL(helper);
}