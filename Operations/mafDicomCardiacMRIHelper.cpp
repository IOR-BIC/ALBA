/*=========================================================================

 Program: MAF2
 Module: mafDicomCardiacMRIHelper
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

const bool DEBUG_MODE = true;

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafDicomCardiacMRIHelper.h"

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
#include "dcmtk/ofstd/ofstdinc.h"

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTRING

#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"
#include "vnl/vnl_math.h"
#include <algorithm>
#include <map>
#include "mafOpImporterDicomOffis.h"
#include "vtkDirectory.h"
#include "wx/busyinfo.h"
#include "mafProgressBarHelper.h"

mafDicomCardiacMRIHelper::mafDicomCardiacMRIHelper()
{
  m_InputDicomDirectoryABSPath = "UNDEFINED_m_InputDicomDirectoryABSPath";
  m_TestMode = false;
  m_Listener = NULL;
  m_Mode = DICOM_DIRECTORY_ABS_PATH;
}

mafDicomCardiacMRIHelper::~mafDicomCardiacMRIHelper()
{

}

int mafDicomCardiacMRIHelper::ParseDicomDirectory() 
{
  // core parsing and transformation code: matlab pseudo code is used
  // in comments and identified by the matlab comment symbol (%)
  
  long int timeFrames = -1;
  DcmDataset *dicomDataset = NULL;
  wxString dicomDir = "UNDEFINED";
  DcmFileFormat dicomFileHandler;
  vector<string> dicomABSFileNamesVector;
  vtkDirectory *directoryReader = NULL;
  int planesPerFrame = -1;
  vector<int> seriesNumbers;
  int minSeriesNumber = 999;

  if (m_Mode == DICOM_DIRECTORY_ABS_PATH)
  {
	  dicomDir = m_InputDicomDirectoryABSPath;

	  assert(wxDirExists(dicomDir));
	  directoryReader = vtkDirectory::New();
	  directoryReader->Open(dicomDir);

	  wxString localFileName = "UNSET";

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
			  localFileName = directoryReader->GetFile(i);
			  dicomABSFileNamesVector.push_back((dicomDir + localFileName).c_str());
		  }
	  }
  }
  else if (m_Mode == DICOM_SLICES_ABS_FILE_NAMES_VECTOR)
  { 
  	assert(m_InputDicomSlicesABSFileNamesVector.size() != 0);
    dicomABSFileNamesVector = m_InputDicomSlicesABSFileNamesVector;
  }
  else
  { 
	  std::ostringstream stringStream;
	  stringStream << "Unsupported mode: " << m_Mode << std::endl;          
	  mafLogMessage(stringStream.str().c_str());
	  return MAF_ERROR;
  }
  
  wxString file1ABSFileName = dicomABSFileNamesVector[0].c_str();

  assert(wxFileExists(file1ABSFileName));

  OFCondition status = dicomFileHandler.loadFile(file1ABSFileName);

  assert(status.good());

  dicomDataset = dicomFileHandler.getDataset();

  long int dcmCardiacNumberOfImages = -1;
  dicomDataset->findAndGetLongInt(DCM_CardiacNumberOfImages,dcmCardiacNumberOfImages);

  // timeFrames
  timeFrames = dcmCardiacNumberOfImages;

  // planesPerFrame
  planesPerFrame = dicomABSFileNamesVector.size() / timeFrames;

  if (planesPerFrame == 0)
  {
    vtkDEL(directoryReader);
    return MAF_ERROR;
  }

  //   spacing = info.PixelSpacing;
  // 

  double spacing[3];
  spacing[2] = 0;

  dicomDataset->findAndGetFloat64(DCM_PixelSpacing,spacing[0],0);
   
  dicomDataset->findAndGetFloat64(DCM_PixelSpacing,spacing[1],1);

  //   % initialization
  //     % xVersors = x versors
  //     % yVersors = y versors
  //     % positions  = patient position
  //     % imageSize  = image size
  //     % frame = time frame
  //   
  //
  //   xVersors = zeros(timeFrames*planesPerFrame,3);
  //   yVersors = zeros(timeFrames*planesPerFrame,3);
  //   positions  = zeros(timeFrames*planesPerFrame,3);
  //   imageSize  = zeros(timeFrames*planesPerFrame,2);
  //   frame  = zeros(timeFrames*planesPerFrame,1);

  vnl_matrix<double> xVersors(timeFrames*planesPerFrame, 3, 0.0);
  vnl_matrix<double> yVersors(timeFrames*planesPerFrame, 3, 0.0);
  vnl_matrix<double> positions(timeFrames*planesPerFrame, 3, 0.0);
  vnl_matrix<double> imageSize(timeFrames*planesPerFrame, 2, 0.0);
  vnl_matrix<double> frame(timeFrames*planesPerFrame, 1, 0.0);

  wxString busyMessage = "Cardiac MRI Found, please wait for reader initialization...";
  
	mafProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar(busyMessage);

  long progress = 0;
  
  for (int i = 0; i < timeFrames*planesPerFrame; i++) 
  {
    wxString currentSliceABSFileName = dicomABSFileNamesVector[i].c_str();

	  std::ostringstream stringStream;
	  stringStream << "Parsing " << currentSliceABSFileName.c_str() << " file with Id " << i << " , number " << i+1 << " of " << timeFrames*planesPerFrame << std::endl;          
	  mafLogMessage(stringStream.str().c_str());

    assert(wxFileExists(currentSliceABSFileName));

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

    long int dcmSeriesNumber;
    dicomDataset->findAndGetLongInt(DCM_SeriesNumber, dcmSeriesNumber);
    if(dcmSeriesNumber < minSeriesNumber)
    {
      minSeriesNumber = dcmSeriesNumber;
    }
    seriesNumbers.push_back(dcmSeriesNumber);

#ifdef _DEBUG
    mafLogMessage("DEBUG >>>>>>>>> Series Number Found %ld",dcmSeriesNumber);
#endif // _DEBUG
    

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


    progress = i * 100 / (double) (timeFrames*planesPerFrame);
		progressHelper.UpdateProgressBar(progress);
    
  }

  
// FILE TO PLANE-FRAME MAPPING
// 
// fileNumberForPlaneIFrameJ: contains image index for each (i,j) coordinate
// If fileNumberForPlaneIFrameJ(i,j) == num_img, then
// dicomLocalFileNamesVector[num_img] is the file name containing the image
// of plane i at time frame j
 
  vnl_matrix<double> fileNumberForPlaneIFrameJ(planesPerFrame,timeFrames,0);

  const char *dcmManufacturer = "?";
  dicomDataset->findAndGetString(DCM_Manufacturer, dcmManufacturer);

  // SIEMENS (P20 dicomDir test data)

  if (strcmp(dcmManufacturer, "SIEMENS")==0)
  {
    //   % SIEMENS - NIGUARDA
    //     if strcmp(info.Manufacturer,'SIEMENS')
    //       % first frame indexes 
    //       id_frame = find(frame==1)';
    //       fileNumberForPlaneIFrameJ(:,1) = id_frame;

    // find frame rows with value == 1

    vector<double> id_frame;

    for (int i = 0; i < frame.rows(); i++) 
    {
      if (frame(i,0) == 1)
      {
        id_frame.push_back(i);
      }
    }

    //Number of frame must be the same
    if (id_frame.size() != planesPerFrame)
    {
      wxMessageBox("ERROR during processing Cardiac MRI");
      vtkDEL(directoryReader);
      return MAF_ERROR;
    }

    for (int i = 0; i < fileNumberForPlaneIFrameJ.rows(); i++) 
    {
      fileNumberForPlaneIFrameJ(i,0) = id_frame[i];
    }

    // 
    //   & find by comparison all images on ith plane
    //   
    //     for i = 1:planesPerFrame
    //       Vtemp  = repmat([xVersors(id_frame(i),:) yVersors(id_frame(i),:)],[timeFrames*planesPerFrame 1]);


    //   K>> [xVersors(656,:) yVersors(656,:)]
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
      
      for (int c = 0; c < id_plane.size(); c++) 
      {
        int k = id_plane[c];

        fileNumberForPlaneIFrameJ(i, frame(k,0) - 1 ) = k;
        assert(true);
      }
      assert(true);
    } //end for (int i = 0; i < planesPerFrame; i++) 
  } //end if SIEMENS
  else if (strcmp(dcmManufacturer, "INVALID")!=0) // Losi patch
  {

    //   %GE MEDICAL SYSTEMS - PISA    
    //   else 
    //   [dummy id_frameplane] = sort(frame);
    //   fileNumberForPlaneIFrameJ = reshape(id_frameplane,[timeFrames planesPerFrame]);
    //   fileNumberForPlaneIFrameJ = fileNumberForPlaneIFrameJ';
    //     end
    
    std::map<int,int> idFramePlane_Dummy_Map;

    for (int i = 0; i < frame.rows() ; i++) 
    {
      idFramePlane_Dummy_Map[frame(i,0)] = i;
    }

    std::map<int,int>::iterator mapIterator;   

    vector<double> idFramePlane;

    for(mapIterator = idFramePlane_Dummy_Map.begin() ; mapIterator!=idFramePlane_Dummy_Map.end(); 
    mapIterator++)
    {
       
      idFramePlane.push_back(mapIterator->second);
    }

    vnl_matrix<double> flag_img_transpose(timeFrames,planesPerFrame);
                        
    for (int j = 0; j < planesPerFrame; j++) 
    {
      for (int i = 0; i < timeFrames; i++) 
      {        
        flag_img_transpose(i,j) = idFramePlane[i + j*timeFrames];
      }
    }
    fileNumberForPlaneIFrameJ = flag_img_transpose.transpose();

    m_FileNumberForPlaneIFrameJ = fileNumberForPlaneIFrameJ;
  }
  assert(true);
//   %GE MEDICAL SYSTEMS - PISA    
//   else 
//   [dummy id_frameplane] = sort(frame);
//   fileNumberForPlaneIFrameJ = reshape(id_frameplane,[timeFrames planesPerFrame]);
//   fileNumberForPlaneIFrameJ = fileNumberForPlaneIFrameJ';
//     end


  //   % reduce xVersors, yVersors, positions and imageSize to the single frame

  //   fileNumberForPlaneIFrameJ(:,1) : colonna 1

  //   xVersors = xVersors(fileNumberForPlaneIFrameJ(:,1),:);
  //   yVersors = yVersors(fileNumberForPlaneIFrameJ(:,1),:);
  //   positions  = positions(fileNumberForPlaneIFrameJ(:,1),:);
  //   imageSize = imageSize(fileNumberForPlaneIFrameJ(:,1),:);

  vnl_vector<double> fileNumbersForPlaneIFrame0 = fileNumberForPlaneIFrameJ.get_column(0);

  vnl_matrix<double> xVersorsSingleFrame(planesPerFrame,3);
  vnl_matrix<double> yVersorsSingleFrame(planesPerFrame,3);

  vnl_matrix<double> positionSingleFrame(planesPerFrame, 3);
  vnl_matrix<double> imageSizeSingleFrame(planesPerFrame, 2);
  
  // for all the id in the first column
  for (int i = 0; i < fileNumbersForPlaneIFrame0.size() ; i++) 
  {
    for (int j = 0; j < 3; j++) 
    {
      xVersorsSingleFrame(i, j) = xVersors(fileNumbersForPlaneIFrame0(i), j);
      yVersorsSingleFrame(i, j) = yVersors(fileNumbersForPlaneIFrame0(i), j);
      positionSingleFrame(i, j) = positions(fileNumbersForPlaneIFrame0(i), j);

      if (j != 2)
      {
        imageSizeSingleFrame(i, j) = imageSize(fileNumbersForPlaneIFrame0(i), j);
      }
    }     
  }

  //     %% IMAGES ROTATION AND FLIP
  // 
  //     % considering 4 possible projections between the 2 versors couple between 2 planes 
  //     proj = abs([dot(xVersors(1,:),xVersors(2,:)); dot(xVersors(1,:),yVersors(2,:)); dot(yVersors(1,:),xVersors(2,:)); dot(yVersors(1,:),yVersors(2,:))]);
  //     [dummy, idx_mode] = maxValue(proj);
  //

  // Workaround Added by Losi on 5/5/2011
  // Patch to correctly visualize and import data
  // \\HD01\Public\Dati\Dicom Regression\NIG004_MAGLU_SA
  // \\HD01\Public\Dati\Dicom Regression\NIG009-PAVVI_SA
  if (strcmp(dcmManufacturer, "INVALID")==0)
  {
    const double inv = -1;
    fileNumberForPlaneIFrameJ.set(&inv); // laminate with invalid value

    std::map<int,int> series;

    for (int i = 0; i < timeFrames * planesPerFrame; i++) 
    {
      series[seriesNumbers[i]] = 0;
    }

    int index = 0;
    for (std::map<int,int>::iterator it = series.begin(); it != series.end(); ++it,++index)
    {
      it->second = index;
    }

    for (int i = 0; i < timeFrames * planesPerFrame; i++) 
    {
      int col = int(frame(i,0)-1);
      int row = seriesNumbers[i] - minSeriesNumber;

#ifdef _DEBUG
      mafLogMessage("DEBUG >>>>>>>>> Row from %d began %d",row,series[seriesNumbers[i]]);
#endif // _DEBUG

      row = series[seriesNumbers[i]];

      if (row >= planesPerFrame || col >= timeFrames)
      {
        wxMessageBox("ERROR during processing Cardiac MRI");
        vtkDEL(directoryReader);

        return MAF_ERROR;
      }
      fileNumberForPlaneIFrameJ.put(row,col,i);
    }
  }
  m_FileNumberForPlaneIFrameJ = fileNumberForPlaneIFrameJ;
  ////

  if (planesPerFrame == 1)
  {
    // just one timevarying plane: skip correction
    m_FileNumberForPlaneIFrameJIdPlane = m_FileNumberForPlaneIFrameJ;
  } 
  else
  {
    // correction 

    vnl_vector<double> proj(4);
    proj(0) = abs(dot_product(xVersorsSingleFrame.get_row(0), xVersorsSingleFrame.get_row(1)));
    proj(1) = abs(dot_product(xVersorsSingleFrame.get_row(0), yVersorsSingleFrame.get_row(1)));
    proj(2) = abs(dot_product(yVersorsSingleFrame.get_row(0), xVersorsSingleFrame.get_row(1)));
    proj(3) = abs(dot_product(yVersorsSingleFrame.get_row(0), yVersorsSingleFrame.get_row(1)));

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


    //     % store in idx and idy planes with versor parallel to the rotation 
    //     axis in xVersors or yVersors
    //     if( idx_mode==1 || idx_mode==2)
    //       Vmode = xVersors(1,:);
    //     else
    //       Vmode = yVersors(1,:);
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
    
     
    //   %  for each plane verify which versor is almost parallel to the Vmode versor
    //   %  and store the index in idx or idy
    // 
    //   idy = [];
    //   for i = 1:planesPerFrame
    //     proj = abs([dot(Vmode,xVersors(i,:)); dot(Vmode,yVersors(i,:))]);
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
      } 
      else
      {
        idy.push_back(i);  
      }
    }
    
    //   % initializing new coordinates ImagePositionPatient and
    //   % ImageOrientationPatient
    //   newPositionSingleFrame  = zeros(size(positions));
    //   newXVersorsSingleFrame = zeros(size(xVersors));
    //   newYVersorsSingleFrame = zeros(size(yVersors));
    //   newImageSizeSingleFrame  = zeros(size(imageSize));
    // 
    //   % variables 
    //   rotateFlag = zeros([planesPerFrame 1]);
    //   fliplr_flag = zeros([planesPerFrame 1]);
    //   flipud_flag = zeros([planesPerFrame 1]);
    // 


    /*
    % correction
    for i = idx
      newImageSizeSingleFrame(i,:) = [imageSize(i,2) imageSize(i,1)];
      if xVersors(i,1)>0
    %         rimg(:,:,i) = imrotate(img,90);
          newPositionSingleFrame(i,:) = positions(i,:) + imageSize(i,1)*spacing(1)*xVersors(i,:);
          newXVersorsSingleFrame(i,:) = yVersors(i,:);
          newYVersorsSingleFrame(i,:) = -xVersors(i,:);
          rotateFlag(i) = 90;
      else
    %         rimg(:,:,i) = imrotate(fliplr(img),90);
          newPositionSingleFrame(i,:) = positions(i,:);
          newXVersorsSingleFrame(i,:) = yVersors(i,:);
          newYVersorsSingleFrame(i,:) = xVersors(i,:);
          rotateFlag(i) = 90;
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

    vnl_matrix<double> rotateFlag(planesPerFrame,1,0.0);
    vnl_matrix<double> flipLeftRightFlag(planesPerFrame,1,0.0);
    vnl_matrix<double> flipUpDownFlag(planesPerFrame,1,0.0);

    for (int index = 0; index < idx.size(); index++) 
    {
      int idx_ith_value = idx[index];
      
      newImageSizeSingleFrame(idx_ith_value,0) = imageSizeSingleFrame(idx_ith_value, 1);
      newImageSizeSingleFrame(idx_ith_value,1) = imageSizeSingleFrame(idx_ith_value, 0);
         
      if (xVersorsSingleFrame(idx_ith_value,0) > 0)
      {
        for (int col = 0; col < 3; col++) 
        {
          newPositionSingleFrame(idx_ith_value,col) = positionSingleFrame(idx_ith_value,col) + \
            imageSizeSingleFrame(idx_ith_value,0) * \
            spacing[0] * xVersorsSingleFrame(idx_ith_value,col);
        }
          
        newXVersorsSingleFrame.set_row(idx_ith_value , yVersorsSingleFrame.get_row(idx_ith_value));
        newYVersorsSingleFrame.set_row(idx_ith_value , -xVersorsSingleFrame.get_row(idx_ith_value));
        rotateFlag(idx_ith_value,0) = 90;
    } 
    else
    {
        newPositionSingleFrame.set_row(idx_ith_value , positionSingleFrame.get_row(idx_ith_value));
        newXVersorsSingleFrame.set_row(idx_ith_value , yVersorsSingleFrame.get_row(idx_ith_value));
        newYVersorsSingleFrame.set_row(idx_ith_value , xVersorsSingleFrame.get_row(idx_ith_value));
        rotateFlag(idx_ith_value,0) = 90;
        flipLeftRightFlag(idx_ith_value,0) = 1;
    }
    }

    /*
    for i = idy
      newImageSizeSingleFrame(i,:) = imageSize(i,:);
      if yVersors(i,1)>0
      %         rimg(:,:,i) = flipud(img);
        newPositionSingleFrame(i,:) = positions(i,:) + imageSize(i,2)*spacing(2)*yVersors(i,:);
        newXVersorsSingleFrame(i,:) = xVersors(i,:);
        newYVersorsSingleFrame(i,:) = -yVersors(i,:);
        flipud_flag(i) = 1;
      else
    %         rimg(:,:,i) = img;
        newPositionSingleFrame(i,:) = positions(i,:);
        newXVersorsSingleFrame(i,:) = xVersors(i,:);
        newYVersorsSingleFrame(i,:) = yVersors(i,:);
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
        for (int col = 0; col < 3; col++) 
        {
          newPositionSingleFrame(idy_ith_value,col) = positionSingleFrame(idy_ith_value,col) + \
            imageSizeSingleFrame(idy_ith_value,1) * \
            spacing[1] * yVersorsSingleFrame(idy_ith_value,col);
        }

        newXVersorsSingleFrame.set_row(idy_ith_value , xVersorsSingleFrame.get_row(idy_ith_value));
        newYVersorsSingleFrame.set_row(idy_ith_value , -yVersorsSingleFrame.get_row(idy_ith_value));
        flipUpDownFlag(idy_ith_value,0) = 1;
      } 
      else
      {
        newPositionSingleFrame.set_row(idy_ith_value , positionSingleFrame.get_row(idy_ith_value));
        newXVersorsSingleFrame.set_row(idy_ith_value , xVersorsSingleFrame.get_row(idy_ith_value));
        newYVersorsSingleFrame.set_row(idy_ith_value , yVersorsSingleFrame.get_row(idy_ith_value));
      }
    }

    //   %% PLANES ORDERING
    // 
    //   % relationship between adjacent planes
    //   V = [(1:planesPerFrame)' newXVersorsSingleFrame];
    // 
    //   % choose plane 1 as tentative solution
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


    // % compute angle between plane 1 and other planes
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
    
    assert(true);

  //   % find planes adjacent to plane 1 with a 10% tolerance with respect to the 
  //   % theoretical angle. Adjacent plane is unique when plane 1 is an extreme plane
  //   id_theta = find(theta<1.1*(pi/planesPerFrame));
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

    if (id_theta.size() == 0 || id_theta.size() == 1)
	{    
		// id_theta.size() == 0 happens with adaro62-20090715-15853 test data   
	} 
    else
    {
	  double tmpi = id_theta[0];

      vnl_vector<double> row0 = V.get_row(id_theta[0]);
      vnl_vector<double> row1 = Vidx.get_row(0);
      vnl_vector<double> row2 = V.get_row(id_theta[1]);

      vnl_matrix<double> tmp(3, Vidx.cols());
      tmp.set_row(0, row0);
      tmp.set_row(1, row1);
      tmp.set_row(2, row2);

      Vidx=tmp;
    }	


    //   V(id_theta,:)=[] (START);
    vnl_matrix<double> inputMatrix = V;
    vector<double> rowsToRemoveVector = id_theta;
    vnl_matrix<double> outputMatrix;

    RemoveRows(inputMatrix, rowsToRemoveVector, outputMatrix);
    
    V = outputMatrix;
     
    theta.clear();

  // % find planes adjacent to first and last plane in V 
  // while ~isempty(V)
  //       % compute the angle between the last indexed plane and the others
  //       for i = 1:size(V,1)
  //         theta(i) = acos(dot(Vidx(end,2:4),V(i,2:4)));
  //       end

	// if id_theta.size() == (0,0) skip this reordering 
	
	bool skip = false;

	int size = id_theta.size();

	if (id_theta.size() == 0)
	{
		// this happens with adaro62-20090715-15853 test data
		skip = true;	
	}
	
	if (!skip)
	{
		// while ~isempty(V)
		
		while (V.rows() != 0 && V.cols() != 0)
		{
			
			for (int i = 0; i < V.rows(); i++) 
			{
				vnl_vector<double> v0(3,0);
				vnl_vector<double> v1(3,0);
				for (int j = 1; j < 4; j++) 
				{
				v0.put(j-1, Vidx(Vidx.rows() - 1, j));
				v1.put(j-1, V(i, j));
				}
	   
				double dot = dot_product(v0,v1);
				theta.push_back(acos(dot_product(v0,v1)));
			}
	           
			// id_theta = find(theta<1.1*(pi/planesPerFrame));         
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
				int row = id_theta[i - Vidx.rows()];

				tmp3.set_row(i, V.get_row(row));
				}

				Vidx = tmp3;
	             
	    
				//   V(id_theta,:)=[];
				inputMatrix = V;
				rowsToRemoveVector = id_theta;
	             
				RemoveRows(inputMatrix, rowsToRemoveVector, outputMatrix);

				V = outputMatrix;
	         
				theta.clear();

			}
	           
	//          % compute the angle between first indexed plane and the others
	//            for i = 1:size(V,1)
	//              theta(i) = acos(dot(Vidx(1,2:4),V(i,2:4)));
	//            end
	           
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

	              
	              
				double dot = dot_product(v0,v1);
				theta.push_back(acos(dot_product(v0,v1)));
			}

			//  id_theta = find(theta<1.1*(pi/planesPerFrame));

			for (int i = 0; i < theta.size(); i++) 
			{
	              
			}
	           
			id_theta.clear();

			for (int i = 0; i < theta.size(); i++) 
			{
        // Commented by Losi 2011/08/04 to open "NIG003 - PIAAL" series
				//if (theta[i] < 1.1 * (vnl_math::pi/planesPerFrame))
				{
				id_theta.push_back(i);
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

				vnl_matrix<double> inputMatrix = V;
				vector<double> rowsToRemoveVector = id_theta;
				vnl_matrix<double> outputMatrix;

				RemoveRows(inputMatrix, rowsToRemoveVector, outputMatrix);

				V = outputMatrix;
	              
				theta.clear();
			}

			theta.clear();

		}
	}

	// id_plane contains the new plane ordering
    vnl_vector<double> id_plane = Vidx.get_column(0);

	int size1 = id_plane.size();
	// this happens with SE10 test data and adaro62-20090715-15853 test data with orthogonal slices:
	// i skip plane reordering and use default order instead
	if (id_plane.size() != planesPerFrame)
	{
		int size = idx.size();

		// this happens with SE10 test data and adaro62-20090715-15853 test data with orthogonal slices:
		// i skip plane reordering and use default order instead
		if (idx.size() == planesPerFrame)
		{
			vnl_vector<double> idx_vnl(planesPerFrame);

			for (int i = 0; i < idx.size(); i++)
			{
				idx_vnl.put(i, idx[i]);
			}

			id_plane = idx_vnl;
		}
		// this happens with SA test data with (? to check) orthogonal slices:
		// i skip plane reordering and use increasing order from 0 to [planesPerFrame - 1] instead
		else if (idx.size() == 0)
		{
			vnl_vector<double> idx_vnl(planesPerFrame);

			for (int i = 0; i < planesPerFrame; i++)
			{
				idx_vnl.put(i, i);
			}

			id_plane = idx_vnl;
		}

	}

    vnl_matrix<double> fileNumberForPlaneIFrameJIdPlane;

    // order variables following planes order
    ExtractRows(fileNumberForPlaneIFrameJ, id_plane, fileNumberForPlaneIFrameJIdPlane);
      
  //     % original DICOM positions/orientations
  //     % - positions: patient position (from DICOM)
  //     % - xVersors, yVersors: image versors (from DICOM)
  //     % - imageSize: image size (from DICOM)
  //     

    vnl_matrix<double> xVersorsSingleFrameIdPlane;
    vnl_matrix<double> yVersorsSingleFrameIdPlane;
    vnl_matrix<double> positionSingleFrameIdPlane;
    vnl_matrix<double> imageSizeSingleFrameIdPlane;

    ExtractRows(positionSingleFrame, id_plane, positionSingleFrameIdPlane);
    
    ExtractRows(xVersorsSingleFrame, id_plane, xVersorsSingleFrameIdPlane);

    ExtractRows(yVersorsSingleFrame, id_plane, yVersorsSingleFrameIdPlane);
     
    ExtractRows(imageSizeSingleFrame, id_plane, imageSizeSingleFrameIdPlane);
     
    
    // Output (updated) dicom parameters 
    
    vnl_matrix<double> newXVersorsSingleFrameIdPlane;
    vnl_matrix<double> newYVersorsSingleFrameIdPlane;
    vnl_matrix<double> newPositionSingleFrameIdPlane;
    vnl_matrix<double> newImageSizeSingleFrameIdPlane;

    ExtractRows(newPositionSingleFrame, id_plane, newPositionSingleFrameIdPlane);
     
    ExtractRows(newXVersorsSingleFrame, id_plane, newXVersorsSingleFrameIdPlane);
     
    ExtractRows(newYVersorsSingleFrame, id_plane, newYVersorsSingleFrameIdPlane);
     
    ExtractRows(newImageSizeSingleFrame, id_plane, newImageSizeSingleFrameIdPlane);
     
    vnl_matrix<double> rotateFlagIdPlane;
    vnl_matrix<double> flipLeftRightFlagIdPlane;
    vnl_matrix<double> flipUpDownFlagIdPlane;
   
    ExtractRows(rotateFlag, id_plane, rotateFlagIdPlane);
     
    ExtractRows(flipLeftRightFlag, id_plane, flipLeftRightFlagIdPlane);
     
    ExtractRows(flipUpDownFlag, id_plane, flipUpDownFlagIdPlane);
    
    m_FileNumberForPlaneIFrameJIdPlane = fileNumberForPlaneIFrameJIdPlane;
    m_PositionSingleFrameIdPlane = positionSingleFrameIdPlane;
    m_XVersorsSingleFrameIdPlane = xVersorsSingleFrameIdPlane;
    m_YVersorsSingleFrameIdPlane = yVersorsSingleFrameIdPlane;
    m_ImageSizeSingleFrameIdPlane = imageSizeSingleFrameIdPlane;

    m_NewPositionSingleFrameIdPlane = newPositionSingleFrameIdPlane;
    m_NewXVersorsSingleFrameIdPlane = newXVersorsSingleFrameIdPlane;
    m_NewYVersorsSingleFrameIdPlane = newYVersorsSingleFrameIdPlane;
    m_NewImageSizeSingleFrameIdPlane = newImageSizeSingleFrameIdPlane;

    m_RotateFlagIdPlane = rotateFlagIdPlane;
    m_FlipLeftRightFlagIdPlane = flipLeftRightFlagIdPlane;
    m_FlipUpDownFlagIdPlane = flipUpDownFlagIdPlane;

  }
  
  m_DicomLocalFileNamesVector = dicomABSFileNamesVector;
  m_TimeFrames = timeFrames;
  m_PlanesPerFrame = planesPerFrame;
  
  vnl_vector<double> tmpSpacing(3,0);

  for (int i = 0; i < 3; i++) 
  {
    tmpSpacing.put(i,spacing[i]);
  }
  
  m_Spacing = tmpSpacing;

  vtkDEL(directoryReader);
	
  return MAF_OK;
}

void mafDicomCardiacMRIHelper::RemoveRows\
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


void mafDicomCardiacMRIHelper::ExtractRows\
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

void mafDicomCardiacMRIHelper::ExtractRows( vnl_matrix<double> &inputMatrix,\
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

void mafDicomCardiacMRIHelper::SetInputDicomDirectoryABSPath( mafString inputDicomDirectoryABSPath )
{
  m_InputDicomDirectoryABSPath = inputDicomDirectoryABSPath;
}

mafString mafDicomCardiacMRIHelper::GetInputDicomDirectoryABSPath()
{
  return m_InputDicomDirectoryABSPath;
}