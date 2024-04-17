/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterDicomTest
 Authors: Roberto Mucci
 
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
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>

#include "albaOpImporterDicomTest.h"
#include "albaOpImporterDicom.h"
#include "albaVMEGroup.h"
#include "albaVMEVolumeGray.h"

#include "vtkALBASmartPointer.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkRectilinearGrid.h"
#include "vtkLookupTable.h"
#include "vtkDirectory.h"

#include <wx/dir.h>
#include "wx/filename.h"



#define INCLUDE_CSTDLIB
#define INCLUDE_CSTRING



//-----------------------------------------------------------
void albaOpImporterDicomTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaOpImporterDicom *importer=new albaOpImporterDicom();
  albaDEL(importer);
}
//-----------------------------------------------------------
void albaOpImporterDicomTest::TestAccept() 
//-----------------------------------------------------------
{
  albaOpImporterDicom *importer=new albaOpImporterDicom();
  albaVMEGroup *group;
  albaNEW(group);
  
  CPPUNIT_ASSERT(importer->Accept(group));
  
  albaDEL(group);
  albaDEL(importer);
}
//-----------------------------------------------------------
void albaOpImporterDicomTest::TestCreateVolume() 
//-----------------------------------------------------------
{
  albaString dirName=ALBA_DATA_ROOT;
  dirName<<"/Dicom/";

  wxDir dir(dirName.GetCStr());
  wxString dicomDir;

  bool cont = dir.GetFirst(&dicomDir, "", wxDIR_DIRS);
  while ( cont )
  {
    if (dicomDir != "CVS")
    {
	    albaOpImporterDicom *importer=new albaOpImporterDicom();
      importer->TestModeOn();
	
	    wxString dicomPath = dirName + dicomDir;
	    importer->CreateSliceVTKPipeline();
	    importer->OpenDir(dicomPath.ToAscii());
	    importer->ImportDicomTags();
			importer->CalculateCropExtent();
	    importer->BuildVMEVolumeGrayOutput();
	
	    albaVME *VME=importer->GetOutput();
	    VME->Update();
	    CPPUNIT_ASSERT(VME!=NULL);
	
	    double sr[2];
	    if(VME->IsA("albaVMEVolumeGray"))
	    {
	      vtkRectilinearGrid *data=vtkRectilinearGrid::SafeDownCast(VME->GetOutput()->GetVTKData());
	      data->GetScalarRange(sr);
	    }
	    else 
	    {
	      CPPUNIT_ASSERT(VME->IsA("albaVMEImage"));
	      VME->GetOutput()->GetVTKData()->GetScalarRange(sr);
	    }
	       
	    CPPUNIT_ASSERT(sr[0]-sr[1] != 0);
	   
	    importer->OpStop(OP_RUN_OK);
	    albaDEL(importer);
	    VME = NULL;
    }

    cont = dir.GetNext(&dicomDir);
  }
}

//-----------------------------------------------------------
void albaOpImporterDicomTest::TestCompareDicomImage() 
//-----------------------------------------------------------
{
  double pixelValue = 0;
  std::vector<double> pixelVector;
  albaString dirName=ALBA_DATA_ROOT;
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
	
	    albaOpImporterDicom *importer=new albaOpImporterDicom();
	    importer->TestModeOn();
	    
	    importer->CreateSliceVTKPipeline();
	    importer->OpenDir(dicomPath.ToAscii());
	    importer->GenerateSliceTexture(0);
	
	    wxFileName::SplitPath(txtFilePath, &path, &short_name, &ext);
	    vtkImageData  *imageImported = importer->GetSliceInCurrentSeries(0);
	   
	    wxFileInputStream inputFile( txtFilePath );
	    wxTextInputStream text( inputFile );
	    wxString line = text.ReadLine();
	
	    do 
	    {
	      wxStringTokenizer tkz(line,wxT('\t'),wxTOKEN_RET_EMPTY_ALL);
	
	      while (tkz.HasMoreTokens())
	      {
	        pixelValue = atof(tkz.GetNextToken().ToAscii());
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
	    albaDEL(importer);
	    pixelVector.clear();
    }

    cont = dir.GetNext(&dicomDir);
  }
}

