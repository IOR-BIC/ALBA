/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpImporterDicom.cpp,v $
Language:  C++
Date:      $Date: 2008-02-06 11:22:39 $
Version:   $Revision: 1.18 $
Authors:   Matteo Giacomoni
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

#include "medOpImporterDicom.h"

#include "medGUIWizard.h"
#include "medGUIWizardPage.h"
#include "mmgValidator.h"
#include "mmdMouse.h"
#include "mmiDICOMImporterInteractor.h"
#include "mafTagArray.h"
#include "mafRWI.h"
#include "mafVMEImage.h"
#include "mafVMEVolumeGray.h"
#include "mmgCheckListBox.h"
#include "medGUIDicomSettings.h"
#include "mmgButton.h"

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
#include "vtkRGSliceAccumulate.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpImporterDicom);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// constants :
//----------------------------------------------------------------------------
#define FIRST_SELECTION 0
#define START_PROGRESS_BAR 0

enum DICOM_IMPORTER_GUI_ID
{
	ID_FIRST = medGUIWizard::ID_LAST,
	//ID_DICTIONARY,
	ID_OPEN_DIR,
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
	LEFT_SIDE = 0,
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
medOpImporterDicom::medOpImporterDicom(wxString label):
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType = OPTYPE_IMPORTER;
	m_Canundo = true;

	m_Wizard = NULL;
	m_LoadPage = NULL;
	m_CropPage = NULL;
	m_BuildPage = NULL;

	m_BuildGuiLeft = NULL;
	m_CropGuiLeft = NULL;
	m_LoadGuiLeft = NULL;

	m_DicomReader = NULL;
	m_DirectoryReader = NULL;
	m_SliceLookupTable = NULL;
	m_SlicePlane = NULL;
	m_SliceMapper = NULL;
	m_SliceTexture = NULL;
	m_SliceActor = NULL;
	m_CropPlane = NULL;
	m_CropActor = NULL;

	m_NumberOfStudy = 0;
	m_StudyListbox = NULL;

	m_FilesList = NULL;

	m_DicomTypeRead = -1;
	m_DicomModality = 0;
	m_BuildStepValue = 0;

	m_SliceScannerBuildPage = NULL;
	m_SliceScannerCropPage = NULL;
	m_SliceScannerLoadPage = NULL;

	m_TagArray = NULL;

	m_BoxCorrect = false;
	m_CropFlag = false;

	m_GizmoStatus = GIZMO_NOT_EXIST;

	m_Image = NULL;
	m_Volume = NULL;

	m_SortAxes = 2;

	m_VolumeName = "";

	m_VolumeSide=RIGHT_SIDE;

	m_DicomModalityListBox = NULL;

	m_CroppedExetuted=false;

  m_DicomInteractor = NULL;

  m_CurrentSlice = VTK_INT_MAX;

  //m_SettingPanel = new medGUIDicomSettings(this);
}
//----------------------------------------------------------------------------
medOpImporterDicom::~medOpImporterDicom()
//----------------------------------------------------------------------------
{
	vtkDEL(m_SliceActor);
	vtkDEL(m_SliceLookupTable);

	cppDEL(m_Wizard);
	mafDEL(m_TagArray);
	vtkDEL(m_DicomReader);

	mafDEL(m_Volume);
	mafDEL(m_Image);
}
//----------------------------------------------------------------------------
mafOp *medOpImporterDicom::Copy()
//----------------------------------------------------------------------------
{
	return new medOpImporterDicom(m_Label);
}
//----------------------------------------------------------------------------
void medOpImporterDicom::OpRun()
//----------------------------------------------------------------------------
{
	m_DictionaryFilename = ((medGUIDicomSettings*)GetSetting())->GetDictionary();
  if(!wxFileExists(m_DictionaryFilename.GetCStr()))
  {
    wxMessageBox(_("Missing Dictionary"));
    OpStop(OP_RUN_CANCEL);
    return;
  }
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

  //wxSplitPath(m_MafStringVar->GetCStr(), &path, &name, &ext);
  wxString path;
  wxDirDialog dialog(m_Wizard->GetParent(),"", path, 0, m_Wizard->GetPosition());
  dialog.SetReturnCode(wxID_OK);
  int ret_code = dialog.ShowModal();
  if (ret_code == wxID_OK)
  {
    path = dialog.GetPath();
    m_DicomDirectory = path.c_str();
    GuiUpdate();
    OpenDir();
  }

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
void medOpImporterDicom::OpDo()
//----------------------------------------------------------------------------
{
	if(m_Image != NULL)
		mafEventMacro(mafEvent(this,VME_ADD,m_Image));
	if(m_Volume != NULL)
		mafEventMacro(mafEvent(this,VME_ADD,m_Volume));
}
//----------------------------------------------------------------------------
void medOpImporterDicom::OpStop(int result)
//----------------------------------------------------------------------------
{
	if(m_DicomInteractor)
		m_Mouse->RemoveObserver(m_DicomInteractor);

	//close dialog
	for (int i=0; i < m_NumberOfStudy;i++)
	{
		((medListDicomFiles *)m_StudyListbox->GetClientData(i))->DeleteContents(TRUE);
		((medListDicomFiles *)m_StudyListbox->GetClientData(i))->Clear();
	}

  if(m_LoadPage)
	  m_LoadPage->GetRWI()->m_RenFront->RemoveActor(m_SliceActor);  

  if(m_CropPage)
	  //m_CropPage->GetRWI()->m_RenFront->RemoveActor(m_SliceActor);
	  m_CropPage->GetRWI()->m_RenFront->RemoveActor(m_CropActor);   

  if(m_BuildPage)
	  m_BuildPage->GetRWI()->m_RenFront->RemoveActor(m_SliceActor);

	vtkDEL(m_SliceTexture);
	vtkDEL(m_DirectoryReader);
	vtkDEL(m_DicomReader);
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

	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
int medOpImporterDicom::BuildVolume()
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

	vtkMAFSmartPointer<vtkRGSliceAccumulate> accumulate;
	accumulate->SetNumberOfSlices(n_slices);
	accumulate->BuildVolumeOnAxes(m_SortAxes);
	mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
	long progress = 0;
	int count,s_count;
	for (count = 0, s_count = 0; count < m_NumberOfSlices; count += step)
	{
		if (s_count == n_slices) {break;}
		ShowSlice(count);
		accumulate->SetSlice(s_count,m_SliceTexture->GetInput());
		s_count++;
		progress = count * 100 / m_DirectoryReader->GetNumberOfFiles();
		mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,progress));
	}
	mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));
	ImportDicomTags();
	if(m_NumberOfSlices == 1 || n_slices == 1)
	{
		//imageCase
		mafNEW(m_Image);
		m_Image->GetTagArray()->DeepCopy(m_TagArray);
		mafDEL(m_TagArray);

		accumulate->Update();
		m_Image->SetData(m_SliceTexture->GetInput(),0);

		mafTagItem tag_Nature;
		tag_Nature.SetName("VME_NATURE");
		tag_Nature.SetValue("NATURAL");
		m_Image->GetTagArray()->SetTag(tag_Nature);

		mafTagItem tag_Surgeon;
		tag_Surgeon.SetName("SURGEON_NAME");
		tag_Surgeon.SetValue(m_SurgeonName.GetCStr());
		m_Image->GetTagArray()->SetTag(tag_Surgeon);

    if(((medGUIDicomSettings*)GetSetting())->EnableChangeSide())
    {
		  mafTagItem tagSide;
		  tagSide.SetName("VOLUME_SIDE");
		  if(m_VolumeSide==LEFT_SIDE)
			  tagSide.SetValue("LEFT");
		  else if(m_VolumeSide==RIGHT_SIDE)
			  tagSide.SetValue("RIGHT");
		  m_Image->GetTagArray()->SetTag(tagSide);
    }

		//Nome VME = CTDir + IDStudio
		//wxString name = m_DicomDirectory + " - " + m_StudyListbox->GetString(m_StudyListbox->GetSelection());
    if(!m_PatientName.IsEmpty())
      m_VolumeName=m_PatientName;
    else
    {
      //Patient ID + Study ID
      m_VolumeName=m_Identifier+"_"+m_StudyListbox->GetString(m_StudyListbox->GetSelection());
    }
		m_Image->SetName(m_VolumeName);
	}
	else if(m_NumberOfSlices > 1)
	{
		//Copy inside the first VME item of m_Volume the CT volume and Dicom's tags
		mafNEW(m_Volume);
		m_Volume->GetTagArray()->DeepCopy(m_TagArray);
		mafDEL(m_TagArray);

		accumulate->Update();
		m_Volume->SetDataByDetaching(accumulate->GetOutput(),0);

		mafTagItem tag_Nature;
		tag_Nature.SetName("VME_NATURE");
		tag_Nature.SetValue("NATURAL");
		m_Volume->GetTagArray()->SetTag(tag_Nature);

		mafTagItem tag_Surgeon;
		tag_Surgeon.SetName("SURGEON_NAME");
		tag_Surgeon.SetValue(m_SurgeonName.GetCStr());
		m_Volume->GetTagArray()->SetTag(tag_Surgeon);

    if(((medGUIDicomSettings*)GetSetting())->EnableChangeSide())
    {
		  mafTagItem tagSide;
		  tagSide.SetName("VOLUME_SIDE");
		  if(m_VolumeSide==LEFT_SIDE)
			  tagSide.SetValue("LEFT");
		  else if(m_VolumeSide==RIGHT_SIDE)
			  tagSide.SetValue("RIGHT");
		  m_Volume->GetTagArray()->SetTag(tagSide);
    }

		//Nome VME = CTDir + IDStudio
		//wxString name = m_DicomDirectory + " - " + m_StudyListbox->GetString(m_StudyListbox->GetSelection());		
    if(!m_PatientName.IsEmpty())
      m_VolumeName=m_PatientName;
    else
    {
      //Patient ID + Study ID
      m_VolumeName=m_Identifier+"_"+m_StudyListbox->GetString(m_StudyListbox->GetSelection());
    }
		m_Volume->SetName(m_VolumeName);
	}

  return OP_RUN_OK;
}
//----------------------------------------------------------------------------
int medOpImporterDicom::BuildVolumeCineMRI()
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
		vtkMAFSmartPointer<vtkRGSliceAccumulate> accumulate;
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
				ShowSlice(currImageId);
			}

			accumulate->SetSlice(targetVolumeSliceId, m_SliceTexture->GetInput());
			targetVolumeSliceId++;
		}
		accumulate->Update();
		m_Volume->SetDataByDetaching(accumulate->GetOutput(),tsDouble);
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

  if(((medGUIDicomSettings*)GetSetting())->EnableChangeSide())
  {
	  mafTagItem tagSide;
	  tagSide.SetName("VOLUME_SIDE");
	  if(m_VolumeSide==LEFT_SIDE)
		  tagSide.SetValue("LEFT");
	  else if(m_VolumeSide==RIGHT_SIDE)
		  tagSide.SetValue("RIGHT");
	  m_Volume->GetTagArray()->SetTag(tagSide);
  }

	//Nome VME = CTDir + IDStudio
	wxString name = m_DicomDirectory + " - " + m_StudyListbox->GetString(m_StudyListbox->GetSelection());
	m_Volume->SetName(name.c_str());

  return OP_RUN_OK;

}
//----------------------------------------------------------------------------
void medOpImporterDicom::CreateLoadPage()
//----------------------------------------------------------------------------
{
	m_LoadPage = new medGUIWizardPage(m_Wizard,medUSEGUI|medUSERWI,_("First Step"));
	m_LoadGuiLeft = new mmgGui(this);
  m_LoadGuiRight = new mmgGui(this);
  m_LoadGuiCenter = new mmgGui(this);

	/*m_DicomModalityListBox=m_LoadGuiLeft->CheckList(ID_TYPE_DICOM,_("Modality"));
	m_DicomModalityListBox->AddItem(ID_CT_MODALITY,_("CT"),true);
	m_DicomModalityListBox->AddItem(ID_SC_MODALITY,_("SC"),true);
	m_DicomModalityListBox->AddItem(ID_MRI_MODALITY,_("MI"),true);
	m_DicomModalityListBox->AddItem(ID_XA_MODALITY,_("XA"),true);*/
	m_SliceScannerLoadPage=m_LoadGuiLeft->Slider(ID_SCAN_SLICE,_("slice #"),&m_CurrentSlice,0,m_CurrentSlice,"",((medGUIDicomSettings*)GetSetting())->EnableNumberOfSlice());
  if(((medGUIDicomSettings*)GetSetting())->EnableNumberOfTime())
  {
    m_TimeScannerLoadPage=m_LoadGuiLeft->Slider(ID_SCAN_TIME,_("time "),&m_CurrentTime,0,VTK_INT_MAX);
  }

  m_LoadGuiCenter->DirOpen(ID_OPEN_DIR, _("Folder"),	&m_DicomDirectory,_("Sel. DICOM Folder"));
  m_LoadGuiCenter->Enable(ID_OPEN_DIR,strcmp(m_DictionaryFilename.GetCStr(),""));//If there isn't a dictionary is impossible open DICOM Directory
  m_LoadGuiCenter->Divider();

  m_StudyListbox = m_LoadGuiRight->ListBox(ID_STUDY,_("study id"),70,"",wxLB_HSCROLL);
  
  m_LoadGuiRight->FitGui();
  m_LoadGuiLeft->FitGui();
  m_LoadGuiRight->FitGui();
  m_LoadPage->AddGuiLowerRight(m_LoadGuiRight);
  m_LoadPage->AddGuiLowerLeft(m_LoadGuiLeft);
  m_LoadPage->AddGuiLowerCenter(m_LoadGuiCenter);

	m_LoadPage->GetRWI()->CameraSet(CAMERA_CT);
	m_LoadPage->GetRWI()->m_RwiBase->SetMouse(m_Mouse);
	m_LoadPage->GetRWI()->m_RenFront->AddActor(m_SliceActor);
	//m_LoadPage->GetRWI()->m_RenFront->AddActor(m_CropActor);
}
//----------------------------------------------------------------------------
void medOpImporterDicom::CreateCropPage()
//----------------------------------------------------------------------------
{
	m_CropPage = new medGUIWizardPage(m_Wizard,medUSEGUI|medUSERWI,_("Second Step"));
	m_CropGuiLeft = new mmgGui(this);
  m_CropGuiCenter = new mmgGui(this);

	//m_CropGuiLeft->Label(_("crop"),true);
	//m_CropGuiLeft->Button(ID_CROP_BUTTON,_("crop"));
	//m_CropGuiLeft->Button(ID_UNDO_CROP_BUTTON,_("undo crop"));
	m_SliceScannerCropPage=m_CropGuiLeft->Slider(ID_SCAN_SLICE,_("slice #"),&m_CurrentSlice,0,VTK_INT_MAX,"",((medGUIDicomSettings*)GetSetting())->EnableNumberOfSlice());
  if(((medGUIDicomSettings*)GetSetting())->EnableNumberOfTime())
  {
    m_TimeScannerCropPage=m_CropGuiLeft->Slider(ID_SCAN_TIME,_("time "),&m_CurrentTime,0,VTK_INT_MAX);
  }

  if(((medGUIDicomSettings*)GetSetting())->EnableChangeSide())
  {
    wxString sideChoices[2] = {_("Left"),_("Right")};
    m_CropGuiCenter->Combo(ID_VOLUME_SIDE,_("volume side"),&m_VolumeSide,2,sideChoices);
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
void medOpImporterDicom::CreateBuildPage()
//----------------------------------------------------------------------------
{
	m_BuildPage = new medGUIWizardPage(m_Wizard,medUSEGUI|medUSERWI,_("Third Step"));
	m_BuildGuiLeft = new mmgGui(this);
  m_BuildGuiCenter = new mmgGui(this);

	//wxString buildStepChoices[4] = {_("1x"),_("2x"),_("3x"),_("4x")};
	//m_BuildGuiLeft->Label(_("build volume"),true);
	//m_BuildGuiLeft->Combo(ID_BUILD_STEP, _("step"), &m_BuildStepValue, 4, buildStepChoices);
	m_SliceScannerBuildPage=m_BuildGuiLeft->Slider(ID_SCAN_SLICE,_("slice #"),&m_CurrentSlice,0,VTK_INT_MAX,"",((medGUIDicomSettings*)GetSetting())->EnableNumberOfSlice());
  
  if(((medGUIDicomSettings*)GetSetting())->EnableNumberOfTime())
  {
	  m_TimeScannerBuildPage=m_BuildGuiLeft->Slider(ID_SCAN_TIME,_("time "),&m_CurrentTime,0,VTK_INT_MAX);
  }

  m_BuildGuiCenter->String(ID_VOLUME_NAME,_("volume name"),&m_VolumeName);

  m_BuildGuiLeft->FitGui();
  m_BuildGuiCenter->FitGui();
	m_BuildPage->AddGuiLowerLeft(m_BuildGuiLeft);
  m_BuildPage->AddGuiLowerCenter(m_BuildGuiCenter);

	m_BuildPage->GetRWI()->CameraSet(CAMERA_CT);
	m_BuildPage->GetRWI()->m_RwiBase->SetMouse(m_Mouse);
	m_BuildPage->GetRWI()->m_RenFront->AddActor(m_SliceActor);
	//m_BuildPage->GetRWI()->m_RenFront->AddActor(m_CropActor);
}
//----------------------------------------------------------------------------
void medOpImporterDicom::GuiUpdate()
//----------------------------------------------------------------------------
{
	m_LoadGuiLeft->Update();
  m_LoadGuiRight->Update();
  m_LoadGuiCenter->Update();

	m_CropGuiLeft->Update();
  m_CropGuiCenter->Update();

	m_BuildGuiLeft->Update();
  m_BuildGuiCenter->Update();
}
//----------------------------------------------------------------------------
void medOpImporterDicom::CreateGui()
//----------------------------------------------------------------------------
{
	m_Gui = new mmgGui(this);
}
//----------------------------------------------------------------------------
void medOpImporterDicom::OpenDir()
//----------------------------------------------------------------------------
{
  ResetStructure();
  // scan dicom directory
  BuildDicomFileList(m_DicomDirectory.GetCStr());
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
  }
}
//----------------------------------------------------------------------------
void medOpImporterDicom::OnEvent(mafEventBase *maf_event) 
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
				}

        if (m_Wizard->GetCurrentPage()==m_CropPage && e->GetBool())//From Crop page to build page
        {
          if(m_CropPage)
          Crop();
        }

        if (m_Wizard->GetCurrentPage()==m_BuildPage && (!e->GetBool()))//From buikd page to crop page
        {
          UndoCrop();
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
				CameraUpdate();
			}
			break;
		/*case ID_DICTIONARY:
			m_DicomReader->SetDictionaryFileName(m_DictionaryFilename.GetCStr());
			m_LoadGuiLeft->Enable(ID_OPEN_DIR,strcmp(m_DictionaryFilename.GetCStr(),""));
			break;*/
		case ID_OPEN_DIR:
			{
				OpenDir();
			}
			break;
		case ID_UNDO_CROP_BUTTON:
			{
				UndoCrop();
			}
			break;
		case ID_STUDY:
			{
				m_VolumeName = m_DicomDirectory + " - " + m_StudyListbox->GetString(m_StudyListbox->GetSelection());
				m_BuildGuiLeft->Update();
				EnableSliceSlider(true);
				if(m_DicomTypeRead == medGUIDicomSettings::ID_CMRI_MODALITY)//If cMRI
				{
					EnableTimeSlider(true);
				}

				//m_CropGuiLeft->Enable(ID_CROP_BUTTON,true);
				//m_CropGuiLeft->Enable(ID_UNDO_CROP_BUTTON,false);
				int sel = m_StudyListbox->GetSelection();
				m_ListSelected = (medListDicomFiles *)m_StudyListbox->GetClientData(sel);
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
				if (currImageId != -1) 
				{
					// show the selected slice
					ShowSlice(currImageId);
				}

				CameraReset();
				ResetSliders();
				CameraUpdate();

				//modified by STEFY 9-7-2003(begin)
				ImportDicomTags();
				mafTagItem *patient_name;
				mafTagItem *patient_id;

				const char* p_name;
				double p_id = 0;

				bool position = m_TagArray->IsTagPresent("PatientName");
				if (position)
				{
					patient_name = m_TagArray->GetTag("PatientName");
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

        wxString tmp_name;

        tmp_name.Append(m_PatientName.GetCStr());
        tmp_name.Append(m_Identifier.GetCStr());
        tmp_name=tmp_name+m_StudyListbox->GetString(m_StudyListbox->GetSelection());
        m_StudyListbox->SetString(m_StudyListbox->GetSelection(),tmp_name);
        m_StudyListbox->Update();
        GuiUpdate();
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
						CameraUpdate();
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
						CameraUpdate();
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
						ShowSlice(currImageId);
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
						ShowSlice(currImageId);
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
void medOpImporterDicom::UndoCrop()
//----------------------------------------------------------------------------
{
  m_CropFlag = false;
  ShowSlice(m_CurrentSlice);
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
void medOpImporterDicom::Crop()
//----------------------------------------------------------------------------
{
	if( !m_BoxCorrect )
	{
		wxMessageBox("Error on selecting the box");
		return;
	}

	m_CropFlag = true;
	ShowSlice(m_CurrentSlice);
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
void medOpImporterDicom::AutoPositionCropPlane()
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
	
	m_CropPage->GetRWI()->CameraReset();
}
//----------------------------------------------------------------------------
void medOpImporterDicom::CameraUpdate()
//----------------------------------------------------------------------------
{
	m_LoadPage->GetRWI()->CameraUpdate();
	m_CropPage->GetRWI()->CameraUpdate();
	m_BuildPage->GetRWI()->CameraUpdate();
}
//----------------------------------------------------------------------------
void medOpImporterDicom::CameraReset()
//----------------------------------------------------------------------------
{
	m_LoadPage->GetRWI()->CameraReset();
	m_CropPage->GetRWI()->CameraReset();
	m_BuildPage->GetRWI()->CameraReset();
}
//----------------------------------------------------------------------------
void medOpImporterDicom::EnableSliceSlider(bool enable)
//----------------------------------------------------------------------------
{
	m_LoadGuiLeft->Enable(ID_SCAN_SLICE,enable);
	m_BuildGuiLeft->Enable(ID_SCAN_SLICE,enable);
	m_CropGuiLeft->Enable(ID_SCAN_SLICE,enable);
}
//----------------------------------------------------------------------------
void medOpImporterDicom::EnableTimeSlider(bool enable)
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
void medOpImporterDicom::CreatePipeline()
//----------------------------------------------------------------------------
{
	vtkNEW(m_DirectoryReader);

	vtkNEW(m_DicomReader);
	m_DicomReader->SetDictionaryFileName(m_DictionaryFilename.GetCStr());
	m_DicomReader->UseDefaultDictionaryOff();

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

	mafNEW(m_DicomInteractor);
	m_DicomInteractor->SetListener(this);
	m_Mouse->AddObserver(m_DicomInteractor, MCH_INPUT);
}
//----------------------------------------------------------------------------
void medOpImporterDicom::BuildDicomFileList(const char *dir)
//----------------------------------------------------------------------------
{
	int row, i;
	double slice_pos[3];
	int imageNumber = -1;
	int numberOfImages = -1;
	double trigTime = -1.0;
	m_DicomTypeRead = -1;

	if (m_DirectoryReader->Open(dir) == 0)
	{
		wxMessageBox(wxString::Format("Directory <%s> can not be opened",dir),"Warning!!");
		return;
	}

	mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
	long progress = START_PROGRESS_BAR;

	wxBusyInfo wait_info("Reading DICOM directory: please wait");

	for (i=0; i < m_DirectoryReader->GetNumberOfFiles(); i++)
	{
		if ((strcmp(m_DirectoryReader->GetFile(i),".") == 0) || (strcmp(m_DirectoryReader->GetFile(i),"..") == 0)) 
		{
			continue;
		}
		else
		{
			m_CurrentSliceName = m_DirectoryReader->GetFile(i);
			// Append of the path at the dicom file
			wxString str_tmp, ct_mode;
			str_tmp.Append(dir);
			str_tmp.Append("\\");
			str_tmp.Append(m_CurrentSliceName);

			vtkDicomUnPacker *reader = vtkDicomUnPacker::New();
			reader->SetFileName((char *)str_tmp.c_str());
			reader->UseDefaultDictionaryOff();
			reader->SetDictionaryFileName(m_DictionaryFilename.GetCStr());
			reader->UpdateInformation();
			reader->Modified();
			reader->Update();
			
			((medGUIDicomSettings*)GetSetting())->EnableToRead(reader->GetModality());

			ct_mode = reader->GetCTMode();
			ct_mode.MakeUpper();
			ct_mode.Trim(FALSE);
			ct_mode.Trim();
			if (((medGUIDicomSettings*)GetSetting())->EnableToRead(reader->GetModality())&& strcmp( reader->GetModality(), "MR" ) != 0)
			{
				if(ct_mode.Find("SCOUT") != -1 || reader->GetStatus() == -1)
				{
					reader->Delete();
					continue;
				}
				row = m_StudyListbox->FindString(reader->GetStudyUID());
				if (row == -1)
				{
					// the study is not present into the listbox, so need to create new
					// list of files related to the new studyID
					m_FilesList = new medListDicomFiles;
					m_StudyListbox->Append(reader->GetStudyUID());
					m_StudyListbox->SetClientData(m_NumberOfStudy,(void *)m_FilesList);
					reader->GetSliceLocation(slice_pos);
					m_FilesList->Append(new medImporterDICOMListElement(str_tmp,slice_pos,reader->GetOutput()));
					m_NumberOfStudy++;
				}
				else 
				{
					reader->GetSliceLocation(slice_pos);
					((medListDicomFiles *)m_StudyListbox->GetClientData(row))->Append(new medImporterDICOMListElement(str_tmp,slice_pos,reader->GetOutput()));
				}
			}
			else if ( ((medGUIDicomSettings*)GetSetting())->EnableToRead(reader->GetModality())&& strcmp( reader->GetModality(), "MR" ) == 0)
			{
				if( reader->GetStatus() == -1)
				{
					reader->Delete();
					continue;
				}
				row = m_StudyListbox->FindString(reader->GetStudyUID());
				if (row == -1)
				{
					// the study is not present into the listbox, so need to create new
					// list of files related to the new studyID
					m_FilesList = new medListDicomFiles;
					m_StudyListbox->Append(reader->GetStudyUID());
					m_StudyListbox->SetClientData(m_NumberOfStudy,(void *)m_FilesList);
					reader->GetSliceLocation(slice_pos);
					imageNumber=reader->GetInstanceNumber();
					numberOfImages = reader->GetCardiacNumberOfImages();
					if(numberOfImages>1)
					{
						if (m_DicomTypeRead==-1)
							m_DicomTypeRead=medGUIDicomSettings::ID_CMRI_MODALITY;
						else if(m_DicomTypeRead!=medGUIDicomSettings::ID_CMRI_MODALITY)
						{
							wxString msg = _("cMRI damaged !");
							wxMessageBox(msg,"Confirm", wxOK , NULL);
							return;
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
							return;
						}
					}
					trigTime = reader->GetTriggerTime();
					m_FilesList->Append(new medImporterDICOMListElement(str_tmp,slice_pos,reader->GetOutput(),imageNumber, numberOfImages, trigTime));
					m_NumberOfStudy++;
				}
				else 
				{
					reader->GetSliceLocation(slice_pos);
					imageNumber=reader->GetInstanceNumber();
					numberOfImages = reader->GetCardiacNumberOfImages();
					trigTime = reader->GetTriggerTime();
					((medListDicomFiles *)m_StudyListbox->GetClientData(row))->Append(new medImporterDICOMListElement(str_tmp,slice_pos,reader->GetOutput(),imageNumber,numberOfImages,trigTime));
				}
			}
			progress = i * 100 / m_DirectoryReader->GetNumberOfFiles();
			mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,progress));
			reader->Delete();
		}
	}
	mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));
	if(m_NumberOfStudy == 0)
	{
		wxString msg = "No study found!";
		wxMessageBox(msg,"Confirm", wxOK , NULL);
	}
}
//----------------------------------------------------------------------------
void medOpImporterDicom::ResetStructure()
//----------------------------------------------------------------------------
{

	EnableSliceSlider(false);
	EnableTimeSlider(false);

	// delete the previous studies detected and reset the related variables
	for (int i=0; i < m_NumberOfStudy;i++)
	{
		((medListDicomFiles *)m_StudyListbox->GetClientData(i))->DeleteContents(true);
		((medListDicomFiles *)m_StudyListbox->GetClientData(i))->Clear();
	}

	m_StudyListbox->Clear();
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
void medOpImporterDicom::ResetSliders()
//----------------------------------------------------------------------------
{
  if(m_LoadGuiLeft)
  {
    m_LoadPage->RemoveGuiLowerLeft(m_LoadGuiLeft);
    delete m_LoadGuiLeft;
    m_LoadGuiLeft = new mmgGui(this);
    m_SliceScannerLoadPage=m_LoadGuiLeft->Slider(ID_SCAN_SLICE,_("slice #"),&m_CurrentSlice,0,m_NumberOfSlices-1,"",((medGUIDicomSettings*)GetSetting())->EnableNumberOfSlice());
    if(((medGUIDicomSettings*)GetSetting())->EnableNumberOfTime())
    {
      m_TimeScannerLoadPage=m_LoadGuiLeft->Slider(ID_SCAN_TIME,_("time "),&m_CurrentTime,0,m_NumberOfTimeFrames);
    }
    m_LoadPage->AddGuiLowerLeft(m_LoadGuiLeft);
  }

  if(m_CropGuiLeft)
  {
    m_CropPage->RemoveGuiLowerLeft(m_CropGuiLeft);
    delete m_CropGuiLeft;
    m_CropGuiLeft = new mmgGui(this);
    m_SliceScannerCropPage=m_CropGuiLeft->Slider(ID_SCAN_SLICE,_("slice #"),&m_CurrentSlice,0,m_NumberOfSlices-1,"",((medGUIDicomSettings*)GetSetting())->EnableNumberOfSlice());
    if(((medGUIDicomSettings*)GetSetting())->EnableNumberOfTime())
    {
      m_TimeScannerCropPage=m_CropGuiLeft->Slider(ID_SCAN_TIME,_("time "),&m_CurrentTime,0,m_NumberOfTimeFrames);
    }
    m_CropPage->AddGuiLowerLeft(m_CropGuiLeft);
  }


  if(m_BuildGuiLeft)
  {
    m_BuildPage->RemoveGuiLowerLeft(m_BuildGuiLeft);
    delete m_BuildGuiLeft;
    m_BuildGuiLeft = new mmgGui(this);
    m_SliceScannerBuildPage=m_BuildGuiLeft->Slider(ID_SCAN_SLICE,_("slice #"),&m_CurrentSlice,0,m_NumberOfSlices-1,"",((medGUIDicomSettings*)GetSetting())->EnableNumberOfSlice());
    if(((medGUIDicomSettings*)GetSetting())->EnableNumberOfTime())
    {
      m_TimeScannerBuildPage=m_BuildGuiLeft->Slider(ID_SCAN_TIME,_("time "),&m_CurrentTime,0,m_NumberOfTimeFrames);
    }
    m_BuildPage->AddGuiLowerLeft(m_BuildGuiLeft);
  }
}
//----------------------------------------------------------------------------
int medOpImporterDicom::GetImageId(int timeId, int heigthId)
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

	assert(m_StudyListbox);

	m_ListSelected = (medListDicomFiles *)m_StudyListbox->GetClientData(m_StudyListbox->GetSelection());

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
void medOpImporterDicom::ShowSlice(int slice_num)
//----------------------------------------------------------------------------
{
	// Description:
	// read the slice number 'slice_num' and generate the texture
	double spacing[3], crop_bounds[6], range[2], loc[3];

	m_ListSelected->Item(slice_num)->GetData()->GetSliceLocation(loc);

	m_ListSelected->Item(slice_num)->GetData()->GetOutput()->Update();
	m_ListSelected->Item(slice_num)->GetData()->GetOutput()->GetBounds(m_DicomBounds);


	// switch from m_DicomReader and v_dicom_probe on m_CropFlag
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

	double diffx,diffy;
	diffx=m_DicomBounds[1]-m_DicomBounds[0];
	diffy=m_DicomBounds[3]-m_DicomBounds[2];

	m_SlicePlane->SetOrigin(0,0,0);
	m_SlicePlane->SetPoint1(diffx,0,0);
	m_SlicePlane->SetPoint2(0,diffy,0);
	m_SliceActor->VisibilityOn();
}
//----------------------------------------------------------------------------
void medOpImporterDicom::ImportDicomTags()
//----------------------------------------------------------------------------
{
	if (m_TagArray == NULL) 
		mafNEW(m_TagArray);

	m_TagArray->SetName("TagArray");

	for (int i=0;i<m_DicomReader->GetNumberOfTags();i++)
	{
		char tmp[256];
		const char *keyword;

		const vtkDicomUnPacker::DICOM *tag = m_DicomReader->GetTag(i);

		if (!strcmp(tag->Keyword,""))
		{
			if (tag->intoDictionary == vtkDicomUnPacker::no)
			{
				int group=tag->Group;
				int element=tag->Element;
				sprintf(tmp,"G%d_E%d",group,element);
				keyword=tmp;
			}
		}
		else
		{
			keyword=(char *)tag->Keyword;
			if (tag->intoDictionary==vtkDicomUnPacker::no)
			{
				mafLogMessage("Hmmm Keyword not in dictionary???");
			}
		}

		int mult=m_DicomReader->GetTagElement(i)->mult;

		const vtkDicomUnPacker::VALUE *tagElement=m_DicomReader->GetTagElement(i);

		if (tagElement->type == vtkDicomUnPacker::string)
		{
			// String TAG
			m_TagArray->SetTag(mafTagItem(keyword,""));
			if (mult>=0)
			{
				// Fill the Element components
				m_TagArray->GetTag(keyword)->SetNumberOfComponents(mult+1);

				for (int j=0;j<=mult;j++)
				{
					const char *value = tagElement->stringa[j];
					m_TagArray->GetTag(keyword)->SetValue(value,j);
				}
			}
			else
			{
				m_TagArray->GetTag(keyword)->SetNumberOfComponents(0);
				cerr << "empty string...\n";
			}
		}
		else
		{
			// NUMERIC TAG
			double *value = (double *)tagElement->num;

			if (mult>0)
			{
				m_TagArray->SetTag(mafTagItem(keyword,value,mult+1));
			}
			else
			{
				// This is a atg with an emptt value...
				mafTagItem empty(keyword,"",MAF_MISSING_TAG);
				//empty.SetNumberOfComponents(0);
				m_TagArray->SetTag(empty);
			}
		}
	}
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