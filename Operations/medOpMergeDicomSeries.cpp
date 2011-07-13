/*=========================================================================
Program:   MED
Module:    $RCSfile: medOpMergeDicomSeries.cpp,v $
Language:  C++
Date:      $Date: 2011-07-13 12:53:06 $
Version:   $Revision: 1.1.2.3 $
Authors:   Alberto Losi
==========================================================================
Copyright (c) 2009
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)
=========================================================================*/

#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "wx/listimpl.cpp"
#include "wx/busyinfo.h"

#include "medOpMergeDicomSeries.h"
#include "medOpImporterDicomOffis.h"
#include "medGUIDicomSettings.h"
#include "vtkDicomUnPacker.h"

//BES: 4.7.2009 - VS 2008 cannot compile it due to the following error
//C:\MAF\Medical\Libraries\Offis\Sources\dcmtk-3.5.4\config\include\dcmtk/config/cfwin32.h(362) : error C2371: 'ssize_t' : redefinition; different basic types
//  C:\MAF\openMAF\Libraries\wxWin\Sources\include\wx/defs.h(1018) : see declaration of 'ssize_t'
//this here is to patch it
#if _MSC_VER >= 1500
#define ssize_t VS2008_ssize_t_HACK 
#endif

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

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpMergeDicomSeries);
//----------------------------------------------------------------------------


medOpMergeDicomSeries::medOpMergeDicomSeries(wxString label):
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType = OPTYPE_OP;
  m_DicomSeriesInstanceUID = -1;
}
//----------------------------------------------------------------------------
medOpMergeDicomSeries::~medOpMergeDicomSeries()
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
mafOp *medOpMergeDicomSeries::Copy()
//----------------------------------------------------------------------------
{
  // returns a copy of this operation
	medOpMergeDicomSeries *op = new medOpMergeDicomSeries(m_Label);
	return op;
}
//----------------------------------------------------------------------------
void medOpMergeDicomSeries::OpRun()
//----------------------------------------------------------------------------
{
  bool result = true;
	do 
	{
		if (m_DicomDirectoryABSFileName == "")
		{	
      // Get the selected dicom directory
			wxString lastDicomDir = ((medGUIDicomSettings*)GetSetting())->GetLastDicomDir();

			if (lastDicomDir == "UNEDFINED_m_LastDicomDir")
			{
				wxString defaultPath = (mafGetApplicationDirectory()+"/data/external/").c_str();
				lastDicomDir = defaultPath;		
			};
			wxDirDialog dialog(NULL,"", lastDicomDir,wxRESIZE_BORDER,wxDefaultPosition);
			dialog.SetReturnCode(wxID_OK);
			int ret_code = dialog.ShowModal();

			if (ret_code == wxID_OK)
			{
				wxString path = dialog.GetPath();
				((medGUIDicomSettings*)GetSetting())->SetLastDicomDir(path);
				m_DicomDirectoryABSFileName = path.c_str();

        int m_DicomSeriesInstanceUID = wxGetNumberFromUser(_("Series number"),_("Insert the series  ID for the output merged series"),"Input",999,0,999);
        if( m_DicomSeriesInstanceUID == -1)
        {
          mafEventMacro(mafEvent(this,OP_RUN_CANCEL)); 
          return;
        }
        // call the "renaming" function
        RanameSeriesAndManufacturer(m_DicomDirectoryABSFileName,m_DicomSeriesInstanceUID);
			}
			else
			{
				mafEventMacro(mafEvent(this,OP_RUN_CANCEL)); 
				return;
			}

		}
		else
		{
			m_DicomDirectoryABSFileName = "";
      bool result = false;
		}
	} while(!result);

  mafEventMacro(mafEvent(this,OP_RUN_OK)); 
  
}
//----------------------------------------------------------------------------
bool medOpMergeDicomSeries::RanameSeriesAndManufacturer(const char *dicomDirABSPath, int dicomSeriesUID)
//----------------------------------------------------------------------------
{
  // This function change the value for the dicom tag
  // that refers to SeriesID and Manufacturer in order to merge dicom data represnting single images
  // into a data that represents a volume

  long progress;
  int sliceNum = -1;
  double lastZPos = 0;
  long int dcmInstanceNumber = -1;
  long int dcmCardiacNumberOfImages = -1;
  double dcmTriggerTime = -1.0;
  double dcmImageOrientationPatient[6] = {0.0,0.0,0.0,0.0,0.0,0.0};
  double dcmImagePositionPatient[3] = {0.0,0.0,0.0};
  bool enableToRead = true; //true for test mode
  bool errorOccurred = false;
  double lastDistance = 0;
  mafString lastFileName = "";

  DcmFileFormat dicomImg;    

  vtkNEW(m_DICOMDirectoryReader);
  if (m_DICOMDirectoryReader->Open(dicomDirABSPath) == 0)
  {
    if(!this->m_TestMode)
    {
      wxMessageBox(wxString::Format("Directory <%s> can not be opened",dicomDirABSPath),"Warning!!");
    }
    return false;
  }

  if (!this->m_TestMode)
  {
    mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
  }

  int i = -1;

  time_t start,end;

  wxBusyInfo *busyInfo = NULL;

  wxString busyMessage = "Reading DICOM directory: please wait";

  if (!m_TestMode)
  {
    busyInfo = new wxBusyInfo(busyMessage);
  }

  // foreach dicom directory file
  for (i=0; i < m_DICOMDirectoryReader->GetNumberOfFiles(); i++)
  {
    time(&start);

    if ((strcmp(m_DICOMDirectoryReader->GetFile(i),".") == 0) ||\
      (strcmp(m_DICOMDirectoryReader->GetFile(i),"..") == 0)) 
    {
      // skip non dicom files
      continue;
    }
    else
    {
      sliceNum++;
      mafString currentSliceABSFileName = "";
      mafString currentSliceLocalFileName = m_DICOMDirectoryReader->GetFile(i);

      currentSliceABSFileName.Append(dicomDirABSPath);
      currentSliceABSFileName.Append("\\");
      currentSliceABSFileName.Append(currentSliceLocalFileName);

      DJDecoderRegistration::registerCodecs(); // register JPEG codecs
      DcmRLEDecoderRegistration ::registerCodecs(OFFalse, OFFalse,OFFalse); // register RLE codecs
      OFCondition status = dicomImg.loadFile(currentSliceABSFileName);//load data into offis structure

      if (!status.good())
      {
        if(!this->m_TestMode)
        {
          wxLogMessage(wxString::Format("File <%s> can not be opened",currentSliceABSFileName));
          errorOccurred = true;
          sliceNum--;
        }
        continue;
      }

      DcmDataset *dicomDataset = dicomImg.getDataset();//obtain dataset information from dicom file (loaded into memory)

      // decompress data set if compressed
      OFCondition error = dicomDataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL);

      DJDecoderRegistration::cleanup(); // deregister JPEG codecs
      DcmRLEDecoderRegistration::cleanup();

      if (!error.good())
      {
        wxLogMessage(wxString::Format("Error decoding the image <%s>",currentSliceABSFileName));
        errorOccurred = true;
        //return false;
        continue;
      }

      // change dicom SeriesUID tag to 999 for all images in order to merge them to a single series
      assert(dicomSeriesUID != -1);
      wxString strDcmSeriesInstanceUID = wxString::Format("%i",dicomSeriesUID);
      const char *dcmSeriesInstanceUID = (const char*)strDcmSeriesInstanceUID.mb_str();
      status = dicomDataset->putAndInsertString(DCM_SeriesInstanceUID,dcmSeriesInstanceUID);

      // change the manufacturer to invalid in order to "inform" dicom importer that this data was merged by this op
      const char *dcmManufacturer = "INVALID";
      status = dicomDataset->putAndInsertString(DCM_Manufacturer,dcmManufacturer);

      // Must load image data before save!
      long int val_long;
      dicomDataset->findAndGetLongInt(DCM_BitsAllocated,val_long);

      const Uint16 *dicom_buf_short = NULL; 
      const Uint8* dicom_buf_char = NULL; 
      if (val_long==16) 
      { 
        dicomDataset->findAndGetUint16Array(DCM_PixelData, dicom_buf_short); 
      } 
      else 
      { 
        dicomDataset->findAndGetUint8Array(DCM_PixelData, dicom_buf_char); 
      } 

      status = dicomImg.saveFile(currentSliceABSFileName);

      mafLogMessage("Modified file %s",currentSliceABSFileName);
      mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,long((double(i)/double(m_DICOMDirectoryReader->GetNumberOfFiles()))*100)));

    }  
  }
  if (!this->m_TestMode)
  {
    mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));
  }
  delete busyInfo;
}