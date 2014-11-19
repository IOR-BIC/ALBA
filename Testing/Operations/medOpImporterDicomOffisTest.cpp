/*=========================================================================

 Program: MAF2Medical
 Module: medOpImporterDicomOffisTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>

#include "medOpImporterDicomOffisTest.h"

#include "medOpImporterDicomOffis.h"
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

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#if _MSC_VER >= 1500
#undef VS2008_ssize_t_HACK
#endif

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcdebug.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcuid.h"       /* for dcmtk version name */
#include "dcmtk/dcmdata/dcistrmz.h"    /* for dcmZlibExpectRFC1950Encoding */
#include "dcmtk/dcmimgle/dcmimage.h."
#include "dcmtk/dcmjpeg/djdecode.h."
#include "dcmtk/dcmdata/dcrledrg.h"

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTRING

#include "dcmtk/ofstd/ofstdinc.h"
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
  importer->SetDicomDirectoryABSFileName(dirName);

  CPPUNIT_ASSERT(strcmp(importer->GetDicomDirectoryABSFileName(),dirName)==0);
  
  mafDEL(importer);

  delete wxLog::SetActiveTarget(NULL);
}

//-----------------------------------------------------------
void medOpImporterDicomOffisTest::TestCreateVolume() 
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
	    medOpImporterDicomOffis *importer=new medOpImporterDicomOffis();
      importer->TestModeOn();
	
	    wxString dicomPath = dirName + dicomDir;
	    importer->SetDicomDirectoryABSFileName(dicomPath.c_str());
	    importer->CreateSliceVTKPipeline();
	    importer->OpenDir();
	    importer->ReadDicom();
	    importer->GenerateSliceTexture(0);
	    importer->BuildOutputVMEGrayVolumeFromDicom();
	
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
    }

    cont = dir.GetNext(&dicomDir);
  }

  delete wxLog::SetActiveTarget(NULL);
}

//-----------------------------------------------------------
void medOpImporterDicomOffisTest::TestCompareDicomImage() 
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
	
	    medOpImporterDicomOffis *importer=new medOpImporterDicomOffis();
	    importer->TestModeOn();
	    
	    importer->SetDicomDirectoryABSFileName(dicomPath.c_str());
	    importer->CreateSliceVTKPipeline();
	    importer->OpenDir();
	    importer->ReadDicom();
	    importer->GenerateSliceTexture(0);
	
	    wxSplitPath(txtFilePath, &path, &short_name, &ext);
	    vtkMAFSmartPointer<vtkImageData> imageImported = importer->GetSliceImageDataFromLocalDicomFileName(short_name);
	   
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

  delete wxLog::SetActiveTarget(NULL);
}

