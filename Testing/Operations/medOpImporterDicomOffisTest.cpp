/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpImporterDicomOffisTest.cpp,v $
Language:  C++
Date:      $Date: 2010-04-01 16:16:16 $
Version:   $Revision: 1.1.2.11 $
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
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"
#include "vnl/vnl_math.h"
#include <algorithm>
#include <map>
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

//-----------------------------------------------------------
void medOpImporterDicomOffisTest::TestCompareDicomImage() 
//-----------------------------------------------------------
{
  double pixelValue = 0;
  std::vector<double> pixelVector;
  mafString dirName=MED_DATA_ROOT;
  dirName<<"/Dicom/";

  wxDir dir(dirName.GetCStr());
  wxString dicomDir;

  bool cont = dir.GetFirst(&dicomDir, "", wxDIR_DIRS);
  while ( cont )
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
    
    importer->SetDirName(dicomPath.c_str());
    importer->CreatePipeline();
    importer->OpenDir();
    importer->ReadDicom();
    importer->CreateSlice(0);

    wxSplitPath(txtFilePath, &path, &short_name, &ext);
    vtkMAFSmartPointer<vtkImageData> imageImported = importer->GetFirstSlice(short_name);
   
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

    cont = dir.GetNext(&dicomDir);
  }

  delete wxLog::SetActiveTarget(NULL);
}

//-----------------------------------------------------------
void medOpImporterDicomOffisTest::TestMatlabScriptConverter() 
//-----------------------------------------------------------
{

  medOpImporterDicomOffis *importer=new medOpImporterDicomOffis();
  importer->TestModeOn();

  // SIEMENS NIGUARDA
  wxString dicomDir = "d:\\wip\\DicomConFetteAlContrario\\p20\\";
  
  // GE PISA
  //wxString dicomDir = "d:\\wip\\DicomConFetteAlContrario\\p09\\";

  CPPUNIT_ASSERT(wxDirExists(dicomDir));
  vtkDirectory *directoryReader = vtkDirectory::New();
  directoryReader->Open(dicomDir);
 
  wxString fileName = "UNSET";

  vector<string> dicomLocalFileNamesVector;

  int firstDicomSliceFileIndex = -1;

  for (int i=0; i < directoryReader->GetNumberOfFiles(); i++)
	{
		if ((strcmp(directoryReader->GetFile(i),".") == 0) || \
        (strcmp(directoryReader->GetFile(i),"..") == 0)) 
		{
			continue;
		}
		else
		{
      fileName = directoryReader->GetFile(i);
      dicomLocalFileNamesVector.push_back(fileName.c_str());
    }
  }
 
//   for (int i = 0; i < dicomLocalFileNamesVector.size(); i++) 
//   {
//     cout << std::endl;
//     cout << dicomLocalFileNamesVector[i];
//   }
//   
  //CPPUNIT_ASSERT(file1.compare("25292865") == 0);

  int numberOfFiles = directoryReader->GetNumberOfFiles();
  
  DcmFileFormat dicomFileHandler;    

  wxString dicomFileName1 = dicomLocalFileNamesVector[0].c_str();
  wxString file1ABSFileName = dicomDir + dicomFileName1; 

  CPPUNIT_ASSERT(wxFileExists(file1ABSFileName));

  OFCondition status = dicomFileHandler.loadFile(file1ABSFileName);

  CPPUNIT_ASSERT(status.good());

  DcmDataset *dicomDataset = dicomFileHandler.getDataset();

  long int dcmCardiacNumberOfImages = -1;
  dicomDataset->findAndGetLongInt(DCM_CardiacNumberOfImages,dcmCardiacNumberOfImages);

  //CPPUNIT_ASSERT(dcmCardiacNumberOfImages == 40);

  // N
  long int timeFrames = dcmCardiacNumberOfImages;

  // S
  int planesPerFrame = directoryReader->GetNumberOfFiles() / timeFrames;

  //CPPUNIT_ASSERT(S == 18);

  
  //   spacing = info.PixelSpacing;
  // 

  double spacing[3];
  spacing[2] = 0;

  dicomDataset->findAndGetFloat64(DCM_PixelSpacing,spacing[0],0);
  //CPPUNIT_ASSERT(spacing[0] == 1.562500000000000);

  dicomDataset->findAndGetFloat64(DCM_PixelSpacing,spacing[1],1);
  //CPPUNIT_ASSERT(spacing[1] == 1.562500000000000);


  //   % inizializzazione
  //     % Vx = versori x
  //     % Vy = versory y
  //     % P  = posizione
  //     % s  = image size
  //     % f  = frame
  //     Vx = zeros(N*S,3);
  //   Vy = zeros(N*S,3);
  //   P  = zeros(N*S,3);
  //   s  = zeros(N*S,2);
  //   f  = zeros(N*S,1);

  vnl_matrix<double> xVersors(timeFrames*planesPerFrame, 3, 0.0);
  vnl_matrix<double> yVersors(timeFrames*planesPerFrame, 3, 0.0);
  vnl_matrix<double> positions(timeFrames*planesPerFrame, 3, 0.0);
  vnl_matrix<double> imageSize(timeFrames*planesPerFrame, 2, 0.0);
  vnl_matrix<double> frame(timeFrames*planesPerFrame, 1, 0.0);


  for (int i = 0; i < timeFrames*planesPerFrame; i++) 
  {

    wxString currentDicomSliceFileName = dicomLocalFileNamesVector[i].c_str();

    wxString currentSliceABSFileName = dicomDir + currentDicomSliceFileName;

    CPPUNIT_ASSERT(wxFileExists(currentSliceABSFileName));

    OFCondition status = dicomFileHandler.loadFile(currentSliceABSFileName);
    
    DcmDataset *dicomDataset = dicomFileHandler.getDataset();

    double dcmImagePositionPatient[3] = {-9999, -9999, -9999};
    dicomDataset->findAndGetFloat64(DCM_ImagePositionPatient,dcmImagePositionPatient[0],0);
    dicomDataset->findAndGetFloat64(DCM_ImagePositionPatient,dcmImagePositionPatient[1],1);
    dicomDataset->findAndGetFloat64(DCM_ImagePositionPatient,dcmImagePositionPatient[2],2);

    double dcmImageOrientationPatient[9] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
    dicomDataset->findAndGetFloat64(DCM_ImageOrientationPatient,dcmImageOrientationPatient[0],0);
    dicomDataset->findAndGetFloat64(DCM_ImageOrientationPatient,dcmImageOrientationPatient[1],1);
    dicomDataset->findAndGetFloat64(DCM_ImageOrientationPatient,dcmImageOrientationPatient[2],2);
    dicomDataset->findAndGetFloat64(DCM_ImageOrientationPatient,dcmImageOrientationPatient[3],3);
    dicomDataset->findAndGetFloat64(DCM_ImageOrientationPatient,dcmImageOrientationPatient[4],4);
    dicomDataset->findAndGetFloat64(DCM_ImageOrientationPatient,dcmImageOrientationPatient[5],5);

    long int dcmColumns;
    dicomDataset->findAndGetLongInt(DCM_Columns, dcmColumns);
  
    long int dcmRows;
    dicomDataset->findAndGetLongInt(DCM_Rows, dcmRows);

    long int dcmInstanceNumber;
    dicomDataset->findAndGetLongInt(DCM_InstanceNumber,dcmInstanceNumber);

    for (int j = 0; j < 3; j++) 
    {
      positions(i,j) = dcmImagePositionPatient[j];
      xVersors(i,j) = dcmImageOrientationPatient[j];
      yVersors(i,j) = dcmImageOrientationPatient[j+3];
    }
    
    imageSize(i, 0) = dcmColumns;
    imageSize(i, 1) = dcmRows;

    frame(i,0) = dcmInstanceNumber;
  }
  
  cout<< positions(0,0);

  double diff = fabs(positions(0,0) - 191.8841);
  //CPPUNIT_ASSERT(diff<.0001);

  diff = fabs(positions(0,1) - (- 195.6752));
  //CPPUNIT_ASSERT(diff<.0001);

  diff = fabs(positions(0,2) - (75.2316));
  //CPPUNIT_ASSERT(diff<.0001);

  cout << positions(719,0);
  diff = fabs(positions(719,0) - ( -7.5477));
  //CPPUNIT_ASSERT(diff<.0001);

  diff = fabs(positions(719,1) - (136.5524));
  //CPPUNIT_ASSERT(diff<.0001);

  diff = fabs(positions(719,2) - (204.6599));
  //CPPUNIT_ASSERT(diff<.0001);

  diff = fabs(xVersors(0,0) - (-0.6247));
  //CPPUNIT_ASSERT(diff<.0001);

  diff = fabs(xVersors(0,1) - (0.6782));
  //CPPUNIT_ASSERT(diff<.0001);

  diff = fabs(xVersors(0,2) - (0.3871));
  //CPPUNIT_ASSERT(diff<.0001);

  // 0.6255   -0.6768   -0.3881
  diff = fabs(xVersors(719,0) - (0.6255));
  //CPPUNIT_ASSERT(diff<.0001);

  diff = fabs(xVersors(719,1) - (-0.6768));
  //CPPUNIT_ASSERT(diff<.0001);

  diff = fabs(xVersors(719,2) - (-0.3881));
  //CPPUNIT_ASSERT(diff<.0001);

  // -0.3197    0.2301   -0.9191
  diff = fabs(yVersors(0,0) - (-0.3197));
  //CPPUNIT_ASSERT(diff<.0001);

  diff = fabs(yVersors(0,1) - (0.2301));
  //CPPUNIT_ASSERT(diff<.0001);

  diff = fabs(yVersors(0,2) - (-0.9191));
  //CPPUNIT_ASSERT(diff<.0001);

  //  -0.6312   -0.1467   -0.7616
  diff = fabs(yVersors(719,0) - ( -0.6312));
  //CPPUNIT_ASSERT(diff<.0001);

  diff = fabs(yVersors(719,1) - (-0.1467));
  //CPPUNIT_ASSERT(diff<.0001);

  diff = fabs(yVersors(719,2) - (-0.7616));
  //CPPUNIT_ASSERT(diff<.0001);


  //   %% MAPPING PIANO-FRAME
  // 
  //     % flag_img(i,j) indica il numero file del piano i, frame j
  //     flag_img = zeros(S,N);
  // 

//   % - flag_img: contiene l'indice dell'immagine per ogni coordinata
//     %   piano-frame (eg: se flag_img(i,j) == num_img, allora
//     %   filelist(num_img).name è il nome file che contiente l'immagine del piano i nel frame j
  vnl_matrix<double> fileNumberForPlaneIFrameJ(planesPerFrame,timeFrames,0);

  const char *dcmManufacturer = "?";
  dicomDataset->findAndGetString(DCM_Manufacturer, dcmManufacturer);

  cout << std::endl;
  cout << frame;
  cout << std::endl;

  // QUI

  // SIEMENS for P20
  cout<<dcmManufacturer<<std::endl;

  if (strcmp(dcmManufacturer, "SIEMENS")==0)
  {
    //   % SIEMENS - NIGUARDA
    //     if strcmp(info.Manufacturer,'SIEMENS')
    //       % indici del primo frame
    //       id_fframe = find(f==1)';
    //       flag_img(:,1) = id_fframe;

    // find f rows with value == 1

    // 1 x 18 double
    vector<double> id_frame;

    for (int i = 0; i < frame.rows(); i++) 
    {
      if (frame(i,0) == 1)
      {
        id_frame.push_back(i);
      }
    }
    
    //CPPUNIT_ASSERT(id_frame.size() == 18);
    //CPPUNIT_ASSERT(id_frame[0] == 655);

    for (int i = 0; i < fileNumberForPlaneIFrameJ.rows(); i++) 
    {
      fileNumberForPlaneIFrameJ(i,0) = id_frame[i];
    }

    //CPPUNIT_ASSERT(flag_img(0,0)==655);
    //CPPUNIT_ASSERT(flag_img(17,0)==672);

    // 
    // 
    //   % cerco per confronto tutte le immagini appartenenti al piano i-esimo
    //     for i = 1:S
    //       Vtemp  = repmat([Vx(id_fframe(i),:) Vy(id_fframe(i),:)],[N*S 1]);


    //   K>> [Vx(656,:) Vy(656,:)]
    // 
    //   ans =
    // 
    //     0.7756    0.5820    0.2444   -0.6277    0.6705    0.3955

    // Vtemp =  
    //     0.775604991058340	0.581979977825110	0.244409908260850	-0.627725681213100	0.670468722566660	0.395515058127140
    //     0.775604991058340	0.581979977825110	0.244409908260850	-0.627725681213100	0.670468722566660	0.395515058127140
    //     0.775604991058340	0.581979977825110	0.244409908260850	-0.627725681213100	0.670468722566660	0.395515058127140
    //     0.775604991058340	0.581979977825110	0.244409908260850	-0.627725681213100	0.670468722566660	0.395515058127140
    //     ....

    // build VxVy
    vnl_matrix<double> VxVy(timeFrames*planesPerFrame, 6);
    for (int i = 0; i < timeFrames*planesPerFrame; i++) 
    {
      for (int j = 0; j < 3; j++) 
      {
        VxVy(i,j) = xVersors(i,j);
        VxVy(i,j+3) = yVersors(i,j);
      }
    }
    
    // cout << VxVy;

    for (int i = 0; i < planesPerFrame; i++) 
    {
      // build Vtemp(i)
      vnl_matrix<double> Vtemp(timeFrames*planesPerFrame, 6);
      for (int row = 0; row < timeFrames*planesPerFrame; row++) 
      {
        for (int col = 0; col < 3; col++) 
        {
          Vtemp(row, col) = xVersors(id_frame[i],col);
          Vtemp(row, col +3) = yVersors(id_frame[i],col);
        }      
      }

      vector<double> id_plane;

      for (int a = 0; a < timeFrames*planesPerFrame; a++) 
      {
        if (Vtemp.get_row(a) == VxVy.get_row(a))
        {
          id_plane.push_back(a);
        }
      }
      
//       for (int i = 0; i < id_plane.size() ; i++) 
//       {    
//         cout << id_plane[i] << std::endl;
//       }

      for (int c = 0; c < id_plane.size(); c++) 
      {
        int k = id_plane[c];

//         cout << "k: " << k << std::endl; 
//         cout << "i: " << i << std::endl;
//         cout << "f(k,0): " << f(k,0) << std::endl;

        fileNumberForPlaneIFrameJ(i, frame(k,0) - 1 ) = k;
        assert(true);
      }
      assert(true);
    } //end for (int i = 0; i < S; i++) 
  } //end if SIEMENS
  else
  {

    //   %GE MEDICAL SYSTEMS - PISA    
    //   else 
    //   [dummy id_frameplane] = sort(f);
    //   flag_img = reshape(id_frameplane,[N S]);
    //   flag_img = flag_img';
    //     end
    
    std::map<int,int> idFramePlane_Dummy_Map;

    for (int i = 0; i < frame.rows() ; i++) 
    {
      idFramePlane_Dummy_Map[frame(i,0)] = i;
    }

    std::map<int,int>::iterator mapIterator;

    cout << std::endl;

    vector<double> idFramePlane;

    for(mapIterator = idFramePlane_Dummy_Map.begin() ; mapIterator!=idFramePlane_Dummy_Map.end(); 
    mapIterator++)
    {
      cout << std::endl;
      idFramePlane.push_back(mapIterator->second);
//      cout << idFramePlane; //sorted
    }


    // 1 3 5 6 8 10
    vnl_matrix<double> flag_img_transpose(timeFrames,planesPerFrame);
                        
    for (int j = 0; j < planesPerFrame; j++) 
    {
      for (int i = 0; i < timeFrames; i++) 
      {
        cout<<"i: "<<i<<"    j: "<<j<<"  " << "    value: " << idFramePlane[i + j*timeFrames];
        cout << std::endl;
        flag_img_transpose(i,j) = idFramePlane[i + j*timeFrames];
      }
    }

    cout << std::endl;
    cout << flag_img_transpose;
    cout << std::endl;
    
    // ok fin qui

    fileNumberForPlaneIFrameJ = flag_img_transpose.transpose();

  }

  cout << fileNumberForPlaneIFrameJ << std::endl;
  assert(true);
//   %GE MEDICAL SYSTEMS - PISA    
//   else 
//   [dummy id_frameplane] = sort(f);
//   flag_img = reshape(id_frameplane,[N S]);
//   flag_img = flag_img';
//     end


  //     % riduco i vettori Vx, Vy, P e s al singolo frame

  // flag_img(:,1) : colonna 1

  //     Vx = Vx(flag_img(:,1),:);
  //   Vy = Vy(flag_img(:,1),:);
  //   P  = P(flag_img(:,1),:);
  //   s  = s(flag_img(:,1),:);

  vnl_vector<double> c0 = fileNumberForPlaneIFrameJ.get_column(0);

  vnl_matrix<double> xVersorsSingleFrame(planesPerFrame,3);
  vnl_matrix<double> yVersorsSingleFrame(planesPerFrame,3);

  vnl_matrix<double> positionSingleFrame(planesPerFrame, 3);
  vnl_matrix<double> imageSizeSingleFrame(planesPerFrame, 2);

  
  // for all the id in the first column
  for (int i = 0; i < c0.size() ; i++) 
  {
    for (int j = 0; j < 3; j++) 
    {
      xVersorsSingleFrame(i, j) = xVersors(c0(i), j);
      yVersorsSingleFrame(i, j) = yVersors(c0(i), j);
      positionSingleFrame(i, j) = positions(c0(i), j);

      if (j != 2)
      {
        imageSizeSingleFrame(i, j) = imageSize(c0(i), j);
      }
    }     
  }

//   cout <<PSingleFrame;
//   cout << sSingleFrame;


  //     %% ROTAZIONE E FLIP IMMAGINI
  // 
  //     % considero le 4 possibili proiezioni tra le 2 coppie di versori tra 2 piani 
  //     proj = abs([dot(Vx(1,:),Vx(2,:)); dot(Vx(1,:),Vy(2,:)); dot(Vy(1,:),Vx(2,:)); dot(Vy(1,:),Vy(2,:))]);
  //   [dummy, idx_mode] = maxValue(proj);
  //

  
  vnl_vector<double> proj(4);
  proj(0) = abs(dot_product(xVersorsSingleFrame.get_row(0), xVersorsSingleFrame.get_row(1)));
  proj(1) = abs(dot_product(xVersorsSingleFrame.get_row(0), yVersorsSingleFrame.get_row(1)));
  proj(2) = abs(dot_product(yVersorsSingleFrame.get_row(0), xVersorsSingleFrame.get_row(1)));
  proj(3) = abs(dot_product(yVersorsSingleFrame.get_row(0), yVersorsSingleFrame.get_row(1)));

  cout << proj;

  double maxValue = -1;
  int maxId = -1; 
  for (int i = 0; i < proj.size(); i++) 
  {
    double currValue = proj(i);
    if (currValue > maxValue)
    {
      maxValue = currValue;
      maxId = i;
    }
  }


  //   % salvo in idx e idy i piani in cui ho trovato il versore parallelo
  //     % all'asse di rotazione in Vx o Vy, rispettivamente
  //     if( idx_mode==1 || idx_mode==2)
  //       Vmode = Vx(1,:);
  //     else
  //       Vmode = Vy(1,:);
  //   end
  // 

  double dummy = maxValue;
  int idx_mode = maxId;

  vnl_vector<double> Vmode;

  if (idx_mode == 0 || idx_mode ==1)
  {
     Vmode = xVersorsSingleFrame.get_row(0);
  } 
  else
  {
    Vmode = yVersorsSingleFrame.get_row(0);
  }
  
  cout << Vmode;

  //     % per ciascun piano verifico quale sia il versore circa parallelo al
  //     % versore Vmode e salvo l'indice in idx o idy
  //     idx = [];
  //   idy = [];
  //   for i = 1:S
  //     proj = abs([dot(Vmode,Vx(i,:)); dot(Vmode,Vy(i,:))]);
  //   [dummy, idx_mode] = maxValue(proj);
  //   if idx_mode == 1
  //     idx(end+1) = i;
  //   else
  //     idy(end+1) = i;
  //   end
  //     end
  // 

  vector<double> idx;
  vector<double> idy;
  
  vnl_vector<double> project(2);

  for (int i = 0; i < planesPerFrame; i++) 
  {
    project(0) = abs(dot_product(Vmode, xVersorsSingleFrame.get_row(i)));
    project(1) = abs(dot_product(Vmode, yVersorsSingleFrame.get_row(i)));

    double p0 = project(0);
    double p1 = project(1);

    maxValue = -1;
    maxId = -1;

    for (int j = 0; j < project.size(); j++) 
    {
      double currValue = project(j);
      if (currValue > maxValue)
      {
        maxValue = currValue;
        maxId = j;
      }
    }

    dummy = maxValue;
    idx_mode = maxId;

    if (idx_mode == 0)
    {
      idx.push_back(i);
      cout << "idx: " << i << " ";
    } 
    else
    {
      idy.push_back(i);
      cout << "idy: " << i << " ";
    }
 
   }
  
  //     % inizializzazione delle nuove coordinate ImagePositionPatient e
  //     % ImageOrientationPatient
  //     Pp  = zeros(size(P));
  //   Vxx = zeros(size(Vx));
  //   Vyy = zeros(size(Vy));
  //   ss  = zeros(size(s));
  // 
  //   % variabili 
  //     rot = zeros([S 1]);
  //   fliplr_flag = zeros([S 1]);
  //   flipud_flag = zeros([S 1]);
  // 


  /*
  % correzione
  for i = idx
    ss(i,:) = [s(i,2) s(i,1)];
    if Vx(i,1)>0
  %         rimg(:,:,i) = imrotate(img,90);
        Pp(i,:) = P(i,:) + s(i,1)*spacing(1)*Vx(i,:);
        Vxx(i,:) = Vy(i,:);
        Vyy(i,:) = -Vx(i,:);
        rot(i) = 90;
    else
  %         rimg(:,:,i) = imrotate(fliplr(img),90);
        Pp(i,:) = P(i,:);
        Vxx(i,:) = Vy(i,:);
        Vyy(i,:) = Vx(i,:);
        rot(i) = 90;
        fliplr_flag(i) = 1;
    end
    clear img
  end
 */

  vnl_matrix<double> newPositionSingleFrame(positionSingleFrame.rows(), positionSingleFrame.cols(), 0);
  vnl_matrix<double> newXVersorsSingleFrame(xVersorsSingleFrame.rows(), xVersorsSingleFrame.cols(), 0);
  vnl_matrix<double> newYVersorsSingleFrame(yVersorsSingleFrame.rows(), yVersorsSingleFrame.cols(), 0);
  vnl_matrix<double> newImageSizeSingleFrame(planesPerFrame, 2, 0);
  newImageSizeSingleFrame.fill(0.0);

  cout << newImageSizeSingleFrame;

  vnl_matrix<double> rotateFlag(planesPerFrame,1,0.0);
  vnl_matrix<double> flipLeftRightFlag(planesPerFrame,1,0.0);
  vnl_matrix<double> flipUpDownFlag(planesPerFrame,1,0.0);

  for (int index = 0; index < idx.size(); index++) 
  {
    int idx_ith_value = idx[index];
    
    newImageSizeSingleFrame(idx_ith_value,0) = imageSizeSingleFrame(idx_ith_value, 1);
    newImageSizeSingleFrame(idx_ith_value,1) = imageSizeSingleFrame(idx_ith_value, 0);
    
    cout << newImageSizeSingleFrame;
    
    if (xVersorsSingleFrame(idx_ith_value,0) > 0)
    {
//       //S 3
      for (int col = 0; col < 3; col++) 
      {
        newPositionSingleFrame(idx_ith_value,col) = positionSingleFrame(idx_ith_value,col) + \
          imageSizeSingleFrame(idx_ith_value,0) * \
          spacing[0] * xVersorsSingleFrame(idx_ith_value,col);

        cout << idx_ith_value << " " << col << " " << newPositionSingleFrame(idx_ith_value, col) << std::endl;
      }
        
      newXVersorsSingleFrame.set_row(idx_ith_value , yVersorsSingleFrame.get_row(idx_ith_value));
      newYVersorsSingleFrame.set_row(idx_ith_value , -xVersorsSingleFrame.get_row(idx_ith_value));
      rotateFlag(idx_ith_value,0) = 90;
//       
   } 
   else
   {
      newPositionSingleFrame.set_row(idx_ith_value , positionSingleFrame.get_row(idx_ith_value));
      newXVersorsSingleFrame.set_row(idx_ith_value , yVersorsSingleFrame.get_row(idx_ith_value));
      newYVersorsSingleFrame.set_row(idx_ith_value , xVersorsSingleFrame.get_row(idx_ith_value));
      rotateFlag(idx_ith_value,0) = 90;
      flipLeftRightFlag(idx_ith_value,0) = 1;
    }
    
    cout << newPositionSingleFrame;
  }


  
  /*
  for i = idy
    ss(i,:) = s(i,:);
    if Vy(i,1)>0
    %         rimg(:,:,i) = flipud(img);
      Pp(i,:) = P(i,:) + s(i,2)*spacing(2)*Vy(i,:);
      Vxx(i,:) = Vx(i,:);
      Vyy(i,:) = -Vy(i,:);
      flipud_flag(i) = 1;
    else
  %         rimg(:,:,i) = img;
      Pp(i,:) = P(i,:);
      Vxx(i,:) = Vx(i,:);
      Vyy(i,:) = Vy(i,:);
    end
  end

  */

  for (int index = 0; index < idy.size(); index++) 
  {
    int idy_ith_value = idy[index];

    newImageSizeSingleFrame(idy_ith_value,0) = imageSizeSingleFrame(idy_ith_value, 0);
    newImageSizeSingleFrame(idy_ith_value,1) = imageSizeSingleFrame(idy_ith_value, 1);

    if (yVersorsSingleFrame(idy_ith_value,0) > 0)
    {
      //       //S 3
      for (int col = 0; col < 3; col++) 
      {
        newPositionSingleFrame(idy_ith_value,col) = positionSingleFrame(idy_ith_value,col) + \
          imageSizeSingleFrame(idy_ith_value,1) * \
          spacing[1] * yVersorsSingleFrame(idy_ith_value,col);
      }

      newXVersorsSingleFrame.set_row(idy_ith_value , xVersorsSingleFrame.get_row(idy_ith_value));
      newYVersorsSingleFrame.set_row(idy_ith_value , -yVersorsSingleFrame.get_row(idy_ith_value));
      flipUpDownFlag(idy_ith_value,0) = 1;
      //       
    } 
    else
    {
      newPositionSingleFrame.set_row(idy_ith_value , positionSingleFrame.get_row(idy_ith_value));
      newXVersorsSingleFrame.set_row(idy_ith_value , xVersorsSingleFrame.get_row(idy_ith_value));
      newYVersorsSingleFrame.set_row(idy_ith_value , yVersorsSingleFrame.get_row(idy_ith_value));
    }
  }

  
  // 
  //   %% ORDINAMENTO PIANI
  // 
  //   %  stabilisco la relazione tra piani adiacenti 
  //   V = [(1:S)' Vxx];
  // 
  //   % scelgo il piano 1 come soluzione tentativo
  //   Vidx(1,:) = V(1,:);
  // 

  vnl_matrix<double> V(planesPerFrame,4);

  for (int i = 0; i < V.rows() ; i++) 
  {
    V(i, 0) = i;
    for (int j = 1; j < V.cols(); j++) 
    {
      V(i,j) = newXVersorsSingleFrame(i, j-1);
    }    
  }
  
  vnl_matrix<double> Vidx(1,4);
  Vidx.set_row(0, V.get_row(0));

  vnl_matrix<double> tmp = V.extract(V.rows() -1 , V.cols(), 1,0);
  
  vector<double> theta;

  V = tmp;

  cout << std::endl;
  cout << V;
  cout << std::endl;

  // % calcolo l'angolo piano 1 e gli altri piani
  for (int i = 0; i < V.rows(); i++) 
  { 
    vnl_vector<double> v0(3,0);
    vnl_vector<double> v1(3,0);
    for (int j = 1; j < 4; j++) 
    {
      v0.put(j-1, Vidx(0, j));
      v1.put(j-1, V(i, j));
    }
    
    double dot = dot_product(v0,v1);
    theta.push_back(acos(dot_product(v0,v1)));

  }


  cout << std::endl;
  for (int i = 0; i < theta.size(); i++) 
  {
    cout << theta[i];
    cout << std::endl;
  }
  
  assert(true);

// 
//   % individuo i piani adiacenti al piano 1, con una tolleranza del 10%
//     % rispetto all'angolo teorico. Il piano adiacente è un unico quando piano 1
//     % è un piano estremo
//     id_theta = find(theta<1.1*(pi/S));
  //   if numel(id_theta)==1
  //     Vidx = [Vidx; V(id_theta,:)];
  //   else
  //     Vidx = [V(id_theta(1),:); Vidx; V(id_theta(2),:)];
  //   end
 
  vector<double> id_theta;

  for (int i = 0; i < theta.size(); i++) 
  {
    if (theta[i] < 1.1 * (vnl_math::pi/planesPerFrame))
    {
      id_theta.push_back(i);
    }
  }

  for (int i = 0; i < id_theta.size(); i++) 
  {
    cout << id_theta[i] << " ";
  }

  if (id_theta.size() == 1)
  {
    // Vidx = [Vidx; V(id_theta,:)];
  } 
  else
  {
    vnl_vector<double> row0 = V.get_row(id_theta[0]);
    vnl_vector<double> row1 = Vidx.get_row(0);
    vnl_vector<double> row2 = V.get_row(id_theta[1]);

    vnl_matrix<double> tmp(3, Vidx.cols());
    tmp.set_row(0, row0);
    tmp.set_row(1, row1);
    tmp.set_row(2, row2);

    Vidx=tmp;
  }

  cout << std::endl;
  cout << V;


  cout << std::endl;
  cout << Vidx;

  //   V(id_theta,:)=[] (START);
  vnl_matrix<double> inputMatrix = V;
  vector<double> rowsToRemoveVector = id_theta;
  vnl_matrix<double> outputMatrix;

  RemoveRows(inputMatrix, rowsToRemoveVector, outputMatrix);
  
  V = outputMatrix;
  cout << outputMatrix;
  theta.clear();

//   % individuo i piani adiacenti al primo e ultimo piano in V

// while ~isempty(V)
//       % calcolo l'angolo tra ultimo piano indicizzato e gli altri piani

//       for i = 1:size(V,1)
//         theta(i) = acos(dot(Vidx(end,2:4),V(i,2:4)));
//       end

  while (V.rows() != 0 && V.cols() != 0)
  {

//     % calcolo l'angolo tra ultimo piano indicizzato e gli altri piani
//       for i = 1:size(V,1)
//         theta(i) = acos(dot(Vidx(end,2:4),V(i,2:4)));
//     end

         for (int i = 0; i < V.rows(); i++) 
         {
           vnl_vector<double> v0(3,0);
           vnl_vector<double> v1(3,0);
           for (int j = 1; j < 4; j++) 
           {
             v0.put(j-1, Vidx(Vidx.rows() - 1, j));
             v1.put(j-1, V(i, j));
           }

           cout << v0 << std::endl;
           cout << v1 << std::endl;
           double dot = dot_product(v0,v1);
           theta.push_back(acos(dot_product(v0,v1)));
         }
         

         // id_theta = find(theta<1.1*(pi/S));         
         id_theta.clear();

         for (int i = 0; i < theta.size(); i++) 
         {
           if (theta[i] < 1.1 * (vnl_math::pi/planesPerFrame))
           {
             id_theta.push_back(i);
           }
         }

         if (id_theta.size() != 0) 
         {

           //  Vidx = [Vidx; V(id_theta,:)];
           vnl_matrix<double> tmp3(Vidx.rows() + id_theta.size(), Vidx.cols());
           for (int i = 0; i < Vidx.rows(); i++) 
           {
             tmp3.set_row(i, Vidx.get_row(i));
           }

           for (int i = Vidx.rows(); i < Vidx.rows() + id_theta.size(); i++) 
           {
             tmp3.set_row(i, V.get_row(id_theta[i - Vidx.rows()]));
           }
           
           Vidx = tmp3;
           
  
           //   V(id_theta,:)=[];
           inputMatrix = V;
           rowsToRemoveVector = id_theta;
           
           RemoveRows(inputMatrix, rowsToRemoveVector, outputMatrix);

           V = outputMatrix;
           cout << outputMatrix;

           cout << std::endl;
           cout << Vidx;
           cout << std::endl;

           theta.clear();

         }
         
//          % calcolo l'angolo tra primo piano indicizzato e gli altri piani
//            for i = 1:size(V,1)
//              theta(i) = acos(dot(Vidx(1,2:4),V(i,2:4)));
//          end
         
         theta.clear();
        
         for (int i = 0; i < V.rows(); i++) 
         {
           vnl_vector<double> v0(3,0);
           vnl_vector<double> v1(3,0);
           
           for (int j = 1; j < 4; j++) 
           {
             v0.put(j-1, Vidx(0, j));
             v1.put(j-1, V(i, j));
           }

           cout << v0 << std::endl;
           cout << v1 << std::endl;
           double dot = dot_product(v0,v1);
           theta.push_back(acos(dot_product(v0,v1)));
         }

         //  id_theta = find(theta<1.1*(pi/S));

         for (int i = 0; i < theta.size(); i++) 
         {
           cout << theta[i] << std::endl;
         }
         
         id_theta.clear();

         for (int i = 0; i < theta.size(); i++) 
         {
           if (theta[i] < 1.1 * (vnl_math::pi/planesPerFrame))
           {
             id_theta.push_back(i);
             cout << std::endl;
             cout << i;
           }
         }


         if (id_theta.size() != 0) 
         {

           vnl_matrix<double> tmp3(Vidx.rows() + id_theta.size(), Vidx.cols());
           
           int size = id_theta.size();

           for (int i = 0; i < id_theta.size(); i++) 
           {
             int id_theta_i = id_theta[i];
             tmp3.set_row(i, V.get_row(id_theta[i]));
           }

           for (int i = id_theta.size(); i < id_theta.size() + Vidx.rows(); i++) 
           {
             tmp3.set_row(i, Vidx.get_row(i-id_theta.size()));
           }

           Vidx = tmp3;

           cout << std::endl;
           cout << Vidx;
           cout << std::endl;

        
           vnl_matrix<double> inputMatrix = V;
           vector<double> rowsToRemoveVector = id_theta;
           vnl_matrix<double> outputMatrix;

           RemoveRows(inputMatrix, rowsToRemoveVector, outputMatrix);

           V = outputMatrix;
           cout << outputMatrix;
           theta.clear();
         }

         cout << std::endl;
         cout << Vidx;
         cout << std::endl;
         cout << V;
         cout << std::endl;

  }

  vnl_vector<double> id_plane = Vidx.get_column(0);

  cout << std::endl;

  // order variables following planes order
  ExtractRows(fileNumberForPlaneIFrameJ, id_plane, fileNumberForPlaneIFrameJ);
  cout << fileNumberForPlaneIFrameJ;
  cout << std::endl;

// 
//   % parametri di posizione/orientamento originali da DICOM
//     % - P patient position (da DICOM)
//     % - Vx, Vy versori immagine (da DICOM)
//     % - s dimensione immagine (da DICOM)
//     % 

  // original dicom parameters
  ExtractRows(positionSingleFrame, id_plane, positionSingleFrame);
  cout << positionSingleFrame;
  cout << std::endl;

  ExtractRows(xVersorsSingleFrame, id_plane, xVersorsSingleFrame);
  cout << xVersorsSingleFrame;
  cout << std::endl;

  ExtractRows(yVersorsSingleFrame, id_plane, yVersorsSingleFrame);
  cout << yVersorsSingleFrame;
  cout << std::endl;

  ExtractRows(imageSizeSingleFrame, id_plane, imageSizeSingleFrame);
  cout << imageSizeSingleFrame;
  cout << std::endl;

// 
//   % parametri di posizione/orientamento originali da DICOM
//     % - Pp patient position (da DICOM)
//     % - Vxx, Vyy versori immagine (da DICOM)
//     % - ss dimensione immagine (da DICOM)
//     % 

  /////////////////////////////
  // updated (output) dicom parameters:
  /////////////////////////////

  
  ExtractRows(newPositionSingleFrame, id_plane, newPositionSingleFrame);
  cout << newPositionSingleFrame;
  cout << std::endl;

  ExtractRows(newXVersorsSingleFrame, id_plane, newXVersorsSingleFrame);
  cout << newXVersorsSingleFrame;
  cout << std::endl;

  ExtractRows(newYVersorsSingleFrame, id_plane, newYVersorsSingleFrame);
  cout << newYVersorsSingleFrame;
  cout << std::endl;

  ExtractRows(newImageSizeSingleFrame, id_plane, newImageSizeSingleFrame);
  cout << newImageSizeSingleFrame;
  cout << std::endl;

// 
//   % mappatura immagini
//     % - N: numero frame nell'acquisizione
//     % - numero piani nell'acquisizione
//     % - filelist: struct array contente nel campo name il nome di ciascuna
//     %   immagine apprtentente all'acquisizione
//     % - flag_img: contiene l'indice dell'immagine per ogni coordinata
//     %   piano-frame (eg: se flag_img(i,j) == num_img, allora
//     %   filelist(num_img).name è il nome file che contiente l'immagine del piano i nel frame j
//     % 

     // fileNumberForPlaneIFrameJ
     // dicomLocalFileNamesVector

//   % parametrui di rotazione da applicare alle immagini acquisite per ottenere
//     % immagini tra loro coerenti
//     % - rot: contiene la rotazione da effettuare
//     % - fliplr_flag: se 1 l'immagine deve essere specchiata in orizzontale
//     % - fliplr_flag: se 1 l'immagine deve essere specchiata in verticale

  // rotations and flip flags

  for (int i = 0; i < id_plane.size(); i++) 
  {
    cout << std::endl;
    cout << id_plane[i];
  }

  ExtractRows(rotateFlag, id_plane, rotateFlag);
  cout << rotateFlag;
  cout << std::endl;

  ExtractRows(flipLeftRightFlag, id_plane, flipLeftRightFlag);
  cout << flipLeftRightFlag;
  cout << std::endl;

  ExtractRows(flipUpDownFlag, id_plane, flipUpDownFlag);
  cout << flipUpDownFlag;
  cout << std::endl;

  mafDEL(importer);
  vtkDEL(directoryReader);


//   % calcolo l'angolo piano 1 e gli altri piani
//     for i = 1:size(V,1)
//       theta(i) = acos(dot(Vidx(1,2:4),V(i,2:4)));
//     end
// 
//     % individuo i piani adiacenti al piano 1, con una tolleranza del 10%
//     % rispetto all'angolo teorico. Il piano adiacente è un unico quando piano 1
//     % è un piano estremo
//     id_theta = find(theta<1.1*(pi/S));
//   if numel(id_theta)==1
//     Vidx = [Vidx; V(id_theta,:)];
//   else
//     Vidx = [V(id_theta(1),:); Vidx; V(id_theta(2),:)];
//   end
//     V(id_theta,:)=[];
//   clear theta
// 
//     % individuo i piani adiacenti al primo e ultimo piano in V
//     while ~isempty(V)
//       % calcolo l'angolo tra ultimo piano indicizzato e gli altri piani
//       for i = 1:size(V,1)
//         theta(i) = acos(dot(Vidx(end,2:4),V(i,2:4)));
//   end
//     id_theta = find(theta<1.1*(pi/S));
//   if ~isempty(id_theta)
//     Vidx = [Vidx; V(id_theta,:)];
//   V(id_theta,:)=[];
//   end
//     clear theta
// 
//     % calcolo l'angolo tra primo piano indicizzato e gli altri piani
//     for i = 1:size(V,1)
//       theta(i) = acos(dot(Vidx(1,2:4),V(i,2:4)));
//   end
//     id_theta = find(theta<1.1*(pi/S));
//   if ~isempty(id_theta)
//     Vidx = [V(id_theta,:); Vidx];
//   V(id_theta,:)=[];
//   end
//     clear theta
//     end
// 
//     % ordino tutte le variabili secondo l'ordine dei piani
//     id_plane = Vidx(:,1);
// 
//   % flag_img
//     flag_img = flag_img(id_plane,:);
// 
//   % parametri DICOM originali
//     P  = P(id_plane,:);
//   Vx = Vx(id_plane,:);
//   Vy = Vy(id_plane,:);
//   s  = s(id_plane,:);
// 
//   % parametri DICOM aggiornati
//     Pp  = Pp(id_plane,:);
//   Vxx = Vxx(id_plane,:);
//   Vyy = Vyy(id_plane,:);
//   ss  = ss(id_plane,:);
// 
//   % flag di rotazione e flip
//     rot = rot(id_plane);
//   fliplr_flag = fliplr_flag(id_plane);
//   flipud_flag = flipud_flag(id_plane);
// 
// 
// 
//     %% SALVATAGGIO VARIABILI DI INTERESSE
// 
//     % esporto nel file map_DICOM le variabili filelist
//     save('DICOMap','filelist','N','S','spacing','flag_img','P','Vx','Vy','s','Pp','Vxx','Vyy','ss','rot','fliplr_flag','flipud_flag');


}

void medOpImporterDicomOffisTest::RemoveRows\
( vnl_matrix<double> &inputMatrix, vector<double> &rowsToRemoveVector, vnl_matrix<double> &outputMatrix )
{
  int rowsToRemove = 0;

  for (int i = 0; i < inputMatrix.rows(); i++) 
  { 
    vector<double>::iterator result;
    result = find( rowsToRemoveVector.begin(), rowsToRemoveVector.end(), i );

    if( result != rowsToRemoveVector.end() ) 
    {
      // found
      rowsToRemove++;    
    }
  }

  cout << rowsToRemove;

  vnl_matrix<double> tmpMatrix(inputMatrix.rows() - rowsToRemove, inputMatrix.cols());
  int idTmpRow = 0;
  for (int i = 0; i < inputMatrix.rows(); i++) 
  { 
    vector<double>::iterator result;
    result = find( rowsToRemoveVector.begin(), rowsToRemoveVector.end(), i );

    if( result == rowsToRemoveVector.end() ) 
    {
      //not found
      tmpMatrix.set_row(idTmpRow, inputMatrix.get_row(i) );
      idTmpRow++;
    }

    else 
    {
      //found: remove row
    }
  }

  outputMatrix = tmpMatrix;
}


void medOpImporterDicomOffisTest::ExtractRows\
( vnl_matrix<double> &inputMatrix, vector<double> &rowsToExtractVector,\
  vnl_matrix<double> &outputMatrix )
{
  int numRowsToExtract = -1;
  numRowsToExtract = rowsToExtractVector.size();
  
  vnl_matrix<double> tmpMatrix(numRowsToExtract, inputMatrix.cols());
  
  int idTmpRow = 0;

  for (int i = 0; i < rowsToExtractVector.size(); i++) 
  {   
    tmpMatrix.set_row(i, inputMatrix.get_row(rowsToExtractVector[i]) );
  }

  outputMatrix = tmpMatrix;
}

void medOpImporterDicomOffisTest::ExtractRows( vnl_matrix<double> &inputMatrix,\
 vnl_vector<double> &rowsToExtractVector, vnl_matrix<double> &outputMatrix )
{
  vnl_vector<double> rowsToExtractVNLVector = rowsToExtractVector;
  
  vector<double> rowsToExtractSTDVector;
 
  for (int i = 0; i < rowsToExtractVNLVector.size(); i++) 
  {
    rowsToExtractSTDVector.push_back(rowsToExtractVNLVector[i]);   
  }  

  ExtractRows(inputMatrix, rowsToExtractSTDVector, outputMatrix);
}