/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpImporterDicomGrassrootsTest.cpp,v $
Language:  C++
Date:      $Date: 2009-04-08 14:03:07 $
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
#include "medOpImporterDicomGrassrootsTest.h"

#include "medOpImporterDicomGrassroots.h"
#include "mafVMEGroup.h"
#include "mafVMEVolumeGray.h"
#include "mafVMEPointSet.h"

#include "vtkMAFSmartPointer.h"
#include "vtkImageData.h"
#include "vtkJPEGReader.h" 
#include "vtkBMPReader.h"
#include "vtkImageMathematics.h"
#include "vtkPointData.h"
#include "vtkRectilinearGrid.h"

#include <wx/dir.h>

//-----------------------------------------------------------
void medOpImporterDicomGrassrootsTest::setUp() 
//-----------------------------------------------------------
{
}
//-----------------------------------------------------------
void medOpImporterDicomGrassrootsTest::tearDown() 
//-----------------------------------------------------------
{
}
//-----------------------------------------------------------
void medOpImporterDicomGrassrootsTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  medOpImporterDicomGrassroots *importer=new medOpImporterDicomGrassroots();
  mafDEL(importer);
}
//-----------------------------------------------------------
void medOpImporterDicomGrassrootsTest::TestAccept() 
//-----------------------------------------------------------
{
  medOpImporterDicomGrassroots *importer=new medOpImporterDicomGrassroots();
  mafVMEGroup *group;
  mafNEW(group);
  
  CPPUNIT_ASSERT(importer->Accept(group));
  
  mafDEL(group);
  mafDEL(importer);

  delete wxLog::SetActiveTarget(NULL);
}
//-----------------------------------------------------------
void medOpImporterDicomGrassrootsTest::TestSetDirName() 
//-----------------------------------------------------------
{
  medOpImporterDicomGrassroots *importer=new medOpImporterDicomGrassroots();
  char *dirName={"dir name"};
  importer->SetDirName(dirName);

  CPPUNIT_ASSERT(strcmp(importer->GetDirName(),dirName)==0);
  
  mafDEL(importer);

  delete wxLog::SetActiveTarget(NULL);
}
//-----------------------------------------------------------
void medOpImporterDicomGrassrootsTest::TestCompareDicomImage() 
//-----------------------------------------------------------
{
  medOpImporterDicomGrassroots *importer=new medOpImporterDicomGrassroots();
  importer->TestModeOn();
  mafString dirName=MED_DATA_ROOT;
  dirName<<"/Dicom/";

  wxDir dir(dirName.GetCStr());
  wxString dicomDir;

  bool cont = dir.GetFirst(&dicomDir, "", wxDIR_DIRS);
  while ( cont )
  {
    wxString dicomPath = dirName + dicomDir;
    importer->CreatePipeline();
    importer->ImportDicom(dicomPath.c_str());
    importer->ReadDicom();

    vtkMAFSmartPointer<vtkImageData> originalImage = importer->GetSliceImageData(0);


    //read original Image
    vtkJPEGReader *rO;
    vtkNEW(rO);
    wxString fileName = dicomPath;
    fileName.Append("/test.jpg");

    rO->SetFileName(fileName.c_str());
    rO->Update();

    vtkImageData *BMPImage = rO->GetOutput();


    // IMPORT BMP TO BE COMPARED
    /*wxString fileName = dicomPath;
    fileName.Append("/test.bmp");
    vtkMAFSmartPointer<vtkBMPReader> r;
    r->SetFileName(fileName.c_str());
    r->Allow8BitBMPOn();
    r->SetDataScalarTypeToDouble();
    r->Update();

    vtkMAFSmartPointer<vtkImageData> BMPImage  = r->GetOutput();
    //create vtkImageData from zero and set the correct parameters (spacing, dimension) ...
//    vtkImageData *originalImage = vtkImageData::New();
    BMPImage->SetSpacing(r->GetOutput()->GetSpacing());
    BMPImage->SetDimensions(r->GetOutput()->GetDimensions());
    BMPImage->AllocateScalars();

    // and scalars
    for(int i=0; i<BMPImage->GetNumberOfPoints();i++)
    {
      BMPImage->GetPointData()->GetScalars()->SetTuple1(i, r->GetOutput()->GetPointData()->GetScalars()->GetTuple1(i));
    }
    BMPImage->Update();*/

    //SetOperationToSubtract subtract one image from the other one: the result 
    //image must be a black image, with all the scalars set to zero
    vtkMAFSmartPointer<vtkImageMathematics> imageMath;
    imageMath->AddInput(originalImage);
    imageMath->AddInput(BMPImage);
    imageMath->SetOperationToSubtract();
    imageMath->Update();

    double sr[2] = {-1,1};
    imageMath->GetOutput()->GetPointData()->GetScalars()->GetRange(sr);

    CPPUNIT_ASSERT(mafEquals(sr[0],0.0));
    CPPUNIT_ASSERT(mafEquals(sr[1],0.0));

    cont = dir.GetNext(&dicomDir);
  }

  mafDEL(importer);

  delete wxLog::SetActiveTarget(NULL);
}
//-----------------------------------------------------------
void medOpImporterDicomGrassrootsTest::TestCreateVolume() 
//-----------------------------------------------------------
{
  mafString dirName=MED_DATA_ROOT;
  dirName<<"/Dicom/";

  wxDir dir(dirName.GetCStr());
  wxString dicomDir;

  bool cont = dir.GetFirst(&dicomDir, "", wxDIR_DIRS);
  while ( cont )
  {
    medOpImporterDicomGrassroots *importer=new medOpImporterDicomGrassroots();
    importer->TestModeOn();

    wxString dicomPath = dirName + dicomDir;
    importer->CreatePipeline();
    importer->ImportDicom(dicomPath.c_str());
    importer->ReadDicom();
    importer->CreateSlice(0);
    importer->BuildVolume();

    //mafVMEVolumeGray *volume = mafVMEVolumeGray::SafeDownCast(importer->GetOutput());
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
