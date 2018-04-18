/*=========================================================================

 Program: MAF2
 Module: mafOpImporterDicomTest
 Authors: Roberto Mucci
 
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
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>

#include "mafOpImporterDicomTest.h"
#include "mafOpImporterDicom.h"
#include "mafVMEGroup.h"
#include "mafVMEVolumeGray.h"
#include "mafVMEPointSet.h"

#include "vtkMAFSmartPointer.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkRectilinearGrid.h"
#include "vtkLookupTable.h"
#include "vtkDirectory.h"

#include <wx/dir.h>



#define INCLUDE_CSTDLIB
#define INCLUDE_CSTRING



//-----------------------------------------------------------
void mafOpImporterDicomTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  mafOpImporterDicom *importer=new mafOpImporterDicom();
  mafDEL(importer);
}
//-----------------------------------------------------------
void mafOpImporterDicomTest::TestAccept() 
//-----------------------------------------------------------
{
  mafOpImporterDicom *importer=new mafOpImporterDicom();
  mafVMEGroup *group;
  mafNEW(group);
  
  CPPUNIT_ASSERT(importer->Accept(group));
  
  mafDEL(group);
  mafDEL(importer);
}
//-----------------------------------------------------------
void mafOpImporterDicomTest::TestCreateVolume() 
//-----------------------------------------------------------
{
  mafString dirName=MAF_DATA_ROOT;
  dirName<<"/Dicom/";

  wxDir dir(dirName.GetCStr());
  wxString dicomDir;

  bool cont = dir.GetFirst(&dicomDir, "", wxDIR_DIRS);
  while ( cont )
  {
    if (dicomDir != "CVS")
    {
	    mafOpImporterDicom *importer=new mafOpImporterDicom();
      importer->TestModeOn();
	
	    wxString dicomPath = dirName + dicomDir;
	    importer->CreateSliceVTKPipeline();
	    importer->OpenDir(dicomPath.c_str());
	    importer->ImportDicomTags();
			importer->CalculateCropExtent();
	    importer->BuildVMEVolumeGrayOutput();
	
	    mafVME *VME=importer->GetOutput();
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
    }

    cont = dir.GetNext(&dicomDir);
  }
}

//-----------------------------------------------------------
void mafOpImporterDicomTest::TestCompareDicomImage() 
//-----------------------------------------------------------
{
  double pixelValue = 0;
  std::vector<double> pixelVector;
  mafString dirName=MAF_DATA_ROOT;
  dirName<<"/Dicom/";

  wxDir dir(dirName.GetCStr());
  wxString dicomDir;

  bool cont = dir.GetFirst(&dicomDir, "", wxDIR_DIRS);
  while ( cont )
  {
    if (dicomDir != "CVS")
    {
	    //find the .txt file
	    wxString name, path, short_name, ext;
	    wxString dicomPath = dirName + dicomDir;
	    wxDir dirDicom(dicomPath);
	    wxArrayString files;
	    wxString extension = "txt";
	    const wxString FileSpec = "*" + extension;
	
	    if (dicomPath != wxEmptyString && wxDirExists(dicomPath))
	    {
	      wxDir::GetAllFiles(dicomPath, &files, FileSpec);
	    }
	    CPPUNIT_ASSERT(files.GetCount() == 1);
	    wxString txtFilePath = files[0];
	
	    mafOpImporterDicom *importer=new mafOpImporterDicom();
	    importer->TestModeOn();
	    
	    importer->CreateSliceVTKPipeline();
	    importer->OpenDir(dicomPath.c_str());
	    importer->GenerateSliceTexture(0);
	
	    wxSplitPath(txtFilePath, &path, &short_name, &ext);
	    vtkImageData  *imageImported = importer->GetSliceInCurrentSeries(0);
	   
	    wxFileInputStream inputFile( txtFilePath );
	    wxTextInputStream text( inputFile );
	    wxString line = text.ReadLine();
	
	    do 
	    {
	      wxStringTokenizer tkz(line,wxT('\t'),wxTOKEN_RET_EMPTY_ALL);
	
	      while (tkz.HasMoreTokens())
	      {
	        pixelValue = atof(tkz.GetNextToken().c_str());
	        pixelVector.push_back(pixelValue);
	      }
	      line = text.ReadLine();
	
	    } while (!inputFile.Eof());
	
	    CPPUNIT_ASSERT(imageImported->GetNumberOfPoints() == pixelVector.size());
	
	    bool idem = true;
	    for(int i=0 ; i<imageImported->GetNumberOfPoints();i++)
	    {
	      idem = imageImported->GetPointData()->GetScalars()->GetTuple1(i) == pixelVector[i];
	      if (!idem)
	      {
	        break;
	      }
	      CPPUNIT_ASSERT(imageImported->GetPointData()->GetScalars()->GetTuple1(i) == pixelVector[i]);
	    }
	
	    importer->OpStop(OP_RUN_OK);
	    mafDEL(importer);
	    pixelVector.clear();
    }

    cont = dir.GetNext(&dicomDir);
  }
}

