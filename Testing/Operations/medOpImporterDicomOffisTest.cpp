/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpImporterDicomOffisTest.cpp,v $
Language:  C++
Date:      $Date: 2009-05-05 08:13:00 $
Version:   $Revision: 1.1.2.1 $
Authors:   Roberto Mucci
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


#include <cppunit/config/SourcePrefix.h>
#include "medOpImporterDicomOffisTest.h"

#include "medOpImporterDicomOffis.h"
#include "mafVMEGroup.h"
#include "mafVMEVolumeGray.h"
#include "mafVMEPointSet.h"

#include "vtkMAFSmartPointer.h"
#include "vtkImageData.h"
//#include "vtkJPEGReader.h" 
//#include "vtkBMPReader.h"
//#include "vtkImageMathematics.h"
#include "vtkPointData.h"
#include "vtkRectilinearGrid.h"

#include <wx/dir.h>

//-----------------------------------------------------------
void medOpImporterDicomOffisTest::setUp() 
//-----------------------------------------------------------
{
}
//-----------------------------------------------------------
void medOpImporterDicomOffisTest::tearDown() 
//-----------------------------------------------------------
{
}
//-----------------------------------------------------------
void medOpImporterDicomOffisTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  medOpImporterDicomOffis *importer=new medOpImporterDicomOffis();
  mafDEL(importer);
}
//-----------------------------------------------------------
void medOpImporterDicomOffisTest::TestAccept() 
//-----------------------------------------------------------
{
  medOpImporterDicomOffis *importer=new medOpImporterDicomOffis();
  mafVMEGroup *group;
  mafNEW(group);
  
  CPPUNIT_ASSERT(importer->Accept(group));
  
  mafDEL(group);
  mafDEL(importer);

  delete wxLog::SetActiveTarget(NULL);
}
//-----------------------------------------------------------
void medOpImporterDicomOffisTest::TestSetDirName() 
//-----------------------------------------------------------
{
  medOpImporterDicomOffis *importer=new medOpImporterDicomOffis();
  char *dirName={"dir name"};
  importer->SetDirName(dirName);

  CPPUNIT_ASSERT(strcmp(importer->GetDirName(),dirName)==0);
  
  mafDEL(importer);

  delete wxLog::SetActiveTarget(NULL);
}

//-----------------------------------------------------------
void medOpImporterDicomOffisTest::TestCreateVolume() 
//-----------------------------------------------------------
{
  mafString dirName=MED_DATA_ROOT;
  dirName<<"/Dicom/";

  wxDir dir(dirName.GetCStr());
  wxString dicomDir;

  bool cont = dir.GetFirst(&dicomDir, "", wxDIR_DIRS);
  while ( cont )
  {
    medOpImporterDicomOffis *importer=new medOpImporterDicomOffis();
    importer->TestModeOn();

    wxString dicomPath = dirName + dicomDir;
    importer->SetDirName(dicomPath.c_str());
    importer->CreatePipeline();
    importer->OpenDir();
    importer->ReadDicom();
    importer->CreateSlice(0);
    importer->BuildVolume();

    mafVME *VME=mafVME::SafeDownCast(importer->GetOutput());
    VME->Update();
    CPPUNIT_ASSERT(VME!=NULL);

    double sr[2];
    if(VME->IsA("mafVMEVolumeGray"))
    {
      vtkRectilinearGrid *data=vtkRectilinearGrid::SafeDownCast(VME->GetOutput()->GetVTKData());
      data->UpdateData();
      data->GetScalarRange(sr);
    }
    else 
    {
      CPPUNIT_ASSERT(VME->IsA("mafVMEImage"));
      VME->GetOutput()->GetVTKData()->UpdateData();
      VME->GetOutput()->GetVTKData()->GetScalarRange(sr);
    }
       
    CPPUNIT_ASSERT(sr[0]-sr[1] != 0);
   
    importer->OpStop(OP_RUN_OK);
    mafDEL(importer);
    VME = NULL;

    cont = dir.GetNext(&dicomDir);
  }

  delete wxLog::SetActiveTarget(NULL);
}
