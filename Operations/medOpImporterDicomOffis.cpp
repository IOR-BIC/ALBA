/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpImporterDicomOffis.cpp,v $
Language:  C++
Date:      $Date: 2009-05-05 13:36:20 $
Version:   $Revision: 1.1.2.12 $
Authors:   Matteo Giacomoni, Roberto Mucci (DCMTK)
==========================================================================
Copyright (c) 2002/2007
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
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "wx/listimpl.cpp"
#include "wx/busyinfo.h"

#include "medOpImporterDicomOffis.h"

#include "medGUIWizard.h"
#include "medGUIWizardPage.h"
#include "mafGUIValidator.h"
#include "mmdMouse.h"
#include "mmiDICOMImporterInteractor.h"
#include "mafTagArray.h"
#include "mafRWI.h"
#include "mafVMEImage.h"
#include "mafVMEVolumeGray.h"
#include "mafGUICheckListBox.h"
#include "medGUIDicomSettings.h"
#include "mafGUIButton.h"
#include "medGUISettingsAdvanced.h"
#include "mafVMEItemVTK.h"
#include "mafDataVector.h"
#include "mafTransform.h"
#include "mafTransformFrame.h"
#include "mafSmartPointer.h"

#include "vtkMAFVolumeResample.h"
#include "vtkMAFSmartPointer.h"
#include "vtkDicomUnPacker.h"
#include "vtkDirectory.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkPlaneSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkTexture.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkOutlineFilter.h"
#include "vtkImageData.h"
#include "vtkPolyData.h"
#include "vtkStructuredPoints.h"
#include "vtkProbeFilter.h"
#include "vtkRenderer.h"
#include "vtkMAFRGSliceAccumulate.h"
#include "vtkTransform.h"
#include "vtkImageData.h"

#include "vtkDataSetWriter.h"
#include "vtkRectilinearGrid.h"
#include "vtkPointData.h"
#include "vtkShortArray.h"

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
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


#define INCLUDE_CSTDLIB
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"




//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpImporterDicomOffis);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// constants :
//----------------------------------------------------------------------------
#define FIRST_SELECTION 0
#define START_PROGRESS_BAR 0

enum DICOM_IMPORTER_GUI_ID
{
	ID_FIRST = medGUIWizard::ID_LAST,
	ID_STUDY,
	ID_CROP_BUTTON,
	ID_UNDO_CROP_BUTTON,
	ID_BUILD_STEP,
	ID_BUILD_BUTTON,
	ID_CANCEL,
	ID_PATIENT_NAME,
	ID_PATIENT_ID,
	ID_SURGEON_NAME,
	//ID_TYPE_DICOM,
	ID_SCAN_TIME,
	ID_SCAN_SLICE,
	ID_VOLUME_NAME,
	ID_VOLUME_SIDE,
};
enum VOLUME_SIDE
{
  NON_VALID_SIDE = 0,
	LEFT_SIDE,
	RIGHT_SIDE,
};
enum DICOM_IMPORTER_MODALITY
{
	CROP_SELECTED,
	ADD_CROP_ITEM,
	GIZMO_NOT_EXIST,
	GIZMO_RESIZING,
	GIZMO_DONE
};

int compareX(const medImporterDICOMListElement **arg1,const medImporterDICOMListElement **arg2);
int compareY(const medImporterDICOMListElement **arg1,const medImporterDICOMListElement **arg2);
int compareZ(const medImporterDICOMListElement **arg1,const medImporterDICOMListElement **arg2);
int compareTriggerTime(const medImporterDICOMListElement **arg1,const medImporterDICOMListElement **arg2);
int compareImageNumber(const medImporterDICOMListElement **arg1,const medImporterDICOMListElement **arg2);

WX_DEFINE_LIST(medListDicomFiles);
//----------------------------------------------------------------------------
medOpImporterDicomOffis::medOpImporterDicomOffis(wxString label):
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType = OPTYPE_IMPORTER;
	m_Canundo = true;

  for (int i = 0; i < 6; i++) 
    m_DicomBounds[i] = 0;

  m_PatientPosition = "";
  m_ImagePositionPatient[3] = 0;
  m_ImageOrientationPatient[6] = 0;

	m_Wizard = NULL;
	m_LoadPage = NULL;
	m_CropPage = NULL;
	m_BuildPage = NULL;

	m_BuildGuiLeft = NULL;
	m_CropGuiLeft = NULL;
	m_LoadGuiLeft = NULL;
  m_LoadGuiUnderLeft = NULL;
  m_CropGuiCenter = NULL;

  m_TimeScannerLoadPage = NULL;
  m_TimeScannerCropPage = NULL;
  m_TimeScannerBuildPage = NULL;

	m_DirectoryReader = NULL;
	m_SliceLookupTable = NULL;
	m_SlicePlane = NULL;
	m_SliceMapper = NULL;
	m_SliceTexture = NULL;
	m_SliceActor = NULL;
  m_SliceActorInCropPage = NULL;
	m_CropPlane = NULL;
	m_CropActor = NULL;

	m_NumberOfStudy = 0;
  m_NumberOfSlices = -1;
	m_StudyListbox = NULL;
  m_ListSelected = NULL;

	m_FilesList = NULL;

  m_DicomDirectory = "";
	m_DicomTypeRead = -1;
	m_DicomModality = 0;
	m_BuildStepValue = 0;

	m_SliceScannerBuildPage = NULL;
	m_SliceScannerCropPage = NULL;
	m_SliceScannerLoadPage = NULL;

	m_TagArray = NULL;

	m_BoxCorrect = false;
	m_CropFlag = false;
  m_CroppedExetuted = false;
  m_ResampleFlag = false;
  m_SideToBeDragged = 0; 

	m_GizmoStatus = GIZMO_NOT_EXIST;

	m_Image = NULL;
	m_Volume = NULL;

	m_SortAxes = 2;

	m_VolumeName = "";

	m_VolumeSide=NON_VALID_SIDE;

	m_DicomModalityListBox = NULL;

  m_DicomInteractor = NULL;

  m_CurrentSlice = VTK_INT_MAX;

  m_ResampleFlag = FALSE;
}
//----------------------------------------------------------------------------
medOpImporterDicomOffis::~medOpImporterDicomOffis()
//----------------------------------------------------------------------------
{
	vtkDEL(m_SliceActor);
	vtkDEL(m_SliceLookupTable);

	cppDEL(m_Wizard);
	mafDEL(m_TagArray);

	m_Volume = NULL;
	m_Image = NULL;
  mafDEL(m_Output);
}
//----------------------------------------------------------------------------
mafOp *medOpImporterDicomOffis::Copy()
//----------------------------------------------------------------------------
{
  medOpImporterDicomOffis *importer = new medOpImporterDicomOffis(m_Label);
  importer->m_ResampleFlag = m_ResampleFlag;
  importer->m_DicomDirectory = m_DicomDirectory;
	return importer;
}
//----------------------------------------------------------------------------
void medOpImporterDicomOffis::OpRun()
//----------------------------------------------------------------------------
{
  m_BuildStepValue = ((medGUIDicomSettings*)GetSetting())->GetBuildStep();

	CreateGui();
	CreatePipeline();

	m_Wizard = new medGUIWizard(_("DICOM IMPORTER"));
	m_Wizard->SetListener(this);

	CreateLoadPage();
	CreateCropPage();
	CreateBuildPage();
	EnableSliceSlider(false);
	EnableTimeSlider(false);

	//Create a chain between pages
	m_LoadPage->SetNextPage(m_CropPage);
	m_CropPage->SetNextPage(m_BuildPage);
	m_Wizard->SetFirstPage(m_LoadPage);
  
  bool result = false;
  bool firstTime = true;
  do 
  {
	  if (m_DicomDirectory == "")
	  {
		  wxString path = (mafGetApplicationDirectory()+"/data/external/").c_str();
		  wxDirDialog dialog(m_Wizard->GetParent(),"", path, 0, m_Wizard->GetPosition());
		  dialog.SetReturnCode(wxID_OK);
		  int ret_code = dialog.ShowModal();
		  if (ret_code == wxID_OK)
		  {
		    path = dialog.GetPath();
		    m_DicomDirectory = path.c_str();
		    GuiUpdate();
		    result = OpenDir();
		  }
	    else
	    {
	      OpStop(OP_RUN_CANCEL);
	      return;
	    }
	  }
    else
    {
      GuiUpdate();
      result = OpenDir();
      if (result == false)
      {
        m_DicomDirectory = "";
      }
    }
  } while(!result);

	if(m_Wizard->Run())
	{
    int result;
		if(m_DicomTypeRead != medGUIDicomSettings::ID_CMRI_MODALITY)
			result = BuildVolume();
		else
			result = BuildVolumeCineMRI();
		OpStop(result);
	}
	else
		OpStop(OP_RUN_CANCEL);
}
//----------------------------------------------------------------------------
void medOpImporterDicomOffis::OpDo()
//----------------------------------------------------------------------------
{
	if(m_Output != NULL)
  {
    m_Output->ReparentTo(m_Input);
  }
}
//----------------------------------------------------------------------------
void medOpImporterDicomOffis::OpStop(int result)
//----------------------------------------------------------------------------
{
  Destroy();
	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void medOpImporterDicomOffis::Destroy()
//----------------------------------------------------------------------------
{
  if(m_DicomInteractor)
    m_Mouse->RemoveObserver(m_DicomInteractor);

  if(!this->m_TestMode)
  {
    //close dialog
    for (int i=0; i < m_NumberOfStudy;i++)
    {
      ((medListDicomFiles *)m_StudyListbox->GetClientData(i))->DeleteContents(TRUE);
      ((medListDicomFiles *)m_StudyListbox->GetClientData(i))->Clear();
    }
  }

  std::map<mafString,medListDicomFiles*>::iterator it;
  for ( it=m_DicomMap.begin() ; it != m_DicomMap.end(); it++ )
  {
    m_DicomMap[(*it).first]->DeleteContents(TRUE);
    m_DicomMap[(*it).first]->Clear();
  }

  m_DicomMap.clear();

  if(m_LoadPage)
    m_LoadPage->GetRWI()->m_RenFront->RemoveActor(m_SliceActor);  

  if(m_CropPage)
    //m_CropPage->GetRWI()->m_RenFront->RemoveActor(m_SliceActor);
    m_CropPage->GetRWI()->m_RenFront->RemoveActor(m_CropActor);   

  if(m_BuildPage)
    m_BuildPage->GetRWI()->m_RenFront->RemoveActor(m_SliceActor);

  vtkDEL(m_SliceTexture);
  vtkDEL(m_DirectoryReader);
  vtkDEL(m_SliceLookupTable);
  vtkDEL(m_SlicePlane);
  vtkDEL(m_SliceMapper);
  vtkDEL(m_SliceActor);
  vtkDEL(m_CropPlane);
  vtkDEL(m_CropActor);

  mafDEL(m_TagArray);
  mafDEL(m_DicomInteractor);

  cppDEL(m_LoadPage);
  cppDEL(m_CropPage);
  cppDEL(m_BuildPage);

  cppDEL(m_FilesList);
}
//----------------------------------------------------------------------------
int medOpImporterDicomOffis::BuildVolume()
//----------------------------------------------------------------------------
{
	int step;

	if(m_BuildStepValue == 0)
		step = 1;
	else if (m_BuildStepValue == 1)
		step = m_BuildStepValue << 1;
	else
		step = m_BuildStepValue + 1;

	int n_slices = m_NumberOfSlices / step;

  if(m_NumberOfSlices % step != 0)
  {
    n_slices+=1;
  }

  if(!this->m_TestMode)
  {
    wxBusyInfo wait_info("Building volume: please wait");
    mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
  }

	vtkMAFSmartPointer<vtkMAFRGSliceAccumulate> accumulate;
	accumulate->SetNumberOfSlices(n_slices);
	accumulate->BuildVolumeOnAxes(m_SortAxes);

  long progress = 0;
	int count,s_count;
	for (count = 0, s_count = 0; count < m_NumberOfSlices; count += step)
	{
		if (s_count == n_slices) {break;}
    CreateSlice(count);
		accumulate->SetSlice(s_count,m_SliceTexture->GetInput());
		s_count++;
     
    if(!this->m_TestMode)
    {
      progress = count * 100 / m_DirectoryReader->GetNumberOfFiles();
		  mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,progress));
    }
	}
  if(!this->m_TestMode)
  {
    mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));
  }
	ImportDicomTags();
	if(m_NumberOfSlices == 1 || n_slices == 1)
	{
		//imageCase
		mafNEW(m_Image);
		m_Image->GetTagArray()->DeepCopy(m_TagArray);
		mafDEL(m_TagArray);

		accumulate->Update();
     
    double spacing[3];
    vtkMAFSmartPointer<vtkImageData> im;
    im->DeepCopy(m_SliceTexture->GetInput());
    if(!this->m_TestMode)
    {
      double scaleFactor;
      medGUISettingsAdvanced *guiAdv = new medGUISettingsAdvanced(this);

      if(guiAdv->GetConversionType() == medGUIDicomSettings::NONE)
      {
        scaleFactor = 1.0;
      }
      else if(guiAdv->GetConversionType() == medGUIDicomSettings::mm2m)
      {
        scaleFactor = 0.001;
      }
      im->GetSpacing(spacing);
      spacing[0]*=scaleFactor;
      spacing[1]*=scaleFactor;
      spacing[2]*=scaleFactor;
      im->SetSpacing(spacing);
      im->Update();
    }
    
		m_Image->SetData(im,0);

		mafTagItem tag_Nature;
		tag_Nature.SetName("VME_NATURE");
		tag_Nature.SetValue("NATURAL");
		m_Image->GetTagArray()->SetTag(tag_Nature);

		mafTagItem tag_Surgeon;
		tag_Surgeon.SetName("SURGEON_NAME");
		tag_Surgeon.SetValue(m_SurgeonName.GetCStr());
		m_Image->GetTagArray()->SetTag(tag_Surgeon);

    if(!this->m_TestMode)
    {
      if(((medGUIDicomSettings*)GetSetting())->EnableChangeSide())
      {
		    mafTagItem tagSide;
		    tagSide.SetName("VOLUME_SIDE");
		    if(m_VolumeSide==LEFT_SIDE)
			    tagSide.SetValue("LEFT");
		    else if(m_VolumeSide==RIGHT_SIDE)
			    tagSide.SetValue("RIGHT");
        else if(m_VolumeSide==NON_VALID_SIDE)
          tagSide.SetValue("NON_VALID_SIDE");
		    m_Image->GetTagArray()->SetTag(tagSide);
      }
    }

		//Nome VME = CTDir + IDStudio
		//wxString name = m_DicomDirectory + " - " + m_StudyListbox->GetString(m_StudyListbox->GetSelection());
    /*if(!m_PatientName.IsEmpty())
      m_VolumeName=m_PatientName;
    else
    {
      //Patient ID + Study ID
      m_VolumeName=m_Identifier;
      m_VolumeName.Append(" -  ");
      if(!this->m_TestMode)
        m_VolumeName.Append(m_StudyListbox->GetString(m_StudyListbox->GetSelection()));
    }*/
		m_Image->SetName(m_VolumeName);
    if(m_Image != NULL)
    {
      m_Output = m_Image;
    }
	}
	else if(m_NumberOfSlices > 1)
	{
		//Copy inside the first VME item of m_Volume the CT volume and Dicom's tags
		mafNEW(m_Volume);
		m_Volume->GetTagArray()->DeepCopy(m_TagArray);
		mafDEL(m_TagArray);

		accumulate->Update();

    vtkMAFSmartPointer<vtkRectilinearGrid> rg_out;
    rg_out->DeepCopy(accumulate->GetOutput());
    rg_out->Update();

    if(!this->m_TestMode)
    {
      medGUISettingsAdvanced *guiAdv = new medGUISettingsAdvanced(this);

      double scaleFactor;
      if(guiAdv->GetConversionType() == medGUIDicomSettings::NONE)
      {
        scaleFactor = 1.0;
      }
      else if(guiAdv->GetConversionType() == medGUIDicomSettings::mm2m)
      {
        scaleFactor = 0.001;
      }

      vtkDataArray *daVector[3] = {rg_out->GetXCoordinates(), rg_out->GetYCoordinates(), rg_out->GetZCoordinates()};

      for (int arrayId = 0; arrayId<3; arrayId ++)
      {

        int numTuples = daVector[arrayId]->GetNumberOfTuples();

        for (int tupleId = 0; tupleId<numTuples; tupleId++)
        {
          double oldVal = daVector[arrayId]->GetComponent(tupleId, 0);
          daVector[arrayId]->SetComponent(tupleId, 0, oldVal * scaleFactor);
        }
      }
      rg_out->Modified();
    }
    m_Volume->SetDataByDetaching(rg_out,0);

    if(m_ResampleFlag == TRUE)
    {
      ResampleVolume();
    }
    
		mafTagItem tag_Nature;
		tag_Nature.SetName("VME_NATURE");
		tag_Nature.SetValue("NATURAL");
		m_Volume->GetTagArray()->SetTag(tag_Nature);

		mafTagItem tag_Surgeon;
		tag_Surgeon.SetName("SURGEON_NAME");
		tag_Surgeon.SetValue(m_SurgeonName.GetCStr());
		m_Volume->GetTagArray()->SetTag(tag_Surgeon);

    if(!this->m_TestMode)
    {
      if(((medGUIDicomSettings*)GetSetting())->EnableChangeSide())
      {
		    mafTagItem tagSide;
		    tagSide.SetName("VOLUME_SIDE");
		    if(m_VolumeSide==LEFT_SIDE)
			    tagSide.SetValue("LEFT");
		    else if(m_VolumeSide==RIGHT_SIDE)
			    tagSide.SetValue("RIGHT");
        else if(m_VolumeSide==NON_VALID_SIDE)
          tagSide.SetValue("NON_VALID_SIDE");
		    m_Volume->GetTagArray()->SetTag(tagSide);
      }
    }

		//Nome VME = CTDir + IDStudio
		//wxString name = m_DicomDirectory + " - " + m_StudyListbox->GetString(m_StudyListbox->GetSelection());		
    /*if(!m_PatientName.IsEmpty())
      m_VolumeName=m_PatientName;
    else
    {
      //Patient ID + Study ID
      m_VolumeName=m_Identifier;
      m_VolumeName.Append(" -  ");
      if(!this->m_TestMode)
        m_VolumeName.Append(m_StudyListbox->GetString(m_StudyListbox->GetSelection()));
    }*/
		m_Volume->SetName(m_VolumeName);
	}
  if(m_Volume != NULL)
  {
    m_Output = m_Volume;
  }
  return OP_RUN_OK;
}
//----------------------------------------------------------------------------
int medOpImporterDicomOffis::BuildVolumeCineMRI()
//----------------------------------------------------------------------------
{
	int step;

	if(m_BuildStepValue == 0)
		step = 1;
	else if (m_BuildStepValue == 1)
		step = m_BuildStepValue << 1;
	else
		step = m_BuildStepValue + 1;

	int n_slices = m_NumberOfSlices / step;

  if(m_NumberOfSlices % step != 0)
  {
    n_slices+=1;
  }

	wxBusyInfo wait_info("Building volume: please wait");

	// create the time varying vme
	mafNEW(m_Volume);

	// for every timestamp
	for (int ts = 0; ts < m_NumberOfTimeFrames; ts++)
	{
		// Build item at timestamp ts    
		vtkMAFSmartPointer<vtkMAFRGSliceAccumulate> accumulate;
		accumulate->SetNumberOfSlices(n_slices);

		// always build the volume on z-axis
		accumulate->BuildVolumeOnAxes(m_SortAxes);

		// get the time stamp from the dicom tag;
		// timestamp is in ms
		int probeHeigthId = 0;    
		int tsImageId = GetImageId(ts, probeHeigthId);
		if (tsImageId == -1) 
		{
			assert(FALSE);
		}

		medImporterDICOMListElement *element0;
		element0 = (medImporterDICOMListElement *)m_ListSelected->Item(tsImageId)->GetData();
		//double tsDouble = ((double) (element0->GetTriggerTime())) / 1000.0;
		mafTimeStamp tsDouble = (mafTimeStamp)(element0->GetTriggerTime());

		for (int sourceVolumeSliceId = 0, targetVolumeSliceId = 0; sourceVolumeSliceId < m_NumberOfSlices; sourceVolumeSliceId += step)
		{
			if (targetVolumeSliceId == n_slices) {break;}

			// show the current slice
			int currImageId = GetImageId(ts, sourceVolumeSliceId);
			if (currImageId != -1) 
			{
				// update v_texture ivar
				//ShowSlice(currImageId);
        CreateSlice(currImageId);
			}

			accumulate->SetSlice(targetVolumeSliceId, m_SliceTexture->GetInput());
			targetVolumeSliceId++;
		}
		accumulate->Update();

    vtkMAFSmartPointer<vtkRectilinearGrid> rg_out;
    rg_out->DeepCopy(accumulate->GetOutput());
    rg_out->Update();

    if(!this->m_TestMode)
    {
      medGUISettingsAdvanced *guiAdv = new medGUISettingsAdvanced(this);

      double scaleFactor;
      if(guiAdv->GetConversionType() == medGUIDicomSettings::NONE)
      {
        scaleFactor = 1.0;
      }
      else if(guiAdv->GetConversionType() == medGUIDicomSettings::mm2m)
      {
        scaleFactor = 0.001;
      }

      vtkDataArray *daVector[3] = {rg_out->GetXCoordinates(), rg_out->GetYCoordinates(), rg_out->GetZCoordinates()};

      for (int arrayId = 0; arrayId<3; arrayId ++)
      {

        int numTuples = daVector[arrayId]->GetNumberOfTuples();

        for (int tupleId = 0; tupleId<numTuples; tupleId++)
        {
          double oldVal = daVector[arrayId]->GetComponent(tupleId, 0);
          daVector[arrayId]->SetComponent(tupleId, 0, oldVal * scaleFactor);
        }
      }
      rg_out->Modified();
    }

		m_Volume->SetDataByDetaching(rg_out,tsDouble);

    if(m_ResampleFlag == TRUE)
    {
      ResampleVolume();
    }
	}

	// update m_tag_array ivar
	ImportDicomTags();
	m_Volume->GetTagArray()->DeepCopy(m_TagArray);
	vtkDEL(m_TagArray);

	mafTagItem tag_Nature;
	tag_Nature.SetName("VME_NATURE");
	tag_Nature.SetValue("NATURAL");
	m_Volume->GetTagArray()->SetTag(tag_Nature);

	mafTagItem tag_Surgeon;
	tag_Surgeon.SetName("SURGEON_NAME");
	tag_Surgeon.SetValue(m_SurgeonName.GetCStr());
	m_Volume->GetTagArray()->SetTag(tag_Surgeon);

  if(!this->m_TestMode)
  {
    if(((medGUIDicomSettings*)GetSetting())->EnableChangeSide())
    {
	    mafTagItem tagSide;
	    tagSide.SetName("VOLUME_SIDE");
	    if(m_VolumeSide==LEFT_SIDE)
		    tagSide.SetValue("LEFT");
	    else if(m_VolumeSide==RIGHT_SIDE)
		    tagSide.SetValue("RIGHT");
      else if(m_VolumeSide==NON_VALID_SIDE)
        tagSide.SetValue("NON_VALID_SIDE");
	    m_Volume->GetTagArray()->SetTag(tagSide);
    }
  }

	m_Volume->SetName(m_VolumeName);

  if(m_Volume != NULL)
  {
    m_Output = m_Volume;
  }

  return OP_RUN_OK;

}
//----------------------------------------------------------------------------
void medOpImporterDicomOffis::CreateLoadPage()
//----------------------------------------------------------------------------
{
	m_LoadPage = new medGUIWizardPage(m_Wizard,medUSEGUI|medUSERWI,_("First Step"));
	m_LoadGuiLeft = new mafGUI(this);
  m_LoadGuiUnderLeft = new mafGUI(this);

	m_SliceScannerLoadPage=m_LoadGuiLeft->Slider(ID_SCAN_SLICE,_("slice #"),&m_CurrentSlice,0,m_CurrentSlice,"",((medGUIDicomSettings*)GetSetting())->EnableNumberOfSlice());
  m_SliceScannerLoadPage->SetPageSize(1);
  if(((medGUIDicomSettings*)GetSetting())->EnableNumberOfTime())
  {
    m_TimeScannerLoadPage=m_LoadGuiLeft->Slider(ID_SCAN_TIME,_("time "),&m_CurrentTime,0,VTK_INT_MAX);
    m_TimeScannerLoadPage->SetPageSize(1);
  }

  m_StudyListbox = m_LoadGuiUnderLeft->ListBox(ID_STUDY,_("study id"),70,"",wxLB_HSCROLL,300);
  
  m_LoadGuiLeft->FitGui();
  m_LoadGuiUnderLeft->FitGui();
  m_LoadPage->AddGuiLowerLeft(m_LoadGuiLeft);
  m_LoadPage->AddGuiLowerUnderLeft(m_LoadGuiUnderLeft);
  
  
	m_LoadPage->GetRWI()->CameraSet(CAMERA_CT);
	m_LoadPage->GetRWI()->m_RwiBase->SetMouse(m_Mouse);
	m_LoadPage->GetRWI()->m_RenFront->AddActor(m_SliceActor);
}
//----------------------------------------------------------------------------
void medOpImporterDicomOffis::CreateCropPage()
//----------------------------------------------------------------------------
{
	m_CropPage = new medGUIWizardPage(m_Wizard,medUSEGUI|medUSERWI,_("Second Step"));
	m_CropGuiLeft = new mafGUI(this);
  m_CropGuiCenter = new mafGUI(this);

	m_SliceScannerCropPage=m_CropGuiLeft->Slider(ID_SCAN_SLICE,_("slice #"),&m_CurrentSlice,0,VTK_INT_MAX,"",((medGUIDicomSettings*)GetSetting())->EnableNumberOfSlice());
  m_SliceScannerCropPage->SetPageSize(1);
  if(((medGUIDicomSettings*)GetSetting())->EnableNumberOfTime())
  {
    m_TimeScannerCropPage=m_CropGuiLeft->Slider(ID_SCAN_TIME,_("time "),&m_CurrentTime,0,VTK_INT_MAX);
    m_TimeScannerCropPage->SetPageSize(1);
  }

  if(((medGUIDicomSettings*)GetSetting())->EnableChangeSide())
  {
    wxString sideChoices[3] = {_("non-valid"),_("Left"),_("Right")};
    m_CropGuiCenter->Combo(ID_VOLUME_SIDE,_("volume side"),&m_VolumeSide,3,sideChoices);
  }

  m_CropGuiLeft->FitGui();
  m_CropGuiCenter->FitGui();
  m_CropPage->AddGuiLowerLeft(m_CropGuiLeft);
	m_CropPage->AddGuiLowerCenter(m_CropGuiCenter);

	m_CropPage->GetRWI()->CameraSet(CAMERA_CT);
	m_CropPage->GetRWI()->m_RwiBase->SetMouse(m_Mouse);
  
	m_CropPage->GetRWI()->m_RenFront->AddActor(m_SliceActor);
  m_CropPage->GetRWI()->m_RenFront->AddActor(m_CropActor);
}
//----------------------------------------------------------------------------
void medOpImporterDicomOffis::CreateBuildPage()
//----------------------------------------------------------------------------
{
	m_BuildPage = new medGUIWizardPage(m_Wizard,medUSEGUI|medUSERWI,_("Third Step"));
	m_BuildGuiLeft = new mafGUI(this);
  m_BuildGuiCenter = new mafGUI(this);

	m_SliceScannerBuildPage=m_BuildGuiLeft->Slider(ID_SCAN_SLICE,_("slice #"),&m_CurrentSlice,0,VTK_INT_MAX,"",((medGUIDicomSettings*)GetSetting())->EnableNumberOfSlice());
  m_SliceScannerBuildPage->SetPageSize(1);
  
  if(((medGUIDicomSettings*)GetSetting())->EnableNumberOfTime())
  {
	  m_TimeScannerBuildPage=m_BuildGuiLeft->Slider(ID_SCAN_TIME,_("time "),&m_CurrentTime,0,VTK_INT_MAX);
    m_TimeScannerBuildPage->SetPageSize(1);
  }

  m_BuildGuiCenter->Label(_("volume name"));
  m_BuildGuiCenter->String(ID_VOLUME_NAME,"",&m_VolumeName);
  
  m_BuildGuiLeft->FitGui();
  m_BuildGuiCenter->FitGui();
	m_BuildPage->AddGuiLowerLeft(m_BuildGuiLeft);
  m_BuildPage->AddGuiLowerUnderLeft(m_BuildGuiCenter);

	m_BuildPage->GetRWI()->CameraSet(CAMERA_CT);
	m_BuildPage->GetRWI()->m_RwiBase->SetMouse(m_Mouse);
	m_BuildPage->GetRWI()->m_RenFront->AddActor(m_SliceActor);
}
//----------------------------------------------------------------------------
void medOpImporterDicomOffis::GuiUpdate()
//----------------------------------------------------------------------------
{
	m_LoadGuiLeft->Update();
  m_LoadGuiUnderLeft->Update();

	m_CropGuiLeft->Update();
  m_CropGuiCenter->Update();

	m_BuildGuiLeft->Update();
  m_BuildGuiCenter->Update();
}
//----------------------------------------------------------------------------
void medOpImporterDicomOffis::CreateGui()
//----------------------------------------------------------------------------
{
	
}
//----------------------------------------------------------------------------
bool medOpImporterDicomOffis::OpenDir()
//----------------------------------------------------------------------------
{
  ResetStructure();
  // scan dicom directory
  if (!BuildDicomFileList(m_DicomDirectory.GetCStr()))
  {
    return false;
  }
  if(!this->m_TestMode)
  {
    if(m_NumberOfStudy>0)
    {
      if(m_NumberOfStudy == 1)
      {
        m_StudyListbox->SetSelection(FIRST_SELECTION);
        OnEvent(&mafEvent(this, ID_STUDY));
      }
      if(((medGUIDicomSettings*)GetSetting())->AutoCropPosition())
      {
        AutoPositionCropPlane();
      }
      else
      {
        m_CropPlane->SetOrigin(0.0,0.0,0.0);
        m_CropPlane->SetPoint1(m_DicomBounds[1]-m_DicomBounds[0],0.0,0.0);
        m_CropPlane->SetPoint2(0.0,m_DicomBounds[3]-m_DicomBounds[2],0.0);
        m_CropPage->GetRWI()->CameraReset();
      }
      m_BoxCorrect=true;

      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------
void medOpImporterDicomOffis::ReadDicom() 
//----------------------------------------------------------------------------
{
  if(!this->m_TestMode)
  {
    int sel = 0;
    sel = m_StudyListbox->GetSelection();
    m_ListSelected = (medListDicomFiles *)m_StudyListbox->GetClientData(sel);
  }
  else
  {
    mafString sel;
    sel = m_DicomMap.begin()->first;
    m_ListSelected = m_DicomMap[sel];
  }

  // sort dicom slices
  if(m_ListSelected->GetCount() > 1)
  {
    double item1_pos[3],item2_pos[3],d[3];
    medImporterDICOMListElement *element1;
    medImporterDICOMListElement *element2;
    element1 = (medImporterDICOMListElement *)m_ListSelected->Item(0)->GetData();
    element2 = (medImporterDICOMListElement *)m_ListSelected->Item(1)->GetData();
    item1_pos[0] = element1->GetCoordinate(0);
    item1_pos[1] = element1->GetCoordinate(1);
    item1_pos[2] = element1->GetCoordinate(2);
    item2_pos[0] = element2->GetCoordinate(0);
    item2_pos[1] = element2->GetCoordinate(1);
    item2_pos[2] = element2->GetCoordinate(2);
    d[0] = fabs(item1_pos[0] - item2_pos[0]);
    d[1] = fabs(item1_pos[1] - item2_pos[1]);
    d[2] = fabs(item1_pos[2] - item2_pos[2]);
    if(d[0] > d[1] && d[0] > d[2])
      m_SortAxes = 0;
    else if(d[1] > d[0] && d[1] > d[2])
      m_SortAxes = 1;
    else if(d[2] > d[0] && d[2] > d[1])
      m_SortAxes = 2;
  }
  switch (m_SortAxes)
  {
  case 0:
    m_ListSelected->Sort(compareX);
    break;
  case 1:
    m_ListSelected->Sort(compareY);
    break;
  case 2:
    m_ListSelected->Sort(compareZ);
    break;
  }

  m_NumberOfTimeFrames = ((medImporterDICOMListElement *)m_ListSelected->Item(0)->GetData())->GetNumberOfImages();
  if(m_DicomTypeRead == medGUIDicomSettings::ID_CMRI_MODALITY) //If cMRI
    m_NumberOfSlices = m_ListSelected->GetCount() / m_NumberOfTimeFrames;
  else
    m_NumberOfSlices = m_ListSelected->GetCount();
  // reset the current slice number to view the first slice
  m_CurrentSlice = 0;
  m_CurrentTime = 0;
  m_CropFlag = false;
  int currImageId = GetImageId(m_CurrentTime, m_CurrentSlice);

  if(!this->m_TestMode)
  {
    if (currImageId != -1) 
    {
      // show the selected slice
      CreateSlice(currImageId);
      ShowSlice();
      CameraReset();
      ResetSliders();
      CameraUpdate();
    }
  }

  //modified by STEFY 9-7-2003(begin)
  ImportDicomTags();
  mafTagItem *patient_name;
  mafTagItem *patient_id;

  const char* p_name;
  double p_id = 0;

  bool position = m_TagArray->IsTagPresent("PatientsName");
  if (position)
  {
    patient_name = m_TagArray->GetTag("PatientsName");
    p_name = patient_name->GetValue();
  }
  else 
    p_name = NULL;

  position = m_TagArray->IsTagPresent("PatientID");
  if (position)
  {
    patient_id = m_TagArray->GetTag("PatientID");
    p_id = patient_id->GetValueAsDouble();
    m_Identifier = mafString(p_id);
  }
  if (p_name)
    m_PatientName = p_name;

  int tmp = m_PatientName.FindChr('^');
  if(tmp != -1 && tmp >= 0 && tmp < m_PatientName.GetSize())
    m_PatientName[tmp] = ' ';

  /*if(!this->m_TestMode)
  {
    wxString tmp_name;

    tmp_name.Append(m_PatientName.GetCStr());
    tmp_name.Append(m_Identifier.GetCStr());
    tmp_name=tmp_name+m_StudyListbox->GetString(m_StudyListbox->GetSelection());
    //m_StudyListbox->SetString(m_StudyListbox->GetSelection(),tmp_name);
    //m_StudyListbox->Update();
    GuiUpdate();
    }*/
}

//----------------------------------------------------------------------------
void medOpImporterDicomOffis::OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId())
		{
		case ID_VOLUME_SIDE:
			{
				if(((medGUIDicomSettings*)GetSetting())->AutoCropPosition())
					AutoPositionCropPlane();
			}
			break;
		case medGUIWizard::MED_WIZARD_CHANGE_PAGE:
			{
				if(m_Wizard->GetCurrentPage()==m_LoadPage && m_NumberOfStudy<1)//From Load page to Crop Page
				{
					m_Wizard->EnableChangePageOff();
					wxMessageBox(_("No study found!"));
					return;
				}
				else
				{
					m_Wizard->EnableChangePageOn();
          m_CropPage->UpdateActor();
				}

        if (m_Wizard->GetCurrentPage()==m_CropPage)//From Crop page to build page
        {
          if (e->GetBool())
          {
            if(m_CropPage)
            Crop();
            m_BuildPage->UpdateActor();
          } 
          else
          {
            m_LoadPage->UpdateActor();
          }
        }

        if (m_Wizard->GetCurrentPage()==m_BuildPage && (!e->GetBool()))//From build page to crop page
        {
          UndoCrop();
          m_CropPage->UpdateActor();
        }

				if (m_Wizard->GetCurrentPage()==m_CropPage)//From Crop page to any other
				{
						m_CropActor->VisibilityOff();
				}
				else
				{
					if(!m_CroppedExetuted)
						m_CropActor->VisibilityOn();
					else
						m_CropActor->VisibilityOff();
				}
        GuiUpdate();
			}
			break;
		case ID_UNDO_CROP_BUTTON:
			{
				UndoCrop();
			}
			break;
		case ID_STUDY:
			{
        if (m_VolumeName.Compare(m_StudyListbox->GetString(m_StudyListbox->GetSelection())) != 0)
        {     
          m_VolumeName = m_StudyListbox->GetString(m_StudyListbox->GetSelection());
          // m_VolumeName.Append(" -  ");
          if(!this->m_TestMode)
          {
				    m_BuildGuiLeft->Update();
				    EnableSliceSlider(true);
				    if(m_DicomTypeRead == medGUIDicomSettings::ID_CMRI_MODALITY)//If cMRI
				    {
					    EnableTimeSlider(true);
				    }
          }
          ReadDicom();
        }
			}
			break;
			case MOUSE_DOWN:
				{
					if(m_CroppedExetuted==false)
					{
						if (m_Wizard->GetCurrentPage()==m_CropPage)
					{
						//long handle_id = e->GetArg();
						double pos[3];
						vtkPoints *p = (vtkPoints *)e->GetVtkObj();
						p->GetPoint(0,pos);

						//calcola altezza rettangolo
						double b[6];
						m_CropPlane->GetOutput()->GetBounds(b);
						double dx = (b[1] - b[0]) / 5;
						double dy = (b[3] - b[2]) / 5;

						double O[3], P1[3], P2[3];
						//Modified by Matteo 21/07/2006
						//Caso di default P1 in alto a SX e P2 in basso a DX
						m_CropPlane->GetOrigin(O);
						m_CropPlane->GetPoint1(P1);
						m_CropPlane->GetPoint2(P2);
						//Se non siamo nel caso di default modifichiamo in modo da ritornare in quel caso
						if(P2[0]<P1[0] && P2[1]<P1[1])//Caso P1 in basso a DX e P2 in alto a SX
						{
							O[0] = P2[0];
							O[1] = P1[1];
							double tempx=P1[0];
							double tempy=P1[1];
							P1[0] = P2[0];
							P1[1] = P2[1];
							P2[0] = tempx;
							P2[1] = tempy;
							m_CropPlane->SetOrigin(O);
							m_CropPlane->SetPoint1(P1);
							m_CropPlane->SetPoint2(P2);
						}
						else if(P1[0]<P2[0] && P1[1]>P2[1])//Caso P1 in basso a SX e P2 in alto a DX
						{
							O[0] = P1[0];
							O[1] = P1[1];
							double tempy=P1[1];
							P1[1] = P2[1];
							P2[1] = tempy;
							m_CropPlane->SetOrigin(O);
							m_CropPlane->SetPoint1(P1);
							m_CropPlane->SetPoint2(P2);
						}
						else if(P1[0]>P2[0] && P1[1]<P2[1])//Caso P1 in alto a DX e P2 in basso a SX
						{
							O[0] = P2[0];
							O[1] = P2[1];
							double tempx=P1[0];
							P1[0] = P2[0];
							P2[0] = tempx;
							m_CropPlane->SetOrigin(O);
							m_CropPlane->SetPoint1(P1);
							m_CropPlane->SetPoint2(P2);
						}
						//End Matteo
						if (m_GizmoStatus == GIZMO_NOT_EXIST)
						{
							m_GizmoStatus = GIZMO_RESIZING;
							m_CropActor->VisibilityOn();

							pos[2] = 0;
							m_CropPlane->SetOrigin(pos);
							m_CropPlane->SetPoint1(pos[0], pos[1], pos[2]);
							m_CropPlane->SetPoint2(pos[0], pos[1], pos[2]);
						}
						else if (m_GizmoStatus == GIZMO_DONE)
						{
							//	  8------------1----------2--->x
							//		|												|
							//		7												3
							//		|												|
							//		6------------5----------4
							//		|
							//	  v y

							if (P1[0] + dx/2 <= pos[0] &&  pos[0] <= P2[0] - dx/2 &&
								P1[1] - dy/2 <= pos[1] && pos[1] <= P1[1] + dy/2)
							{
								m_SideToBeDragged = 1;
							}
							else if (P2[0] - dx/2 <= pos[0] && pos[0] <= P2[0] + dx/2 &&
								P1[1] - dy/2 <= pos[1] && pos[1] <= P1[1] + dy/2)
							{
								m_SideToBeDragged = 2;
							}
							else if (P2[0] - dx/2 <= pos[0] && pos[0] <= P2[0] + dx/2 &&
								P2[1] - dy/2 >= pos[1] && pos[1] >= P1[1] + dy/2)
							{
								m_SideToBeDragged = 3;
							}
							else if (P2[0] - dx/2 <= pos[0] && pos[0] <= P2[0] + dx/2 &&
								P2[1] - dy/2 <= pos[1] && pos[1] <= P2[1] + dy/2)
							{
								m_SideToBeDragged = 4;
							}
							else if (P1[0] + dx/2 <= pos[0] && pos[0] <= P2[0] - dx/2 &&
								P2[1] - dy/2 <= pos[1] && pos[1] <= P2[1] + dy/2)
							{
								m_SideToBeDragged = 5;
							}
							else if (P1[0] - dx/2 <= pos[0] && pos[0] <= P1[0] + dx/2 &&
								P2[1] - dy/2 <= pos[1] && pos[1] <= P2[1] +dy/2)
							{
								m_SideToBeDragged = 6;
							}
							else if (P1[0] - dx/2 <= pos[0] && pos[0] <= P1[0] + dx/2 &&
								P2[1] - dy/2 >= pos[1] && pos[1] >= P1[1] + dy/2)
							{
								m_SideToBeDragged = 7;
							}
							else if (P1[0] - dx/2 <= pos[0] && pos[0] <= P1[0] + dx/2 &&
								P1[1] - dy/2 <= pos[1] && pos[1] <= P1[1] + dy/2)
							{
								m_SideToBeDragged = 8;
							}	
							else
								//hai pickato in un punto che non corrisponde a nessun lato
								// => crea un nuovo gizmo
							{
								m_GizmoStatus = GIZMO_RESIZING;
								m_CropActor->VisibilityOn();

								pos[2] = 0;
								m_CropPlane->SetOrigin(pos);
								m_CropPlane->SetPoint1(pos[0], pos[1], pos[2]);
								m_CropPlane->SetPoint2(pos[0], pos[1], pos[2]);
								m_CropPlane->SetXResolution(10);
							}
						}
            m_CropPage->UpdateActor();
            m_CropPage->GetRWI()->CameraUpdate();
					}
					}
				}
				break;
			case MOUSE_MOVE:  //ridimensiona il gizmo
				{
					if(m_CroppedExetuted==false)
					{
						if (m_Wizard->GetCurrentPage()==m_CropPage)
					{
						//long handle_id = e->GetArg();
						double pos[3], oldO[3], oldP1[3], oldP2[3];
						vtkPoints *p = (vtkPoints *)e->GetVtkObj();
						p->GetPoint(0,pos);

						m_CropPlane->GetOrigin(oldO);
						m_CropPlane->GetPoint1(oldP1);
						m_CropPlane->GetPoint2(oldP2);

						if (m_GizmoStatus == GIZMO_RESIZING)
						{
							m_CropPlane->SetPoint1(oldO[0], pos[1], oldP1[2]);
							m_CropPlane->SetPoint2(pos[0], oldO[1], oldP1[2]);
						}
						else if (m_GizmoStatus == GIZMO_DONE)
						{
							if (m_SideToBeDragged == 0)
							{
							}
							else if (m_SideToBeDragged == 1)
								m_CropPlane->SetPoint1(oldP1[0], pos[1], oldP1[2]);
							else if (m_SideToBeDragged == 2)
							{
								m_CropPlane->SetPoint1(oldP1[0], pos[1], oldP1[2]);
								m_CropPlane->SetPoint2(pos[0], oldP2[1], oldP2[2]);
							}
							else if (m_SideToBeDragged == 3)
								m_CropPlane->SetPoint2(pos[0], oldP2[1], oldP2[2]);
							else if (m_SideToBeDragged == 4)
							{
								m_CropPlane->SetOrigin(oldO[0], pos[1], oldO[2]);
								m_CropPlane->SetPoint2(pos[0], pos[1], oldP2[2]);
							}
							else if (m_SideToBeDragged == 5)
							{
								m_CropPlane->SetOrigin(oldO[0], pos[1], oldO[2]);
								m_CropPlane->SetPoint2(oldP2[0], pos[1], oldP2[2]);
							}
							else if (m_SideToBeDragged == 6)
							{
								m_CropPlane->SetOrigin(pos[0], pos[1], oldO[2]);
								m_CropPlane->SetPoint1(pos[0], oldP1[1], oldP2[2]);
								m_CropPlane->SetPoint2(oldP2[0], pos[1], oldP2[2]);
							}
							else if (m_SideToBeDragged == 7)
							{
								m_CropPlane->SetOrigin(pos[0], oldO[1], oldO[2]);
								m_CropPlane->SetPoint1(pos[0], oldP1[1], oldP1[2]);
							}
							else if (m_SideToBeDragged == 8)
							{
								m_CropPlane->SetOrigin(pos[0], oldO[1], oldO[2]);
								m_CropPlane->SetPoint1(pos[0], pos[1], oldP1[2]);
							}
						}
            m_CropPage->GetRWI()->CameraUpdate();
					}
					}
				}
				break;
			case MOUSE_UP:  //blocca il gizmo
				{
					if(m_CroppedExetuted==false)
					{
						if (m_Wizard->GetCurrentPage()==m_CropPage)
					{
						if (m_GizmoStatus == GIZMO_RESIZING)
							m_GizmoStatus = 	GIZMO_DONE;
						else if (m_GizmoStatus == GIZMO_DONE)
							m_SideToBeDragged = 0;

						double p1[3], p2[3], origin[3];
						m_CropPlane->GetPoint1(p1);
						m_CropPlane->GetPoint2(p2);
						m_CropPlane->GetOrigin(origin);

						if( (p1[0] == p2[0] && p1[1] == p2[1] && p1[2] == p2[2])  ||
							(p1[0] == origin[0] && p1[1] == origin[1] && p1[2] == origin[2]) ||
							(p2[0] == origin[0] && p2[1] == origin[1] && p2[2] == origin[2])
							)
						{
							m_BoxCorrect = false;
						}
						else
							m_BoxCorrect = true;   
					}
					}
				}
				break; 
			case ID_SCAN_SLICE:
				{
					// show the current slice
					int currImageId = GetImageId(m_CurrentTime, m_CurrentSlice);
					if (currImageId != -1) 
					{
            CreateSlice(currImageId);
						ShowSlice();
						CameraUpdate();
					}

					m_SliceScannerLoadPage->SetValue(m_CurrentSlice);
					m_SliceScannerLoadPage->Update();
					m_SliceScannerCropPage->SetValue(m_CurrentSlice);
					m_SliceScannerCropPage->Update();
					m_SliceScannerBuildPage->SetValue(m_CurrentSlice);
					m_SliceScannerBuildPage->Update();

					GuiUpdate();
				}
				break;
			case ID_SCAN_TIME:
				{
					// show the current slice
					int currImageId = GetImageId(m_CurrentTime, m_CurrentSlice);
					if (currImageId != -1) 
					{
            CreateSlice(currImageId);
            ShowSlice();
						CameraUpdate();
					}
					m_TimeScannerLoadPage->SetValue(m_CurrentTime);
					m_TimeScannerLoadPage->Update();
					m_TimeScannerCropPage->SetValue(m_CurrentTime);
					m_TimeScannerCropPage->Update();
					m_TimeScannerBuildPage->SetValue(m_CurrentTime);
					m_TimeScannerBuildPage->Update();

					GuiUpdate();
				}
				break;
			case ID_CROP_BUTTON:
				{     
					Crop();
				}
				break;
		}
	}
}
//----------------------------------------------------------------------------
void medOpImporterDicomOffis::UndoCrop()
//----------------------------------------------------------------------------
{
  m_CropFlag = false;
  int currImageId = GetImageId(m_CurrentTime, m_CurrentSlice);
  if (currImageId != -1) 
  {
    CreateSlice(currImageId);
    ShowSlice();
  }
  double diffx,diffy,boundsCamera[6];
  diffx=m_DicomBounds[1]-m_DicomBounds[0];
  diffy=m_DicomBounds[3]-m_DicomBounds[2];
  boundsCamera[0]=0.0;
  boundsCamera[1]=diffx;
  boundsCamera[2]=0.0;
  boundsCamera[3]=diffy;
  boundsCamera[4]=0.0;
  boundsCamera[5]=0.0;
  m_LoadPage->GetRWI()->CameraReset(boundsCamera);
  m_LoadPage->GetRWI()->CameraUpdate();
  m_CropPage->GetRWI()->CameraReset(boundsCamera);
  m_CropPage->GetRWI()->CameraUpdate();
  m_BuildPage->GetRWI()->CameraReset(boundsCamera);
  m_BuildPage->GetRWI()->CameraUpdate();
  //m_CropGuiLeft->Enable(ID_UNDO_CROP_BUTTON,false);
  m_CropActor->VisibilityOn();
  m_CroppedExetuted=false;
  CameraUpdate();
}
//----------------------------------------------------------------------------
void medOpImporterDicomOffis::Crop()
//----------------------------------------------------------------------------
{
	if( !m_BoxCorrect )
	{
		wxMessageBox("Error on selecting the box");
		return;
	}

	m_CropFlag = true;
  int currImageId = GetImageId(m_CurrentTime, m_CurrentSlice);
  if (currImageId != -1) 
  {
    CreateSlice(currImageId);
    ShowSlice();
  }
	m_CropActor->VisibilityOff();
	m_CroppedExetuted=true;
	double diffx,diffy,boundsCamera[6];
	diffx=m_DicomBounds[1]-m_DicomBounds[0];
	diffy=m_DicomBounds[3]-m_DicomBounds[2];
	boundsCamera[0]=0.0;
	boundsCamera[1]=diffx;
	boundsCamera[2]=0.0;
	boundsCamera[3]=diffy;
	boundsCamera[4]=0.0;
	boundsCamera[5]=0.0;

	/*m_CropPlane->SetOrigin(0.0,0.0,0.0);
	m_CropPlane->SetPoint1(diffx,0.0,0.0);
	m_CropPlane->SetPoint2(0.0,diffy,0.0);
	m_CropPlane->Update();*/

	m_CropPage->GetRWI()->CameraReset(boundsCamera);
	m_CropPage->GetRWI()->CameraUpdate();
	//m_CropGuiLeft->Enable(ID_UNDO_CROP_BUTTON,true);

	m_LoadPage->GetRWI()->CameraReset(boundsCamera);
	m_LoadPage->GetRWI()->CameraUpdate();
	m_BuildPage->GetRWI()->CameraReset(boundsCamera);
	m_BuildPage->GetRWI()->CameraUpdate();
}
//----------------------------------------------------------------------------
void medOpImporterDicomOffis::AutoPositionCropPlane()
//----------------------------------------------------------------------------
{
	int currImageId = GetImageId(m_CurrentTime, m_CurrentSlice);

	m_ListSelected->Item(currImageId)->GetData()->GetOutput()->Update();
	m_ListSelected->Item(currImageId)->GetData()->GetOutput()->GetBounds(m_DicomBounds);

	double diffY,diffX;
	diffY=m_DicomBounds[3]-m_DicomBounds[2];
	diffX=m_DicomBounds[1]-m_DicomBounds[0];

	if(m_VolumeSide==RIGHT_SIDE)
	{
		m_CropPlane->SetOrigin(0.0,diffY/4,0.0);
		m_CropPlane->SetPoint1(diffX/2,diffY/4,0.0);
		m_CropPlane->SetPoint2(0.0,(diffY/4)*3,0.0);
		m_CropPlane->Update();
	}
	else if(m_VolumeSide==LEFT_SIDE)
	{
 		m_CropPlane->SetOrigin(m_DicomBounds[1]-diffX/2,diffY/4,0.0);
		m_CropPlane->SetPoint1(m_DicomBounds[1],diffY/4,0.0);
		m_CropPlane->SetPoint2(m_DicomBounds[1]-diffX/2,(diffY/4)*3,0.0);
		m_CropPlane->Update();
	}
  else if (m_VolumeSide==NON_VALID_SIDE)
  {
    m_CropPlane->SetOrigin(0.0,0.0,0.0);
    m_CropPlane->SetPoint1(m_DicomBounds[1]-m_DicomBounds[0],0.0,0.0);
    m_CropPlane->SetPoint2(0.0,m_DicomBounds[3]-m_DicomBounds[2],0.0);
    m_CropPage->GetRWI()->CameraReset();
  }
	
	m_CropPage->GetRWI()->CameraReset();
}
//----------------------------------------------------------------------------
void medOpImporterDicomOffis::CameraUpdate()
//----------------------------------------------------------------------------
{
  m_LoadPage->UpdateActor();
	//m_LoadPage->GetRWI()->CameraUpdate();
  m_CropPage->UpdateActor();
  //m_CropPage->GetRWI()->CameraUpdate();
  m_BuildPage->UpdateActor();
	//m_BuildPage->GetRWI()->CameraUpdate();
  }
//----------------------------------------------------------------------------
void medOpImporterDicomOffis::CameraReset()
//----------------------------------------------------------------------------
{
	m_LoadPage->GetRWI()->CameraReset();
  m_LoadPage->UpdateWindowing();
	m_CropPage->GetRWI()->CameraReset();
  m_CropPage->UpdateWindowing();
	m_BuildPage->GetRWI()->CameraReset();
  m_BuildPage->UpdateWindowing();
}
//----------------------------------------------------------------------------
void medOpImporterDicomOffis::EnableSliceSlider(bool enable)
//----------------------------------------------------------------------------
{
	m_LoadGuiLeft->Enable(ID_SCAN_SLICE,enable);
	m_BuildGuiLeft->Enable(ID_SCAN_SLICE,enable);
	m_CropGuiLeft->Enable(ID_SCAN_SLICE,enable);
}
//----------------------------------------------------------------------------
void medOpImporterDicomOffis::EnableTimeSlider(bool enable)
//----------------------------------------------------------------------------
{
  if(((medGUIDicomSettings*)GetSetting())->EnableNumberOfTime())
  {
	  m_LoadGuiLeft->Enable(ID_SCAN_TIME,enable);
	  m_BuildGuiLeft->Enable(ID_SCAN_TIME,enable);
	  m_CropGuiLeft->Enable(ID_SCAN_TIME,enable);
  }
}
//----------------------------------------------------------------------------
void medOpImporterDicomOffis::CreatePipeline()
//----------------------------------------------------------------------------
{
	vtkNEW(m_DirectoryReader);

	vtkNEW(m_SliceLookupTable);

	vtkNEW(m_SliceTexture);
	m_SliceTexture->InterpolateOn();

	vtkNEW(m_SlicePlane);

	vtkNEW(m_SliceMapper);
	m_SliceMapper->SetInput(m_SlicePlane->GetOutput());

	vtkNEW(m_SliceActor);
	m_SliceActor->SetMapper(m_SliceMapper);
	m_SliceActor->SetTexture(m_SliceTexture); 
	m_SliceActor->VisibilityOff();

	vtkNEW(m_CropPlane);

	vtkMAFSmartPointer<vtkOutlineFilter>	of;
	of->SetInput(((vtkDataSet *)(m_CropPlane->GetOutput())));

	vtkMAFSmartPointer<vtkPolyDataMapper> pdm;
	pdm->SetInput(of->GetOutput());

	vtkNEW(m_CropActor);
	m_CropActor->GetProperty()->SetColor(0.8,0,0);
	m_CropActor->VisibilityOff();
	m_CropActor->SetMapper(pdm);

  if(!this->m_TestMode)
  {
    mafNEW(m_DicomInteractor);
    m_DicomInteractor->SetListener(this);
    m_Mouse->AddObserver(m_DicomInteractor, MCH_INPUT);
  }
}

//----------------------------------------------------------------------------
void medOpImporterDicomOffis::FillListBox(mafString StudyUID)
//----------------------------------------------------------------------------
{
  int row = m_StudyListbox->FindString(StudyUID.GetCStr());
  if (row == -1)
  {
    m_StudyListbox->Append(StudyUID.GetCStr());
    m_StudyListbox->SetClientData(m_NumberOfStudy,(void *)m_FilesList);
  }
}

//----------------------------------------------------------------------------
bool medOpImporterDicomOffis::BuildDicomFileList(const char *dir)
//----------------------------------------------------------------------------
{
  long progress;
  int sliceNum = -1;
  int row, i;
	double slice_pos[3];
	long int imageNumber = -1;
	long int numberOfImages = -1;
	double trigTime = -1.0;
  bool enableToRead = true; //true for test mode
	m_DicomTypeRead = -1;
  DcmFileFormat dicomImg;    

	if (m_DirectoryReader->Open(dir) == 0)
	{
    if(!this->m_TestMode)
    {
      wxMessageBox(wxString::Format("Directory <%s> can not be opened",dir),"Warning!!");
    }
		return false;
	}

  if (!this->m_TestMode)
  {
    mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
	  progress = START_PROGRESS_BAR;
    wxBusyInfo wait_info("Reading DICOM directory: please wait");
  }

	for (i=0; i < m_DirectoryReader->GetNumberOfFiles(); i++)
	{
		if ((strcmp(m_DirectoryReader->GetFile(i),".") == 0) || (strcmp(m_DirectoryReader->GetFile(i),"..") == 0)) 
		{
			continue;
		}
		else
		{
      sliceNum++;
      mafString file = "";
			m_CurrentSliceName = m_DirectoryReader->GetFile(i);
			// Append of the path at the dicom file
			wxString ct_mode;
			file.Append(dir);
			file.Append("\\");
			file.Append(m_CurrentSliceName);

      DJDecoderRegistration::registerCodecs(); // register JPEG codecs
      OFCondition status = dicomImg.loadFile(file);//load data into offis structure

      if (!status.good())
      {
        if(!this->m_TestMode)
        {
          wxMessageBox(wxString::Format("File <%s> can not be opened",file),"Warning!!");
        }
        continue;
      }
      m_FileName = file;

      DcmDataset *ds = dicomImg.getDataset();//obtain dataset information from dicom file (loaded into memory)
 
      // decompress data set if compressed
      ds->chooseRepresentation(EXS_LittleEndianExplicit, NULL);
      DJDecoderRegistration::cleanup(); // deregister JPEG codecs

      //now are used findAndGet* method to get dicom information
      long int val_long;
      ds->findAndGetLongInt(DCM_Columns, val_long);
      int width = val_long;
      ds->findAndGetLongInt(DCM_Rows, val_long);
      int height = val_long;

      ds->findAndGetFloat64(DCM_ImagePositionPatient,m_ImagePositionPatient[2],0);
      ds->findAndGetFloat64(DCM_ImagePositionPatient,m_ImagePositionPatient[1],1);
      ds->findAndGetFloat64(DCM_ImagePositionPatient,m_ImagePositionPatient[0],2);

      ds->findAndGetFloat64(DCM_ImageOrientationPatient,m_ImageOrientationPatient[0],0);
      ds->findAndGetFloat64(DCM_ImageOrientationPatient,m_ImageOrientationPatient[1],1);
      ds->findAndGetFloat64(DCM_ImageOrientationPatient,m_ImageOrientationPatient[2],2);
      ds->findAndGetFloat64(DCM_ImageOrientationPatient,m_ImageOrientationPatient[3],3);
      ds->findAndGetFloat64(DCM_ImageOrientationPatient,m_ImageOrientationPatient[4],4);
      ds->findAndGetFloat64(DCM_ImageOrientationPatient,m_ImageOrientationPatient[5],5);

      double spacing[3];
      spacing[2] = 0;
  
      if(ds->findAndGetFloat64(DCM_PixelSpacing,spacing[0],0).bad())
      {
        //Unable to get element: DCM_PixelSpacing[0];
        spacing[0] = 1.0;// for RGB??
      } 
      if(ds->findAndGetFloat64(DCM_PixelSpacing,spacing[1],1).bad())
      {
        //Unable to get element: DCM_PixelSpacing[0];
        spacing[1] = 1.0;// for RGB??
      } 
      
      double slope,intercept;
      ds->findAndGetFloat64(DCM_RescaleSlope,slope);
      ds->findAndGetFloat64(DCM_RescaleIntercept,intercept);

///////////////////CREATE VTKIMAGEDATA////////////////////////////
      //initialize vtkImageData
      //vtkImageData *imageData = vtkImageData::New();
      vtkMAFSmartPointer<vtkImageData> imageData;
      imageData->SetDimensions(height, width,1);
      imageData->SetWholeExtent(0,width-1,0,height-1,0,0);
      imageData->SetUpdateExtent(0,width-1,0,height-1,0,0);
      imageData->SetExtent(imageData->GetUpdateExtent());
      imageData->SetNumberOfScalarComponents(1);
      imageData->SetOrigin(m_ImagePositionPatient);
      imageData->SetSpacing(spacing);

      ds->findAndGetLongInt(DCM_BitsAllocated,val_long);
      if(val_long==16)
        imageData->SetScalarType(VTK_SHORT);
      else if(val_long==8)
        imageData->SetScalarType(VTK_CHAR);
      imageData->AllocateScalars();
      imageData->GetPointData()->GetScalars()->SetName("Scalars");
      imageData->Update();
      unsigned short *vtk_buf = (unsigned short*)imageData->GetScalarPointer();

      const Uint16 *dicom_buf_short = NULL;
      const Uint8* dicom_buf_char = NULL;
      unsigned long nObject;
      if (val_long==16)
      {
        ds->findAndGetUint16Array(DCM_PixelData, dicom_buf_short);

        // Copy the data from DcmTK to VTK
        memcpy(vtk_buf, dicom_buf_short, val_long/8 * width * height);//copy array of scalars from dicom dataset to vtkImageData scalars array
      }
      else
      {
        ds->findAndGetUint8Array(DCM_PixelData, dicom_buf_char);

        // Copy the data from DcmTK to VTK
        memcpy(vtk_buf, dicom_buf_char, val_long/8 * width * height);//copy array of scalars from dicom dataset to vtkImageData scalars array
      }

      imageData->Update();
////////////////////////////////////////////////////

      const char *mode = "?";
      ds->findAndGetString(DCM_Modality,mode);

      const char *patPos = "?";
      ds->findAndGetString(DCM_PatientPosition,patPos);
      m_PatientPosition = patPos;

      const char *studyUID = "?";
      ds->findAndGetString(DCM_StudyInstanceUID,studyUID);
			
			ct_mode.MakeUpper();
			ct_mode.Trim(FALSE);
			ct_mode.Trim();

      if (!this->m_TestMode)
      {
        enableToRead = ((medGUIDicomSettings*)GetSetting())->EnableToRead((char*)mode);
      }
      if (enableToRead && strcmp((char *)mode, "MR" ) != 0)
			{
        wxString stringMode = mode;
				if(stringMode.Find("SCOUT") != -1)
				{
					continue;
				}

				//row = m_StudyListbox->FindString(studyUID);
        if(m_DicomMap.find(studyUID) == m_DicomMap.end()) 
        {
					// the study is not present into the listbox, so need to create new
					// list of files related to the new studyID
					m_FilesList = new medListDicomFiles;

          m_DicomMap.insert(std::pair<mafString,medListDicomFiles*>(studyUID,m_FilesList));

          if (!this->m_TestMode)
          {
            FillListBox(studyUID);
          }

					//m_StudyListbox->Append(studyUID);
					//m_StudyListbox->SetClientData(m_NumberOfStudy,(void *)m_FilesList);

          ds->findAndGetFloat64(DCM_SliceLocation,slice_pos[2],0);
          ds->findAndGetFloat64(DCM_SliceLocation,slice_pos[1],1);
          ds->findAndGetFloat64(DCM_SliceLocation,slice_pos[0],2);

					m_FilesList->Append(new medImporterDICOMListElement(m_FileName,slice_pos,imageData));
					m_NumberOfStudy++;
				}
				else 
				{
          //for(int k=0;k<3;k++)
          ds->findAndGetFloat64(DCM_SliceLocation,slice_pos[2],0);
          ds->findAndGetFloat64(DCM_SliceLocation,slice_pos[1],1);
          ds->findAndGetFloat64(DCM_SliceLocation,slice_pos[0],2);
          m_DicomMap[studyUID]->Append(new medImporterDICOMListElement(m_FileName,slice_pos,imageData));
				}
			}
			else if ( enableToRead && strcmp( (char *)mode, "MR" ) == 0)
			{
        if(m_DicomMap.find(studyUID) == m_DicomMap.end()) 
				{
					// the study is not present into the listbox, so need to create new
					// list of files related to the new studyID
					m_FilesList = new medListDicomFiles;

          m_DicomMap.insert(std::pair<mafString,medListDicomFiles*>(studyUID,m_FilesList));
          if (!this->m_TestMode)
          {
            FillListBox(studyUID);
          }
					
          ds->findAndGetFloat64(DCM_SliceLocation,slice_pos[2],0);
          ds->findAndGetFloat64(DCM_SliceLocation,slice_pos[1],1);
          ds->findAndGetFloat64(DCM_SliceLocation,slice_pos[0],2);

          ds->findAndGetLongInt(DCM_InstanceNumber,imageNumber);

          ds->findAndGetLongInt(DCM_CardiacNumberOfImages,numberOfImages);
           
					if(numberOfImages>1)
					{
						if (m_DicomTypeRead==-1)
							m_DicomTypeRead=medGUIDicomSettings::ID_CMRI_MODALITY;
						else if(m_DicomTypeRead!=medGUIDicomSettings::ID_CMRI_MODALITY)
						{
							wxString msg = _("cMRI damaged !");
							wxMessageBox(msg,"Confirm", wxOK , NULL);
							return false;
						}
					}
					else
					{
						if (m_DicomTypeRead==-1)
							m_DicomTypeRead=medGUIDicomSettings::ID_MRI_MODALITY;
						else if(m_DicomTypeRead!=medGUIDicomSettings::ID_MRI_MODALITY)
						{
							wxString msg = _("cMRI damaged !");
							wxMessageBox(msg,"Confirm", wxOK , NULL);
							return false;
						}
					}
          ds->findAndGetFloat64(DCM_TriggerTime,trigTime);
					m_FilesList->Append(new medImporterDICOMListElement(m_FileName,slice_pos,imageData,imageNumber, numberOfImages, trigTime));
					m_NumberOfStudy++;
				}
				else 
				{
          ds->findAndGetFloat64(DCM_SliceLocation,slice_pos[2],0);
          ds->findAndGetFloat64(DCM_SliceLocation,slice_pos[1],1);
          ds->findAndGetFloat64(DCM_SliceLocation,slice_pos[0],2);

          ds->findAndGetLongInt(DCM_InstanceNumber,imageNumber);

          ds->findAndGetLongInt(DCM_CardiacNumberOfImages,numberOfImages);

          ds->findAndGetFloat64(DCM_TriggerTime,trigTime);

           m_DicomMap[studyUID]->Append(new medImporterDICOMListElement(m_FileName,slice_pos,imageData,imageNumber,numberOfImages,trigTime));
				}
			}
      if (!this->m_TestMode)
      {
        progress = i * 100 / m_DirectoryReader->GetNumberOfFiles();
			  mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,progress));
      }
      dicomImg.clear();
		}
	}
  
  if (!this->m_TestMode)
  {
    mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));
  }
	if(m_NumberOfStudy == 0)
	{
    if (!this->m_TestMode)
    {
		  wxString msg = "No study found!";
		  wxMessageBox(msg,"Confirm", wxOK , NULL);
    }
    return false;
	}
  else
  {
    return true;
  }
}
//----------------------------------------------------------------------------
void medOpImporterDicomOffis::ResetStructure()
//----------------------------------------------------------------------------
{
  if(!this->m_TestMode)
  {
    EnableSliceSlider(false);
    EnableTimeSlider(false);
  }

  	// delete the previous studies detected and reset the related variables
	for (int i=0; i < m_NumberOfStudy;i++)
	{
		((medListDicomFiles *)m_StudyListbox->GetClientData(i))->DeleteContents(true);
		((medListDicomFiles *)m_StudyListbox->GetClientData(i))->Clear();
	}
  std::map<mafString,medListDicomFiles*>::iterator it;
  for ( it=m_DicomMap.begin() ; it != m_DicomMap.end(); it++ )
  {
    m_DicomMap[(*it).first]->DeleteContents(TRUE);
    m_DicomMap[(*it).first]->Clear();
  }

  m_DicomMap.clear();

  if(!this->m_TestMode)
  {
    m_StudyListbox->Clear();
  }
	m_NumberOfStudy		= 0;
	m_NumberOfSlices	= 0;
	m_CurrentSlice		= 0;
	m_NumberOfTimeFrames = 0;
	m_CurrentTime				= 0; 
	m_DicomTypeRead			= -1;

	m_CropFlag				= false;

	mafYield();
}
//----------------------------------------------------------------------------
void medOpImporterDicomOffis::ResetSliders()
//----------------------------------------------------------------------------
{
  if(m_LoadGuiLeft)
  {
    m_LoadPage->RemoveGuiLowerLeft(m_LoadGuiLeft);
    delete m_LoadGuiLeft;
    m_LoadGuiLeft = new mafGUI(this);
    m_SliceScannerLoadPage=m_LoadGuiLeft->Slider(ID_SCAN_SLICE,_("slice #"),&m_CurrentSlice,0,m_NumberOfSlices-1,"",((medGUIDicomSettings*)GetSetting())->EnableNumberOfSlice());
    m_SliceScannerLoadPage->SetPageSize(1);
    if(((medGUIDicomSettings*)GetSetting())->EnableNumberOfTime())
    {
      m_TimeScannerLoadPage=m_LoadGuiLeft->Slider(ID_SCAN_TIME,_("time "),&m_CurrentTime,0,m_NumberOfTimeFrames);
      m_TimeScannerLoadPage->SetPageSize(1);
    }
    m_LoadPage->AddGuiLowerLeft(m_LoadGuiLeft);
  }

  if(m_CropGuiLeft)
  {
    m_CropPage->RemoveGuiLowerLeft(m_CropGuiLeft);
    delete m_CropGuiLeft;
    m_CropGuiLeft = new mafGUI(this);
    m_SliceScannerCropPage=m_CropGuiLeft->Slider(ID_SCAN_SLICE,_("slice #"),&m_CurrentSlice,0,m_NumberOfSlices-1,"",((medGUIDicomSettings*)GetSetting())->EnableNumberOfSlice());
    m_SliceScannerCropPage->SetPageSize(1);
    if(((medGUIDicomSettings*)GetSetting())->EnableNumberOfTime())
    {
      m_TimeScannerCropPage=m_CropGuiLeft->Slider(ID_SCAN_TIME,_("time "),&m_CurrentTime,0,m_NumberOfTimeFrames);
      m_TimeScannerCropPage->SetPageSize(1);
    }
    m_CropPage->AddGuiLowerLeft(m_CropGuiLeft);
  }


  if(m_BuildGuiLeft)
  {
    m_BuildPage->RemoveGuiLowerLeft(m_BuildGuiLeft);
    delete m_BuildGuiLeft;
    m_BuildGuiLeft = new mafGUI(this);
    m_SliceScannerBuildPage=m_BuildGuiLeft->Slider(ID_SCAN_SLICE,_("slice #"),&m_CurrentSlice,0,m_NumberOfSlices-1,"",((medGUIDicomSettings*)GetSetting())->EnableNumberOfSlice());
    m_SliceScannerBuildPage->SetPageSize(1);
    if(((medGUIDicomSettings*)GetSetting())->EnableNumberOfTime())
    {
      m_TimeScannerBuildPage=m_BuildGuiLeft->Slider(ID_SCAN_TIME,_("time "),&m_CurrentTime,0,m_NumberOfTimeFrames);
      m_TimeScannerBuildPage->SetPageSize(1);
    }
    m_BuildPage->AddGuiLowerLeft(m_BuildGuiLeft);
  }
}
//----------------------------------------------------------------------------
int medOpImporterDicomOffis::GetImageId(int timeId, int heigthId)
//----------------------------------------------------------------------------
{
	/* 

	test:
	ListDicomCineMRIFiles: [0 .. 11] : 12 elements

	heightId

	^
	2 | 8 9 10 11
	1 | 4 5  6  7
	0 | 0 1  2  3
	--------------> timeId
	| 0 1  2  3  

	numberOfImages = 4 (from wxList element);
	maxTimeId = numberOfImages - 1 = 3;

	numberOfDicomSlices = 12 (from wxList);

	numSlicesPerTS = numberOfDicomSlices / numberOfImages = 12 / 4 = 3;
	maxHeigthId = numSlicesPerTS - 1;    

	test:                
	GetImageId(3,2) =    4 * hId + tId = 4*2 + 3 = 11 :) 
	GetImageId(1,2) =    4 * 2 + 1 = 9 :)

	*/


	if (m_DicomTypeRead != medGUIDicomSettings::ID_CMRI_MODALITY)
		return heigthId;

  m_ListSelected = m_DicomMap[m_DicomMap.begin()->first];

	medImporterDICOMListElement *element0;
	element0 = (medImporterDICOMListElement *)m_ListSelected->Item(0)->GetData();

	int numberOfImages =  element0->GetNumberOfImages();

	int numberOfDicomSlices = m_ListSelected->GetCount();

	int numSlicesPerTS = numberOfDicomSlices / numberOfImages;
	assert(numberOfDicomSlices % numberOfImages == 0);

	int maxHeigthId = numSlicesPerTS - 1; // 
	int maxTimeId = numberOfImages - 1; // numberOfImages - 1;

	if (heigthId < 0 || heigthId > maxHeigthId || timeId < 0 || timeId > maxTimeId )
	{
		return -1;
	}

	return (heigthId * numberOfImages + timeId); 
}
//----------------------------------------------------------------------------
void medOpImporterDicomOffis::CreateSlice(int slice_num)
//----------------------------------------------------------------------------
{
	// Description:
	// read the slice number 'slice_num' and generate the texture
	double spacing[3], crop_bounds[6], range[2], loc[3];

	m_ListSelected->Item(slice_num)->GetData()->GetSliceLocation(loc);

	m_ListSelected->Item(slice_num)->GetData()->GetOutput()->Update();
	m_ListSelected->Item(slice_num)->GetData()->GetOutput()->GetBounds(m_DicomBounds);

	if (m_CropFlag) 
	{
		double Origin[3];
		m_ListSelected->Item(slice_num)->GetData()->GetOutput()->GetOrigin(Origin);
		m_ListSelected->Item(slice_num)->GetData()->GetOutput()->GetSpacing(spacing);

		m_CropPlane->Update();
		m_CropPlane->GetOutput()->GetBounds(crop_bounds);

		crop_bounds[0]+=Origin[0];
		crop_bounds[1]+=Origin[0];
		crop_bounds[2]+=Origin[1];
		crop_bounds[3]+=Origin[1];

		crop_bounds[4] = m_DicomBounds[4];
		crop_bounds[5] = m_DicomBounds[5];

		if(crop_bounds[1] > m_DicomBounds[1]) 
			crop_bounds[1] = m_DicomBounds[1];
		if(crop_bounds[3] > m_DicomBounds[3]) 
			crop_bounds[3] = m_DicomBounds[3];

		int k = 0;
		while(k * spacing[0] +Origin[0]<crop_bounds[0])
		{
			k++;
		}
		crop_bounds[0] = (k-1) * spacing[0] +Origin[0];

		k=0;
		while(k * spacing[1] +Origin[1]<crop_bounds[2])
		{
			k++;
		}
		crop_bounds[2] = (k-1) * spacing[1] +Origin[1];

		double dim_x_clip = ceil((double)(((crop_bounds[1] - crop_bounds[0]) / spacing[0]) + 1));
		double dim_y_clip = ceil((double)(((crop_bounds[3] - crop_bounds[2]) / spacing[1]) + 1));

		vtkMAFSmartPointer<vtkStructuredPoints> clip;
		clip->SetOrigin(crop_bounds[0], crop_bounds[2], loc[m_SortAxes]);	//modified by Paolo 12-11-2003
		clip->SetSpacing(spacing[0], spacing[1], 0);
		clip->SetDimensions(dim_x_clip, dim_y_clip, 1);
		clip->Update();

		vtkMAFSmartPointer<vtkProbeFilter> probe;
		probe->SetInput(clip);
		probe->SetSource(m_ListSelected->Item(slice_num)->GetData()->GetOutput());
    probe->Update();
    probe->GetOutput()->GetBounds(m_DicomBounds);
		probe->GetOutput()->GetScalarRange(range);
		m_SliceTexture->SetInput((vtkImageData *)probe->GetOutput());
	} 
	else 
	{
		m_ListSelected->Item(slice_num)->GetData()->GetOutput()->GetScalarRange(range);
		m_SliceTexture->SetInput(m_ListSelected->Item(slice_num)->GetData()->GetOutput());
	}

	m_SliceTexture->Modified();

	m_SliceLookupTable->SetTableRange(range);
	m_SliceLookupTable->SetWindow(range[1] - range[0]);
	m_SliceLookupTable->SetLevel((range[1] + range[0]) / 2.0);
	m_SliceLookupTable->Build();

	m_SliceTexture->MapColorScalarsThroughLookupTableOn();
	m_SliceTexture->SetLookupTable((vtkLookupTable *)m_SliceLookupTable);
}

//----------------------------------------------------------------------------
void medOpImporterDicomOffis::ShowSlice()
//----------------------------------------------------------------------------
{	
	double diffx,diffy;
	diffx=m_DicomBounds[1]-m_DicomBounds[0];
	diffy=m_DicomBounds[3]-m_DicomBounds[2];

	m_SlicePlane->SetOrigin(0,0,0);
	m_SlicePlane->SetPoint1(diffx,0,0);
	m_SlicePlane->SetPoint2(0,diffy,0);
	m_SliceActor->VisibilityOn();
  }

//----------------------------------------------------------------------------
vtkImageData* medOpImporterDicomOffis::GetSliceImageData(int slice_num)
//----------------------------------------------------------------------------
{
 	m_ListSelected->Item(slice_num)->GetData()->GetOutput()->Update();
  return m_ListSelected->Item(slice_num)->GetData()->GetOutput();
}

//----------------------------------------------------------------------------
void medOpImporterDicomOffis::ImportDicomTags()
//----------------------------------------------------------------------------
{
  if (m_TagArray == NULL) 
    mafNEW(m_TagArray);

  m_TagArray->SetName("TagArray");

  DcmFileFormat dicomImg;  
  DJDecoderRegistration::registerCodecs(); // register JPEG codecs
  OFCondition status = dicomImg.loadFile(m_FileName);//load data into offis structure

  if (!status.good()) 
  {
    if(!this->m_TestMode)
    {
      mafLogMessage(wxString::Format("File <%s> can not be opened",m_FileName),"Warning!!");
    }
    return;
  }
  DcmDataset *ds = dicomImg.getDataset();//obtain dataset information from dicom file (loaded into memory)

  // decompress data set if compressed
  ds->chooseRepresentation(EXS_LittleEndianExplicit, NULL);
  DJDecoderRegistration::cleanup(); // deregister JPEG codecs

  OFString string;
  DcmStack stack;
  DcmObject *dobject = NULL;
  DcmElement *delem = NULL;
  status = ds->nextObject(stack, OFTrue);
  while (status.good())
  {
    try
    {

      dobject = stack.top();
      if (dobject->verify().good())
      { 
        DcmTag tag = dobject->getTag();
        wxString tagName = tag.getTagName();

        delem = dynamic_cast<DcmElement*>(dobject);
        if (delem != NULL)
        {
          delem->getOFStringArray(string);

          if (tagName.compare("PixelData") == 0)
            m_TagArray->SetTag(mafTagItem(tagName.c_str(),""));
          else
          {
            m_TagArray->SetTag(mafTagItem(tagName.c_str(),string.c_str()));
            if (tagName.compare("PatientPosition") == 0)
            {
              m_PatientPosition = string.c_str();
            }
          }         
        }
      }
    }
    catch (...)
    {
      mafLogMessage("Can not read Dicom tag.");
    }

   
    status = ds->nextObject(stack, OFTrue);
  } 
  dicomImg.clear();
}
//----------------------------------------------------------------------------
void medOpImporterDicomOffis::ResampleVolume()
//----------------------------------------------------------------------------
{
  double m_VolumePosition[3],m_VolumeOrientation[3], m_VolumeSpacing[3];
  m_VolumeSpacing[0] = m_VolumeSpacing[1] = m_VolumeSpacing[2] = VTK_DOUBLE_MAX;

  m_VolumePosition[0]    = m_VolumePosition[1]    = m_VolumePosition[2]    = 0;
  m_VolumeOrientation[0] = m_VolumeOrientation[1] = m_VolumeOrientation[2] = 0;

  mafVMEVolumeGray *vrg;
  mafNEW(vrg);

  mafSmartPointer<mafTransform> box_pose;
  box_pose->SetOrientation(m_VolumeOrientation);
  box_pose->SetPosition(m_VolumePosition);
  //box_pose->SetPosition(m_VolumePosition);

  mafSmartPointer<mafTransformFrame> local_pose;
  local_pose->SetInput(box_pose);

  mafSmartPointer<mafTransformFrame> output_to_input;

  // In a future version if not a "Natural" data the filter should operate in place.
  mafString new_vme_name = "resampled_";
  new_vme_name += m_Input->GetName();

  vrg->SetMatrix(box_pose->GetMatrix());

  double volumeBounds[6];
  vtkRectilinearGrid *rgrid;
  rgrid = vtkRectilinearGrid::SafeDownCast(m_Volume->GetVolumeOutput()->GetVTKData());
  rgrid->Update();
  rgrid->GetBounds(volumeBounds);

  for (int xi = 1; xi < rgrid->GetXCoordinates()->GetNumberOfTuples (); xi++)
  {
    double spcx = rgrid->GetXCoordinates()->GetTuple1(xi)-rgrid->GetXCoordinates()->GetTuple1(xi-1);
    if (m_VolumeSpacing[0] > spcx)
      m_VolumeSpacing[0] = spcx;
  }

  for (int yi = 1; yi < rgrid->GetYCoordinates()->GetNumberOfTuples (); yi++)
  {
    double spcy = rgrid->GetYCoordinates()->GetTuple1(yi)-rgrid->GetYCoordinates()->GetTuple1(yi-1);
    if (m_VolumeSpacing[1] > spcy)
      m_VolumeSpacing[1] = spcy;
  }

  for (int zi = 1; zi < rgrid->GetZCoordinates()->GetNumberOfTuples (); zi++)
  {
    double spcz = rgrid->GetZCoordinates()->GetTuple1(zi)-rgrid->GetZCoordinates()->GetTuple1(zi-1);
    if (m_VolumeSpacing[2] > spcz)
      m_VolumeSpacing[2] = spcz;
  }

  int output_extent[6];
  output_extent[0] = 0;
  output_extent[1] = (volumeBounds[1] - volumeBounds[0]) / m_VolumeSpacing[0];
  output_extent[2] = 0;
  output_extent[3] = (volumeBounds[3] - volumeBounds[2]) / m_VolumeSpacing[1];
  output_extent[4] = 0;
  output_extent[5] = (volumeBounds[5] - volumeBounds[4]) / m_VolumeSpacing[2];

  double w,l,sr[2];
  for (int i = 0; i < m_Volume->GetDataVector()->GetNumberOfItems(); i++)
  {
    if (mafVMEItemVTK *input_item = mafVMEItemVTK::SafeDownCast(m_Volume->GetDataVector()->GetItemByIndex(i)))
    {
      if (vtkDataSet *input_data = input_item->GetData())
      {
        // the resample filter
        vtkMAFSmartPointer<vtkMAFVolumeResample> resampler;
        double m_ZeroPadValue = 0.;
        resampler->SetZeroValue(m_ZeroPadValue);

        // Set the target be vme's parent frame. And Input frame to the root. I've to 
        // set at each iteration since I'm using the SetMatrix, which doesn't support
        // transform pipelines.
        mafSmartPointer<mafMatrix> output_parent_abs_pose;
        mafVME::SafeDownCast(m_Input)->GetOutput()->GetAbsMatrix(*output_parent_abs_pose.GetPointer(),input_item->GetTimeStamp());
        local_pose->SetInputFrame(output_parent_abs_pose);

        mafSmartPointer<mafMatrix> input_parent_abs_pose;
        ((mafVME *)m_Input)->GetOutput()->GetAbsMatrix(*input_parent_abs_pose.GetPointer(),input_item->GetTimeStamp());
        local_pose->SetTargetFrame(input_parent_abs_pose);
        local_pose->Update();

        mafSmartPointer<mafMatrix> output_abs_pose;
        m_Volume->GetOutput()->GetAbsMatrix(*output_abs_pose.GetPointer(),input_item->GetTimeStamp());
        output_to_input->SetInputFrame(output_abs_pose);

        mafSmartPointer<mafMatrix> input_abs_pose;
        ((mafVME *)m_Input)->GetOutput()->GetAbsMatrix(*input_abs_pose.GetPointer(),input_item->GetTimeStamp());
        output_to_input->SetTargetFrame(input_abs_pose);
        output_to_input->Update();

        double orient_input[3],orient_target[3];
        mafTransform::GetOrientation(*output_abs_pose.GetPointer(),orient_target);
        mafTransform::GetOrientation(*input_abs_pose.GetPointer(),orient_input);

        double origin[3];
        origin[0] = volumeBounds[0];
        origin[1] = volumeBounds[2];
        origin[2] = volumeBounds[4];

        output_to_input->TransformPoint(origin,origin);

        resampler->SetVolumeOrigin(origin[0],origin[1],origin[2]);

        vtkMatrix4x4 *mat = output_to_input->GetMatrix().GetVTKMatrix();

        double local_orient[3],local_position[3];
        mafTransform::GetOrientation(output_to_input->GetMatrix(),local_orient);
        mafTransform::GetPosition(output_to_input->GetMatrix(),local_position);

        // extract versors
        double x_axis[3],y_axis[3];

        mafMatrix::GetVersor(0,mat,x_axis);
        mafMatrix::GetVersor(1,mat,y_axis);

        resampler->SetVolumeAxisX(x_axis);
        resampler->SetVolumeAxisY(y_axis);

        vtkMAFSmartPointer<vtkStructuredPoints> output_data;
        output_data->SetSpacing(m_VolumeSpacing);
        // TODO: here I probably should allow a data type casting... i.e. a GUI widget
        output_data->SetScalarType(input_data->GetPointData()->GetScalars()->GetDataType());
        output_data->SetExtent(output_extent);
        output_data->SetUpdateExtent(output_extent);

        input_data->GetScalarRange(sr);

        w = sr[1] - sr[0];
        l = (sr[1] + sr[0]) * 0.5;

        resampler->SetWindow(w);
        resampler->SetLevel(l);
        resampler->SetInput(input_data);
        resampler->SetOutput(output_data);
        resampler->AutoSpacingOff();
        resampler->Update();

        output_data->SetSource(NULL);
        output_data->SetOrigin(volumeBounds[0],volumeBounds[2],volumeBounds[4]);

        vrg->SetDataByDetaching(output_data, input_item->GetTimeStamp());
        vrg->Update();
      }
    }
  }
  m_Volume->DeepCopy(vrg);
  m_Volume->Update();

  mafDEL(vrg);
}
//----------------------------------------------------------------------------
int compareX(const medImporterDICOMListElement **arg1,const medImporterDICOMListElement **arg2)
//----------------------------------------------------------------------------
{
	// compare the x coordinate of both arguments
	// return:
	double x1 = (*(medImporterDICOMListElement **)arg1)->GetCoordinate(0);
	double x2 = (*(medImporterDICOMListElement **)arg2)->GetCoordinate(0);
	if (x1 > x2)
		return 1;
	if (x1 < x2)
		return -1;
	else
		return 0;
}
//----------------------------------------------------------------------------
int compareY(const medImporterDICOMListElement **arg1,const medImporterDICOMListElement **arg2)
//----------------------------------------------------------------------------
{
	// compare the y coordinate of both arguments
	// return:
	double y1 = (*(medImporterDICOMListElement **)arg1)->GetCoordinate(1);
	double y2 = (*(medImporterDICOMListElement **)arg2)->GetCoordinate(1);
	if (y1 > y2)
		return 1;
	if (y1 < y2)
		return -1;
	else
		return 0;
}
//----------------------------------------------------------------------------
int compareZ(const medImporterDICOMListElement **arg1,const medImporterDICOMListElement **arg2)
//----------------------------------------------------------------------------
{
	// compare the z coordinate of both arguments
	// return:
	double z1 = (*(medImporterDICOMListElement **)arg1)->GetCoordinate(2);
	double z2 = (*(medImporterDICOMListElement **)arg2)->GetCoordinate(2);
	if (z1 > z2)
		return 1;
	if (z1 < z2)
		return -1;
	else
		return 0;
}
//----------------------------------------------------------------------------
int compareTriggerTime(const medImporterDICOMListElement **arg1,const medImporterDICOMListElement **arg2)
//----------------------------------------------------------------------------
{
	// compare the trigger time of both arguments
	// return:
	float t1 = (*(medImporterDICOMListElement **)arg1)->GetTriggerTime();
	float t2 = (*(medImporterDICOMListElement **)arg2)->GetTriggerTime();;
	if (t1 > t2)
		return 1;
	if (t1 < t2)
		return -1;
	else
		return 0;
}

//----------------------------------------------------------------------------
int compareImageNumber(const medImporterDICOMListElement **arg1,const medImporterDICOMListElement **arg2)
//----------------------------------------------------------------------------
{
	// compare the trigger time of both arguments
	// return:
	float i1 = (*(medImporterDICOMListElement **)arg1)->GetImageNumber();
	float i2 = (*(medImporterDICOMListElement **)arg2)->GetImageNumber();;
	if (i1 > i2)
		return 1;
	if (i1 < i2)
		return -1;
	else
		return 0;
}
