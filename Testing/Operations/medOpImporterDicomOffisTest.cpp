/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpImporterDicomOffisTest.cpp,v $
Language:  C++
Date:      $Date: 2010-03-30 15:08:25 $
Version:   $Revision: 1.1.2.8 $
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
//   % script per la mappatura e il calcolo delle traformazioni per allineare le
//     % immagini CMR
//     % 
//     % INPUT directory contente solo le immagini
//     % 
//     % OUTPUT 
//     % 
//     % mappatura immagini
//     % - N: numero frame nell'acquisizione
//     % - numero piani nell'acquisizione
//     % - filelist: struct array contente nel campo name il nome di ciascuna
//     %   immagine apprtentente all'acquisizione
//     % - flag_img: contiene l'indice dell'immagine per ogni coordinata
//     %   piano-frame (eg: se flag_img(i,j) == num_img, allora
//     %   filelist(num_img).name è il nome file che contiente l'immagine del piano i nel frame j
//     % 
//     % parametri di posizione/orientamento originali da DICOM
//     % - P patient position (da DICOM)
//     % - Vx, Vy versori immagine (da DICOM)
//     % - s dimensione immagine (da DICOM)
//     % 
//     % parametri di posizione/orientamento originali da DICOM
//     % - Pp patient position (da DICOM)
//     % - Vxx, Vyy versori immagine (da DICOM)
//     % - ss dimensione immagine (da DICOM)
//     % 
//     % parametrui di rotazione da applicare alle immagini acquisite per ottenere
//     % immagini tra loro coerenti
//     % - rot: contiene la rotazione da effettuare
//     % - fliplr_flag: se 1 l'immagine deve essere specchiata in orizzontale
//     % - fliplr_flag: se 1 l'immagine deve essere specchiata in verticale
// 
//     clear all
//     close all
// 
//     % carico la directory contentente le immagini e creo la list delle immagini
//     path = uigetdir('Select patient directory');
//   filelist = dir(path);
//   filelist(1:2) = [];
// 
//   % calcolo numero frame N e piani S dal numero totale di file e dal
//     % dicominfo della prima immagine
//     info = dicominfo([path '\' filelist(1).name]);
//     N = info.CardiacNumberOfImages;
//   S = numel(filelist)/N;

  medOpImporterDicomOffis *importer=new medOpImporterDicomOffis();
  importer->TestModeOn();

  // SIEMENS NIGUARDA
  wxString dicomDir = "d:\\wip\\DicomConFetteAlContrario\\p20\\";
  
  // GE PISA
  // wxString dicomDir = "d:\\wip\\DicomConFetteAlContrario\\p09\\";

  CPPUNIT_ASSERT(wxDirExists(dicomDir));
  vtkDirectory *directoryReader = vtkDirectory::New();
  directoryReader->Open(dicomDir);
 
  wxString file1 = "UNSET";

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
      file1 = directoryReader->GetFile(i);
      firstDicomSliceFileIndex = i;
      break;
    }
  }
 
  //CPPUNIT_ASSERT(file1.compare("25292865") == 0);

  int numberOfFiles = directoryReader->GetNumberOfFiles();
  
  DcmFileFormat dicomFileHandler;    

  wxString file1ABSFileName = dicomDir + file1;

  CPPUNIT_ASSERT(wxFileExists(file1ABSFileName));

  OFCondition status = dicomFileHandler.loadFile(file1ABSFileName);

  CPPUNIT_ASSERT(status.good());

  DcmDataset *dicomDataset = dicomFileHandler.getDataset();

  long int dcmCardiacNumberOfImages = -1;
  dicomDataset->findAndGetLongInt(DCM_CardiacNumberOfImages,dcmCardiacNumberOfImages);

  //CPPUNIT_ASSERT(dcmCardiacNumberOfImages == 40);

  // N
  long int N = dcmCardiacNumberOfImages;

  // S
  int S = directoryReader->GetNumberOfFiles() / N;

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

  vnl_matrix<double> Vx(N*S, 3, 0.0);
  vnl_matrix<double> Vy(N*S, 3, 0.0);
  vnl_matrix<double> P(N*S, 3, 0.0);
  vnl_matrix<double> s(N*S, 2, 0.0);
  vnl_matrix<double> f(N*S, 1, 0.0);

  // 
  //   % dall'infodicom di ciascuna immagine recupero le info necessarie
  //     h = waitbar(0,'Mapping DICOM folder ...');
  //   for i = 1:N*S
  //     info = dicominfo([path '\' filelist(i).name]);
  //     P(i,:)  = info.ImagePositionPatient;
  //   Vx(i,:) = info.ImageOrientationPatient(1:3);
  //   Vy(i,:) = info.ImageOrientationPatient(4:6);
  //   s(i,:)  = [info.Columns info.Rows]; 
  //   f(i) = info.InstanceNumber;
  //   waitbar(i/(N*S));
  //   end
  //     close(h);

  for (int i = 0; i < N*S; i++) 
  {

    wxString currentDicomSliceFileName = directoryReader->GetFile(i+firstDicomSliceFileIndex);
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

    //     P(i,:)  = info.ImagePositionPatient;
    //   Vx(i,:) = info.ImageOrientationPatient(1:3);
    //   Vy(i,:) = info.ImageOrientationPatient(4:6);
    //   s(i,:)  = [info.Columns info.Rows]; 
    //   f(i) = info.InstanceNumber;
    //   waitbar(i/(N*S));
    //   end


    for (int j = 0; j < 3; j++) 
    {
      P(i,j) = dcmImagePositionPatient[j];
      Vx(i,j) = dcmImageOrientationPatient[j];
      Vy(i,j) = dcmImageOrientationPatient[j+3];
    }
    
    s(i, 0) = dcmColumns;
    s(i, 1) = dcmRows;

    f(i,0) = dcmInstanceNumber;
  }
  
  cout<< P(0,0);

  double diff = fabs(P(0,0) - 191.8841);
  //CPPUNIT_ASSERT(diff<.0001);

  diff = fabs(P(0,1) - (- 195.6752));
  //CPPUNIT_ASSERT(diff<.0001);

  diff = fabs(P(0,2) - (75.2316));
  //CPPUNIT_ASSERT(diff<.0001);

  cout << P(719,0);
  diff = fabs(P(719,0) - ( -7.5477));
  //CPPUNIT_ASSERT(diff<.0001);

  diff = fabs(P(719,1) - (136.5524));
  //CPPUNIT_ASSERT(diff<.0001);

  diff = fabs(P(719,2) - (204.6599));
  //CPPUNIT_ASSERT(diff<.0001);

  diff = fabs(Vx(0,0) - (-0.6247));
  //CPPUNIT_ASSERT(diff<.0001);

  diff = fabs(Vx(0,1) - (0.6782));
  //CPPUNIT_ASSERT(diff<.0001);

  diff = fabs(Vx(0,2) - (0.3871));
  //CPPUNIT_ASSERT(diff<.0001);

  // 0.6255   -0.6768   -0.3881
  diff = fabs(Vx(719,0) - (0.6255));
  //CPPUNIT_ASSERT(diff<.0001);

  diff = fabs(Vx(719,1) - (-0.6768));
  //CPPUNIT_ASSERT(diff<.0001);

  diff = fabs(Vx(719,2) - (-0.3881));
  //CPPUNIT_ASSERT(diff<.0001);

  // -0.3197    0.2301   -0.9191
  diff = fabs(Vy(0,0) - (-0.3197));
  //CPPUNIT_ASSERT(diff<.0001);

  diff = fabs(Vy(0,1) - (0.2301));
  //CPPUNIT_ASSERT(diff<.0001);

  diff = fabs(Vy(0,2) - (-0.9191));
  //CPPUNIT_ASSERT(diff<.0001);

  //  -0.6312   -0.1467   -0.7616
  diff = fabs(Vy(719,0) - ( -0.6312));
  //CPPUNIT_ASSERT(diff<.0001);

  diff = fabs(Vy(719,1) - (-0.1467));
  //CPPUNIT_ASSERT(diff<.0001);

  diff = fabs(Vy(719,2) - (-0.7616));
  //CPPUNIT_ASSERT(diff<.0001);

  cout << std::endl;
  cout << s(0,0) << std::endl;
  cout << s(0,1) << std::endl;


  //   %% MAPPING PIANO-FRAME
  // 
  //     % flag_img(i,j) indica il numero file del piano i, frame j
  //     flag_img = zeros(S,N);
  // 

  vnl_matrix<int> flag_img(S,N,0);

  const char *dcmManufacturer = "?";
  dicomDataset->findAndGetString(DCM_Manufacturer, dcmManufacturer);

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

    for (int i = 0; i < f.rows(); i++) 
    {
      if (f(i,0) == 1)
      {
        id_frame.push_back(i);
      }
    }
    
    //CPPUNIT_ASSERT(id_frame.size() == 18);
    //CPPUNIT_ASSERT(id_frame[0] == 655);

    for (int i = 0; i < flag_img.rows(); i++) 
    {
      flag_img(i,0) = id_frame[i];
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
    vnl_matrix<double> VxVy(N*S, 6);
    for (int i = 0; i < N*S; i++) 
    {
      for (int j = 0; j < 3; j++) 
      {
        VxVy(i,j) = Vx(i,j);
        VxVy(i,j+3) = Vy(i,j);
      }
    }
    
    // cout << VxVy;

    for (int i = 0; i < S; i++) 
    {
      // build Vtemp(i)
      vnl_matrix<double> Vtemp(N*S, 6);
      for (int row = 0; row < N*S; row++) 
      {
        for (int col = 0; col < 3; col++) 
        {
          Vtemp(row, col) = Vx(id_frame[i],col);
          Vtemp(row, col +3) = Vy(id_frame[i],col);
        }      
      }

      vector<double> id_plane;

      for (int a = 0; a < N*S; a++) 
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

        flag_img(i, f(k,0) - 1 ) = k;
        assert(true);
      }
      assert(true);
    } //end for (int i = 0; i < S; i++) 
  } //end if SIEMENS

  cout << flag_img << std::endl;
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

  vnl_vector<int> c0 = flag_img.get_column(0);

  vnl_matrix<double> VxSingleFrame(S,3);
  vnl_matrix<double> VySingleFrame(S,3);

  vnl_matrix<double> PSingleFrame(S, 3);
  vnl_matrix<double> sSingleFrame(S, 2);

  
  // for all the id in the first column
  for (int i = 0; i < c0.size() ; i++) 
  {
    for (int j = 0; j < 3; j++) 
    {
      VxSingleFrame(i, j) = Vx(c0(i), j);
      VySingleFrame(i, j) = Vy(c0(i), j);
      PSingleFrame(i, j) = P(c0(i), j);

      if (j != 2)
      {
        sSingleFrame(i, j) = s(c0(i), j);
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
  proj(0) = abs(dot_product(VxSingleFrame.get_row(0), VxSingleFrame.get_row(1)));
  proj(1) = abs(dot_product(VxSingleFrame.get_row(0), VySingleFrame.get_row(1)));
  proj(2) = abs(dot_product(VySingleFrame.get_row(0), VxSingleFrame.get_row(1)));
  proj(3) = abs(dot_product(VySingleFrame.get_row(0), VySingleFrame.get_row(1)));

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
     Vmode = VxSingleFrame.get_row(0);
  } 
  else
  {
    Vmode = VySingleFrame.get_row(0);
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

  vector<int> idx;
  vector<int> idy;
  
  vnl_vector<double> project(2);

  for (int i = 0; i < S; i++) 
  {
    project(0) = abs(dot_product(Vmode, VxSingleFrame.get_row(i)));
    project(1) = abs(dot_product(Vmode, VySingleFrame.get_row(i)));

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

  vnl_matrix<double> Pp(PSingleFrame.rows(), PSingleFrame.cols(), 0);
  vnl_matrix<double> Vxx(VxSingleFrame.rows(), VxSingleFrame.cols(), 0);
  vnl_matrix<double> Vyy(VySingleFrame.rows(), VySingleFrame.cols(), 0);
  vnl_matrix<int> ss(S, 2, 0);
  ss.fill(0.0);

  cout << ss;

  vnl_matrix<double> rot(S,1,0.0);
  vnl_matrix<double> fliplr_flag(S,1,0.0);
  vnl_matrix<double> flipud_flag(S,1,0.0);

  for (int index = 0; index < idx.size(); index++) 
  {
    int idx_ith_value = idx[index];
    
    ss(idx_ith_value,0) = sSingleFrame(idx_ith_value, 1);
    ss(idx_ith_value,1) = sSingleFrame(idx_ith_value, 0);
    
    cout << ss;
    
    if (VxSingleFrame(idx_ith_value,0) > 0)
    {
//       //S 3
      for (int col = 0; col < 3; col++) 
      {
        Pp(idx_ith_value,col) = PSingleFrame(idx_ith_value,col) + \
          sSingleFrame(idx_ith_value,0) * \
          spacing[0] * VxSingleFrame(idx_ith_value,col);

        cout << idx_ith_value << " " << col << " " << Pp(idx_ith_value, col) << std::endl;
      }
        
      Vxx.set_row(idx_ith_value , VySingleFrame.get_row(idx_ith_value));
      Vyy.set_row(idx_ith_value , -VxSingleFrame.get_row(idx_ith_value));
      rot(idx_ith_value,0) = 90;
//       
   } 
   else
   {
      Pp.set_row(idx_ith_value , PSingleFrame.get_row(idx_ith_value));
      Vxx.set_row(idx_ith_value , VySingleFrame.get_row(idx_ith_value));
      Vyy.set_row(idx_ith_value , VxSingleFrame.get_row(idx_ith_value));
      rot(idx_ith_value,0) = 90;
      fliplr_flag(idx_ith_value,0) = 1;
    }
    
    cout << Pp;
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

    ss(idy_ith_value,0) = sSingleFrame(idy_ith_value, 0);
    ss(idy_ith_value,1) = sSingleFrame(idy_ith_value, 1);

    if (VySingleFrame(idy_ith_value,0) > 0)
    {
      //       //S 3
      for (int col = 0; col < 3; col++) 
      {
        Pp(idy_ith_value,col) = PSingleFrame(idy_ith_value,col) + \
          sSingleFrame(idy_ith_value,1) * \
          spacing[1] * VySingleFrame(idy_ith_value,col);
      }

      Vxx.set_row(idy_ith_value , VxSingleFrame.get_row(idy_ith_value));
      Vyy.set_row(idy_ith_value , -VySingleFrame.get_row(idy_ith_value));
      flipud_flag(idy_ith_value,0) = 1;
      //       
    } 
    else
    {
      Pp.set_row(idy_ith_value , PSingleFrame.get_row(idy_ith_value));
      Vxx.set_row(idy_ith_value , VxSingleFrame.get_row(idy_ith_value));
      Vyy.set_row(idy_ith_value , VySingleFrame.get_row(idy_ith_value));
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

  vnl_matrix<double> V(S,4);

  for (int i = 0; i < V.rows() ; i++) 
  {
    V(i, 0) = i;
    for (int j = 1; j < V.cols(); j++) 
    {
      V(i,j) = Vxx(i, j-1);
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
 
  vector<int> id_theta;

  for (int i = 0; i < theta.size(); i++) 
  {
    if (theta[i] < 1.1 * (vnl_math::pi/S))
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
  vector<int> rowsToRemoveVector = id_theta;
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
           if (theta[i] < 1.1 * (vnl_math::pi/S))
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

      // fin qui ok           
      //     cout << V;
           //   V(id_theta,:)=[] (END);

         }
         
//          cout << std::endl;
//          cout << Vidx;
//          cout << std::endl;
//          cout << V;
//          cout << std::endl;

         //////////////// second FOR
         
// 
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

         cout << std::endl;

         for (int i = 0; i < theta.size(); i++) 
         {
           cout << theta[i] << std::endl;
         }
         
         // ok fin qui 
         id_theta.clear();

         for (int i = 0; i < theta.size(); i++) 
         {
           if (theta[i] < 1.1 * (vnl_math::pi/S))
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
           vector<int> rowsToRemoveVector = id_theta;
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
//       if ~isempty(id_theta)
//         Vidx = [Vidx; V(id_theta,:)];
//         V(id_theta,:)=[];
//       end
//       clear theta
//  

//       % calcolo l'angolo tra primo piano indicizzato e gli altri piani
//       for i = 1:size(V,1)
//           theta(i) = acos(dot(Vidx(1,2:4),V(i,2:4)));
//       end

//       id_theta = find(theta<1.1*(pi/S));
//       if ~isempty(id_theta)
//           Vidx = [V(id_theta,:); Vidx];
//           V(id_theta,:)=[];
//       end
//       clear theta
//   end

  cout << std::endl;
  cout << Vidx;
  cout << std::endl;
  cout << ss;
  cout << Pp;
  cout << rot; // ok
  cout << std::endl;
  cout << Vxx;
  cout << std::endl;
  cout << Vyy;
  cout << std::endl;
  cout <<fliplr_flag;
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
( vnl_matrix<double> &inputMatrix, vector<int> &rowsToRemoveVector, vnl_matrix<double> &outputMatrix )
{
  int rowsToRemove = 0;

  for (int i = 0; i < inputMatrix.rows(); i++) 
  { 
    vector<int>::iterator result;
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
    vector<int>::iterator result;
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