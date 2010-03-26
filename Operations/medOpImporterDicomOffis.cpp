/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpImporterDicomOffis.cpp,v $
Language:  C++
Date:      $Date: 2010-03-26 15:27:43 $
Version:   $Revision: 1.1.2.78 $
Authors:   Matteo Giacomoni, Roberto Mucci 
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
#include "medGUIWizardPageNew.h"
#include "mafGUIValidator.h"
#include "mafDeviceButtonsPadMouse.h"
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
#include "mafVMEMesh.h"
#include "mafVMEGroup.h"

#include "vtkMAFVolumeResample.h"
#include "vtkMAFSmartPointer.h"
#include "vtkDicomUnPacker.h"
#include "vtkDirectory.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkPlaneSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkTexture.h"
#include "vtkActor.h"
#include "vtkActor2D.h"
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
#include "vtkAppendFilter.h"
#include "vtkExtractVOI.h"
#include "vtkImageClip.h"
#include "vtkImageDataGeometryFilter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkUnstructuredGrid.h"
#include "vtkAppendPolyData.h" 
#include "vtkHexahedron.h"
#include "vtkFloatArray.h"
#include "vtkCellData.h"
#include "vtkIdTypeArray.h"
#include "vtkCellArray.h"
#include "vtkTextMapper.h"
#include "vtkTextProperty.h"
#include "vtkProperty2D.h"

#include "vtkDataSetWriter.h"
#include "vtkRectilinearGrid.h"
#include "vtkPointData.h"
#include "vtkShortArray.h"
#include "vtkUnsignedShortArray.h"
#include "vtkCharArray.h"

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
  ID_SERIES,
	ID_CROP_BUTTON,
	ID_UNDO_CROP_BUTTON,
	ID_BUILD_STEP,
	ID_BUILD_BUTTON,
	ID_CANCEL,
	ID_PATIENT_NAME,
	ID_PATIENT_ID,
	ID_SURGEON_NAME,
	ID_SCAN_TIME,
	ID_SCAN_SLICE,
	ID_VOLUME_NAME,
	ID_VOLUME_SIDE,
  ID_VME_TYPE
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

int CompareX(const medImporterDICOMListElements **arg1,const medImporterDICOMListElements **arg2);
int CompareY(const medImporterDICOMListElements **arg1,const medImporterDICOMListElements **arg2);
int CompareZ(const medImporterDICOMListElements **arg1,const medImporterDICOMListElements **arg2);
int CompareTriggerTime(const medImporterDICOMListElements **arg1,const medImporterDICOMListElements **arg2);
int CompareImageNumber(const medImporterDICOMListElements **arg1,const medImporterDICOMListElements **arg2);

WX_DEFINE_LIST(medListDICOMFiles);
//----------------------------------------------------------------------------
medOpImporterDicomOffis::medOpImporterDicomOffis(wxString label):
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType = OPTYPE_IMPORTER;
	m_Canundo = true;

  for (int i = 0; i < 6; i++) 
    m_DicomBounds[i] = 0;

  m_ZCropBounds[0] = 0;
  m_ZCropBounds[1] = 0;
    
  m_PatientPosition = "";

  m_VectorSelected.clear();
  m_VectorSelected.resize(3);

	m_Wizard = NULL;
	m_LoadPage = NULL;
	m_CropPage = NULL;
	m_BuildPage = NULL;
  m_Mesh = NULL;
  m_ImagesGroup = NULL;

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
  m_TextActor=NULL;
  m_TextMapper=NULL;

	m_NumberOfStudy = 0;
  m_NumberOfSlices = -1;
	m_StudyListbox = NULL;
  m_SeriesListbox = NULL;
  m_ListSelected = NULL;

  m_DicomDirectory = "";
	m_DicomTypeRead = -1;

  m_HighBit = 0;
  m_RescaleIntercept = 0;

	m_BuildStepValue = 0;
  m_OutputType = 0;

	m_SliceScannerBuildPage = NULL;
	m_SliceScannerCropPage = NULL;
	m_SliceScannerLoadPage = NULL;

	m_TagArray = NULL;

	m_BoxCorrect = false;
	m_CropFlag = false;
  m_CroppedExetuted = false;
  m_IsRotated = false;
  m_ConstantRotation = true;
  m_ZCrop = true;
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

  m_DiscardPosition = FALSE;

  m_RescaleTo16Bit = FALSE;
}
//----------------------------------------------------------------------------
medOpImporterDicomOffis::~medOpImporterDicomOffis()
//----------------------------------------------------------------------------
{
	vtkDEL(m_SliceActor);

	//cppDEL(m_Wizard);
	mafDEL(m_TagArray);
	mafDEL(m_Image);
  mafDEL(m_Mesh);
  mafDEL(m_Volume);

  m_ImagesGroup = NULL;

  //mafDEL(m_Output);  //achiarini(giaco): removed because useless and potentially dangerous

}
//----------------------------------------------------------------------------
mafOp *medOpImporterDicomOffis::Copy()
//----------------------------------------------------------------------------
{
  medOpImporterDicomOffis *importer = new medOpImporterDicomOffis(m_Label);
  importer->m_ResampleFlag = m_ResampleFlag;
  importer->m_DicomDirectory = m_DicomDirectory;
  importer->m_DiscardPosition = m_DiscardPosition;
  importer->m_RescaleTo16Bit = m_RescaleTo16Bit;
  
	return importer;
}
//----------------------------------------------------------------------------
void medOpImporterDicomOffis::OpRun()
//----------------------------------------------------------------------------
{
  m_BuildStepValue = ((medGUIDicomSettings*)GetSetting())->GetBuildStep();
  m_DiscardPosition = ((medGUIDicomSettings*)GetSetting())->EnableDiscardPosition();
  m_ResampleFlag = ((medGUIDicomSettings*)GetSetting())->EnableResampleVolume();
  m_RescaleTo16Bit = ((medGUIDicomSettings*)GetSetting())->EnableRescaleTo16Bit();

	CreateGui();
	CreatePipeline();

	m_Wizard = new medGUIWizard(_("DICOM IMPORTER"));
	m_Wizard->SetListener(this);

	CreateLoadPage();
	CreateCropPage();
	CreateBuildPage();
  m_Wizard->SetButtonString("Crop >");
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
		  wxDirDialog dialog(m_Wizard->GetParent(),"", path,wxRESIZE_BORDER, m_Wizard->GetPosition());
      
		  dialog.SetReturnCode(wxID_OK);
		  int ret_code = dialog.ShowModal();
		  if (ret_code == wxID_OK)
		  {
		    path = dialog.GetPath();
		    m_DicomDirectory = path.c_str();
		    GuiUpdate();
		    result = OpenDir();
        if (!result)
        {
          OpStop(OP_RUN_CANCEL);
          return;
        }
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


  int wizardResult = RunWizard();
  OpStop(wizardResult);
}
//----------------------------------------------------------------------------
int medOpImporterDicomOffis::RunWizard()
//----------------------------------------------------------------------------
{
  if(m_Wizard->Run())
  {
    int result;
    switch (m_OutputType)
    {
    case 0: 
      if (m_IsRotated)
      {
        if(!this->m_TestMode)
        {
          int answer = wxMessageBox( "Dicom dataset contains rotated images - Apply rotation?", "Warning", wxYES_NO, NULL);
          if (answer == wxNO)
          {
            m_IsRotated = false;
          }
        }
      }

      //rescale to 16 bit all the rest of the dataset
      if(m_RescaleTo16Bit == TRUE && m_HighBit == 11)
      {
        int i=0, size = m_ListSelected->size();

        wxBusyInfo *wait = NULL;
        if(!this->m_TestMode)
        {
          wait = new wxBusyInfo("Conversion to Unsigned Short: please wait...");
          mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
        }

        long progress = 0;
        for(int slice_num=0;slice_num<size;slice_num++)
        {
          RescaleTo16Bit(m_ListSelected->Item(slice_num)->GetData()->GetOutput());
          if(!this->m_TestMode)
          {
            progress = slice_num * 100 / (double)size;
            mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,progress));
          }
        }
        mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));
        if(wait) delete wait;
      }

      if(m_DicomTypeRead != medGUIDicomSettings::ID_CMRI_MODALITY)
        result = BuildVolume();
      else
        result = BuildVolumeCineMRI();
      break;
    case 1:
      if(m_DicomTypeRead != medGUIDicomSettings::ID_CMRI_MODALITY)
        result = BuildMesh();
      else
        result = BuildMeshCineMRI();
      break;
    case 2:
      if(m_DicomTypeRead != medGUIDicomSettings::ID_CMRI_MODALITY)
        result = BuildImages();
      else
        result = BuildImagesCineMRI();
      break;
    }
    return result;
  }
  else
    return OP_RUN_CANCEL;
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
    m_SeriesListbox->Clear();

  std::map<std::vector<mafString>,medListDICOMFiles*>::iterator it;
  for ( it=m_DicomMap.begin() ; it != m_DicomMap.end(); it++ )
  {
    m_DicomMap[(*it).first]->DeleteContents(TRUE);
    m_DicomMap[(*it).first]->Clear();
  }

  m_DicomMap.clear();

  if(m_LoadPage)
  {
    m_LoadPage->GetRWI()->m_RenFront->RemoveActor(m_SliceActor);
    if(((medGUIDicomSettings*)GetSetting())->EnableVisualizationPosition())
    {
      m_LoadPage->GetRWI()->m_RenFront->RemoveActor(m_TextActor);
    }
  }

  if(m_CropPage)
  {
    m_CropPage->GetRWI()->m_RenFront->RemoveActor(m_CropActor);  
    if(((medGUIDicomSettings*)GetSetting())->EnableVisualizationPosition())
    {
      m_LoadPage->GetRWI()->m_RenFront->RemoveActor(m_TextActor);
    }
  }

  if(m_BuildPage)
  {
    m_BuildPage->GetRWI()->m_RenFront->RemoveActor(m_SliceActor);
    if(((medGUIDicomSettings*)GetSetting())->EnableVisualizationPosition())
    {
      m_LoadPage->GetRWI()->m_RenFront->RemoveActor(m_TextActor);
    }
  }

  vtkDEL(m_SliceTexture);
  vtkDEL(m_DirectoryReader);
  vtkDEL(m_SliceLookupTable);
  vtkDEL(m_SlicePlane);
  vtkDEL(m_SliceMapper);
  vtkDEL(m_SliceActor);
  vtkDEL(m_CropPlane);
  vtkDEL(m_CropActor);

  vtkDEL(m_TextMapper);
  vtkDEL(m_TextActor);

  mafDEL(m_TagArray);
  mafDEL(m_DicomInteractor);

  if(!this->m_TestMode)
  {
    cppDEL(m_LoadGuiLeft);
    cppDEL(m_LoadGuiUnderLeft);
    cppDEL(m_LoadGuiCenter);
    cppDEL(m_CropGuiLeft);
    cppDEL(m_CropGuiCenter);
    cppDEL(m_BuildGuiLeft); 
    cppDEL(m_BuildGuiUnderLeft);
    cppDEL(m_BuildGuiCenter);
    cppDEL(m_LoadPage);
    cppDEL(m_CropPage);
    cppDEL(m_BuildPage);
    cppDEL(m_Wizard);
  }
}

//----------------------------------------------------------------------------
int medOpImporterDicomOffis::BuildImages()
//----------------------------------------------------------------------------
{
  int step;

  if(m_BuildStepValue == 0)
    step = 1;
  else if (m_BuildStepValue == 1)
    step = m_BuildStepValue << 1;
  else
    step = m_BuildStepValue + 1;

  int cropInterval = (m_ZCropBounds[1]+1 - m_ZCropBounds[0]);
  int n_slices = cropInterval / step;

  if(cropInterval % step != 0)
  {
    n_slices+=1;
  }

  if(!this->m_TestMode)
  {
    wxBusyInfo wait_info("Building images: please wait");
    mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
  }

  ImportDicomTags();
  long progress = 0;
  int count,s_count;
  mafNEW(m_ImagesGroup);

  m_ImagesGroup->SetName(wxString::Format("%s images",m_VolumeName));
  m_ImagesGroup->ReparentTo(m_Input);
  
  for (count = m_ZCropBounds[0], s_count = 0; count < m_ZCropBounds[1]+1; count += step)
  {
    if (s_count == n_slices) {break;}
    CreateSlice(count);
    
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

    mafSmartPointer<mafVMEImage> image;
    wxString name = m_VolumeName;
    name.Append(wxString::Format("_%d", count));
    image->SetName(name.c_str());
    image->SetData(im,0);
    image->GetTagArray()->DeepCopy(m_TagArray);

    mafTagItem tag_Nature;
    tag_Nature.SetName("VME_NATURE");
    tag_Nature.SetValue("NATURAL");
    image->GetTagArray()->SetTag(tag_Nature);

    mafTagItem tag_Surgeon;
    tag_Surgeon.SetName("SURGEON_NAME");
    tag_Surgeon.SetValue(m_SurgeonName.GetCStr());
    image->GetTagArray()->SetTag(tag_Surgeon);

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
        image->GetTagArray()->SetTag(tagSide);
      }
    }

    m_ImagesGroup->AddChild(image);
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
 
  if(m_ImagesGroup != NULL)
  {
    m_Output = m_ImagesGroup;
  }

  return OP_RUN_OK;
}

//----------------------------------------------------------------------------
int medOpImporterDicomOffis::BuildImagesCineMRI()
//----------------------------------------------------------------------------
{
  int step;

  if(m_BuildStepValue == 0)
    step = 1;
  else if (m_BuildStepValue == 1)
    step = m_BuildStepValue << 1;
  else
    step = m_BuildStepValue + 1;

  int cropInterval = (m_ZCropBounds[1]+1 - m_ZCropBounds[0]);
  int n_slices = cropInterval / step;

  if(cropInterval % step != 0)
  {
    n_slices+=1;
  }

  if(!this->m_TestMode)
  {
    wxBusyInfo wait_info("Building images: please wait");
    mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
  }
  ImportDicomTags();

  mafNEW(m_ImagesGroup);

  m_ImagesGroup->SetName(wxString::Format("%s images",m_VolumeName));
  m_ImagesGroup->ReparentTo(m_Input);

  //create all the animated images
  for (int i = m_ZCropBounds[0]; i < m_ZCropBounds[1]+1;i += step)
  {
    mafSmartPointer<mafVMEImage> image;
    wxString name = m_VolumeName;
    name.Append(wxString::Format("_%d", i));
    image->SetName(name.c_str());
    m_ImagesGroup->AddChild(image);
  }

  long progress = 0;
  int totalNumberOfImages = (m_ZCropBounds[1]+1)*m_NumberOfTimeFrames;
  int progressCounter = 0;

  // for every timestamp
  for (int ts = 0; ts < m_NumberOfTimeFrames; ts++)
  {
    if(!this->m_TestMode)
    {
      progress = ts * 100 / m_NumberOfTimeFrames;
      mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,progress));
    }

    // get the time stamp from the dicom tag;
    // timestamp is in ms
    int probeHeigthId = 0;    
    int tsImageId = GetImageId(ts, probeHeigthId);
    if (tsImageId == -1) 
    {
      assert(FALSE);
    }

    medImporterDICOMListElements *element0;
    element0 = (medImporterDICOMListElements *)m_ListSelected->Item(tsImageId)->GetData();
    mafTimeStamp tsDouble = (mafTimeStamp)(element0->GetTriggerTime());

    for (int sourceVolumeSliceId = m_ZCropBounds[0], targetVolumeSliceId = 0; sourceVolumeSliceId < m_ZCropBounds[1]+1; sourceVolumeSliceId += step)
    {
      if(!this->m_TestMode)
      {
        progress = progressCounter * 100 / totalNumberOfImages;
        mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,progress));
      }

      if (targetVolumeSliceId == n_slices) {break;}

      // show the current slice
      int currImageId = GetImageId(ts, sourceVolumeSliceId);
      if (currImageId != -1) 
      {
        // update v_texture ivar
        CreateSlice(currImageId);
      }

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

      ((mafVMEImage*)m_ImagesGroup->GetChild(targetVolumeSliceId))->SetData(im,ts);
      
      m_ImagesGroup->GetChild(targetVolumeSliceId)->GetTagArray()->DeepCopy(m_TagArray);

      mafTagItem tag_Nature;
      tag_Nature.SetName("VME_NATURE");
      tag_Nature.SetValue("NATURAL");
      m_ImagesGroup->GetChild(targetVolumeSliceId)->GetTagArray()->SetTag(tag_Nature);

      mafTagItem tag_Surgeon;
      tag_Surgeon.SetName("SURGEON_NAME");
      tag_Surgeon.SetValue(m_SurgeonName.GetCStr());
      m_ImagesGroup->GetChild(targetVolumeSliceId)->GetTagArray()->SetTag(tag_Surgeon);

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
          m_ImagesGroup->GetChild(targetVolumeSliceId)->GetTagArray()->SetTag(tagSide);
        }
      }
      targetVolumeSliceId++;
      progressCounter++;
    }
  }

  if(!this->m_TestMode)
  {
    mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));
  }

  if(m_ImagesGroup != NULL)
  {
    m_Output = m_ImagesGroup;
  }

  return OP_RUN_OK;
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

  int cropInterval = (m_ZCropBounds[1]+1 - m_ZCropBounds[0]);

  int n_slices = cropInterval/ step;
  if(cropInterval % step != 0)
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
  for (count = m_ZCropBounds[0], s_count = 0; count < m_ZCropBounds[1]+1; count += step)
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
  
  mafNEW(m_Volume);

//   ImportDicomTags(); 
//   //Copy inside the first VME item of m_Volume the CT volume and Dicom's tags
//   m_Volume->GetTagArray()->DeepCopy(m_TagArray);
//   mafDEL(m_TagArray);
  
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


  if (m_IsRotated)
  {
    double orientation[9] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
    m_ListSelected->Item(m_ZCropBounds[0])->GetData()->GetSliceOrientation(orientation);


    //transform direction cosines to be used to set vtkMatrix
    /* [ orientation[0]  orientation[1]  orientation[2]  -dst_pos_x ] 
    [ orientation[3]  orientation[4]  orientation[5]  -dst_pos_y ]
    [ dst_nrm_dircos_x  dst_nrm_dircos_y  dst_nrm_dircos_z  -dst_pos_z ]
    [ 0                 0                 0                 1          ]*/

    double dst_nrm_dircos_x = orientation[1] * orientation[5] - orientation[2] * orientation[4]; 
    double dst_nrm_dircos_y = orientation[2] * orientation[2] - orientation[0] * orientation[5]; 
    double dst_nrm_dircos_z = orientation[0] * orientation[4] - orientation[1] * orientation[3]; 

    vtkMatrix4x4 *mat = vtkMatrix4x4::New();
    mat->Identity();

    mat->SetElement(0,0,orientation[0]);
    mat->SetElement(1,0,orientation[3]);
    mat->SetElement(2,0,dst_nrm_dircos_x);
    mat->SetElement(3,0,0);
    mat->SetElement(0,1,orientation[1]);
    mat->SetElement(1,1,orientation[4]);
    mat->SetElement(2,1,dst_nrm_dircos_y);
    mat->SetElement(3,1,0);
    mat->SetElement(0,2,orientation[2]);
    mat->SetElement(1,2,orientation[5]);
    mat->SetElement(2,2,dst_nrm_dircos_z);
    mat->SetElement(3,2,0);
    mat->SetElement(3,3,1);

    mafSmartPointer<mafTransform> boxPose;
    boxPose->SetMatrix(mat);     
    boxPose->Update();
    m_Volume->SetAbsMatrix(boxPose->GetMatrix());
  }

  if(m_ResampleFlag == TRUE)
  {
    ResampleVolume();
  }

  ImportDicomTags(); 
  //Copy inside the first VME item of m_Volume the CT volume and Dicom's tags
  m_Volume->GetTagArray()->DeepCopy(m_TagArray);
  mafDEL(m_TagArray);
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

  int cropInterval = (m_ZCropBounds[1]+1 - m_ZCropBounds[0]);
  int n_slices = cropInterval / step;
  if(cropInterval % step != 0)
  {
    n_slices+=1;
  }

  if(!this->m_TestMode)
  {
    wxBusyInfo wait_info("Building volume: please wait");
    mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
  }

  // create the time varying vme
  mafNEW(m_Volume);
  int currImageId = 0;
  long progress = 0;
  int totalNumberOfImages = (m_ZCropBounds[1]+1)*m_NumberOfTimeFrames;
  int progressCounter = 0;
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
    medImporterDICOMListElements *element0;
    element0 = (medImporterDICOMListElements *)m_ListSelected->Item(tsImageId)->GetData();
    mafTimeStamp tsDouble = (mafTimeStamp)(element0->GetTriggerTime());

    for (int sourceVolumeSliceId = m_ZCropBounds[0], targetVolumeSliceId = 0; sourceVolumeSliceId < m_ZCropBounds[1]+1; sourceVolumeSliceId += step)
    {
      if(!this->m_TestMode)
      {
        progress = progressCounter * 100 / totalNumberOfImages;
        mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,progress));
      }
      if (targetVolumeSliceId == n_slices) {break;}

      // show the current slice
      currImageId = GetImageId(ts, sourceVolumeSliceId);
      if (currImageId != -1) 
      {
        // update v_texture ivar
        CreateSlice(currImageId);
      }
      accumulate->SetSlice(targetVolumeSliceId, m_SliceTexture->GetInput());
      targetVolumeSliceId++;
      progressCounter++;
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
    m_Volume->Update();

    if(m_ResampleFlag == TRUE)
    {
      ResampleVolume();
    }

    if (m_IsRotated)
    {
      double orientation[9] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
      m_ListSelected->Item(m_ZCropBounds[0])->GetData()->GetSliceOrientation(orientation);
  
      //transform direction cosines to be used to set vtkMatrix
   /* [ orientation[0]  orientation[1]  orientation[2]  -dst_pos_x ] 
      [ orientation[3]  orientation[4]  orientation[5]  -dst_pos_y ]
      [ dst_nrm_dircos_x  dst_nrm_dircos_y  dst_nrm_dircos_z  -dst_pos_z ]
      [ 0                 0                 0                 1          ]*/

      double dst_nrm_dircos_x = orientation[1] * orientation[5] - orientation[2] * orientation[4]; 
      double dst_nrm_dircos_y = orientation[2] * orientation[2] - orientation[0] * orientation[5]; 
      double dst_nrm_dircos_z = orientation[0] * orientation[4] - orientation[1] * orientation[3]; 

      vtkMatrix4x4 *mat = vtkMatrix4x4::New();
      mat->Identity();
      mat->SetElement(0,0,orientation[0]);
      mat->SetElement(1,0,orientation[3]);
      mat->SetElement(2,0,dst_nrm_dircos_x);
      mat->SetElement(3,0,0);
      mat->SetElement(0,1,orientation[1]);
      mat->SetElement(1,1,orientation[4]);
      mat->SetElement(2,1,dst_nrm_dircos_y);
      mat->SetElement(3,1,0);
      mat->SetElement(0,2,orientation[2]);
      mat->SetElement(1,2,orientation[5]);
      mat->SetElement(2,2,dst_nrm_dircos_z);
      mat->SetElement(3,2,0);
      mat->SetElement(3,3,1);

      mafSmartPointer<mafTransform> boxPose;
      boxPose->SetMatrix(mat);     
      boxPose->Update();

      m_Volume->SetAbsMatrix(boxPose->GetMatrix(),tsDouble);
    }
  }
  if(!this->m_TestMode)
  {
    mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));
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
int medOpImporterDicomOffis::BuildMesh()
//----------------------------------------------------------------------------
{
  long progress = 0;
  int step;

  if(m_BuildStepValue == 0)
    step = 1;
  else if (m_BuildStepValue == 1)
    step = m_BuildStepValue << 1;
  else
    step = m_BuildStepValue + 1;

  int dim[3];
  m_SliceTexture->GetInput()->GetDimensions(dim);
  int cropInterval = (m_ZCropBounds[1]+1 - m_ZCropBounds[0]);

  if(!this->m_TestMode)
  {
    wxBusyInfo wait_info("Building mesh: please wait");
    mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
  }

  mafNEW(m_Mesh);
  vtkCellArray *Cells = vtkCellArray::New();
  vtkUnstructuredGrid *grid = vtkUnstructuredGrid::New();
  vtkPoints *points = vtkPoints::New();
  vtkFloatArray *newScalars = vtkFloatArray::New();
  int pointsCounter = 0;
  int scalarCounter = 0;
  vtkPolyData *poly1;

  for (int sourceVolumeSliceId = m_ZCropBounds[0], targetVolumeSliceId = 0; sourceVolumeSliceId < m_ZCropBounds[1]+1; sourceVolumeSliceId += step)
  {
    poly1 = ExtractPolyData(0,sourceVolumeSliceId);
    poly1->Update();

    for(int n = 0; n < poly1->GetNumberOfPoints(); n++)
    {
      points->InsertPoint(pointsCounter, poly1->GetPoint(n));
      pointsCounter++;
    }
    for(int x=0;x<poly1->GetPointData()->GetNumberOfTuples();x++)
    {
      newScalars->InsertValue(scalarCounter, poly1->GetPointData()->GetScalars()->GetTuple1(x));
      scalarCounter++;
    }
  }

  grid->SetPoints(points);
  grid->GetPointData()->SetScalars(newScalars);
  grid->GetPointData()->GetScalars()->SetName(m_SliceTexture->GetInput()->GetPointData()->GetScalars()->GetName());
  grid->Update();

  int counter= 0;
  int total = dim[0]*dim[1];
  int sourceVolumeSliceId = m_ZCropBounds[0]; // ac fixed compilation error: vs2005
  for ( ; sourceVolumeSliceId <m_ZCropBounds[1]+1; sourceVolumeSliceId += step)
  { 
    if(!this->m_TestMode)
    {
      progress = sourceVolumeSliceId * 100 / m_NumberOfSlices;
      mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,progress));
    }
    
    if (sourceVolumeSliceId+1>=m_NumberOfSlices)// compilation error: vs2005: sourceVolumeSliceId defined in the for loop
      break;
    int lineCounter = 1;
    for(int n = 0; n < poly1->GetNumberOfPoints()-dim[0]-1; n++)
    {
      if (n == lineCounter*dim[0] )
      {
        lineCounter++;
      }
      vtkHexahedron *hexahedron = vtkHexahedron::New();

      if (n == (lineCounter-1)*dim[0]+(dim[0]-1) && n != 0) //on the edge
      {
        continue;
      }
      hexahedron->GetPointIds()->SetId(0,counter*(total)+n);
      hexahedron->GetPointIds()->SetId(1,counter*(total)+n+1);
      hexahedron->GetPointIds()->SetId(2,counter*(total)+n+dim[0]+1);
      hexahedron->GetPointIds()->SetId(3,counter*(total)+n+dim[0]);
      hexahedron->GetPointIds()->SetId(4,((counter+1)*(total)+n));
      hexahedron->GetPointIds()->SetId(5,((counter+1)*(total)+n+1));
      hexahedron->GetPointIds()->SetId(6,((counter+1)*(total)+n+dim[0]+1));
      hexahedron->GetPointIds()->SetId(7,((counter+1)*(total)+n+dim[0]));

      Cells->InsertNextCell(hexahedron->GetPointIds());
      grid->Update();
      hexahedron->Delete();
    }
    counter++;
  }

  if(!this->m_TestMode)
  {
    mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));
  }

  grid->SetCells(VTK_HEXAHEDRON,Cells);  


  int currImageId = GetImageId(0, sourceVolumeSliceId);
  medImporterDICOMListElements *element0;
  element0 = (medImporterDICOMListElements *)m_ListSelected->Item(currImageId)->GetData();
  mafTimeStamp tsDouble = (mafTimeStamp)(element0->GetTriggerTime());
  m_Mesh->SetData(grid, 0);
  points->Delete();
  grid->Delete();

  m_Mesh->SetName(m_VolumeName);

  m_Output = m_Mesh;
  return OP_RUN_OK;
}
//----------------------------------------------------------------------------
int medOpImporterDicomOffis::BuildMeshCineMRI()
//----------------------------------------------------------------------------
{
  int step;

  if(m_BuildStepValue == 0)
    step = 1;
  else if (m_BuildStepValue == 1)
    step = m_BuildStepValue << 1;
  else
    step = m_BuildStepValue + 1;

  int dim[3];
  m_SliceTexture->GetInput()->GetDimensions(dim);
  int cropInterval = (m_ZCropBounds[1]+1 - m_ZCropBounds[0]);



  if(!this->m_TestMode)
  {
    wxBusyInfo wait_info("Building mesh: please wait");
    mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
  }
  mafNEW(m_Mesh);

  long progress = 0;
  int totalNumberOfImages = (m_ZCropBounds[1]+1)*m_NumberOfTimeFrames;
  int progressCounter = 0;
  for (int ts = 0; ts < m_NumberOfTimeFrames; ts++)
  {
    vtkCellArray *Cells = vtkCellArray::New();
    vtkUnstructuredGrid *grid = vtkUnstructuredGrid::New();
    vtkPoints *points = vtkPoints::New();
    vtkFloatArray *newScalars = vtkFloatArray::New();
    int pointsCounter = 0;
    int scalarCounter = 0;
    vtkPolyData *poly1;

    for (int sourceVolumeSliceId = m_ZCropBounds[0], targetVolumeSliceId = 0; sourceVolumeSliceId < m_ZCropBounds[1]+1; sourceVolumeSliceId += step)
    {
       poly1 = ExtractPolyData(ts,sourceVolumeSliceId);
       poly1->Update();

       for(int n = 0; n < poly1->GetNumberOfPoints(); n++)
       {
         points->InsertPoint(pointsCounter, poly1->GetPoint(n));
         pointsCounter++;
       }
       for(int x=0;x<poly1->GetPointData()->GetNumberOfTuples();x++)
       {
         newScalars->InsertValue(scalarCounter, poly1->GetPointData()->GetScalars()->GetTuple1(x));
         scalarCounter++;
       }
    }
    
    grid->SetPoints(points);
    grid->GetPointData()->SetScalars(newScalars);
    grid->GetPointData()->GetScalars()->SetName(m_SliceTexture->GetInput()->GetPointData()->GetScalars()->GetName());
    grid->Update();

    int counter= 0;
    int total = dim[0]*dim[1];
    int sourceVolumeSliceId = m_ZCropBounds[0];// ac: compilation error (vs2005)
    for (; sourceVolumeSliceId <m_ZCropBounds[1]+1; sourceVolumeSliceId += step)
    { 
      if(!this->m_TestMode)
      {
        progress = progressCounter * 100 / totalNumberOfImages;
        mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,progress));
      }

       if (sourceVolumeSliceId+1>=m_NumberOfSlices)// ac: compilation error (vs2005): sourceVolumeSliceId defined in the for loop
         break;
      int lineCounter = 1;
      for(int n = 0; n < poly1->GetNumberOfPoints()-dim[0]-1; n++)
      {
        if (n == lineCounter*dim[0] )
        {
          lineCounter++;
        }
        vtkHexahedron *hexahedron = vtkHexahedron::New();

        if (n == (lineCounter-1)*dim[0]+(dim[0]-1) && n != 0) //on the edge
        {
          continue;
        }
        hexahedron->GetPointIds()->SetId(0,counter*(total)+n);
        hexahedron->GetPointIds()->SetId(1,counter*(total)+n+1);
        hexahedron->GetPointIds()->SetId(2,counter*(total)+n+dim[0]+1);
        hexahedron->GetPointIds()->SetId(3,counter*(total)+n+dim[0]);
        hexahedron->GetPointIds()->SetId(4,((counter+1)*(total)+n));
        hexahedron->GetPointIds()->SetId(5,((counter+1)*(total)+n+1));
        hexahedron->GetPointIds()->SetId(6,((counter+1)*(total)+n+dim[0]+1));
        hexahedron->GetPointIds()->SetId(7,((counter+1)*(total)+n+dim[0]));

        Cells->InsertNextCell(hexahedron->GetPointIds());
        grid->Update();
        hexahedron->Delete();
      }
      counter++;
      progressCounter++;
    }
    grid->SetCells(VTK_HEXAHEDRON,Cells);  


    int currImageId = GetImageId(ts, sourceVolumeSliceId);
    medImporterDICOMListElements *element0;
    element0 = (medImporterDICOMListElements *)m_ListSelected->Item(currImageId)->GetData();
    mafTimeStamp tsDouble = (mafTimeStamp)(element0->GetTriggerTime());
    m_Mesh->SetData(grid, tsDouble);
    points->Delete();
    grid->Delete();
  }
  if(!this->m_TestMode)
  {
    mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));
  }

  m_Mesh->SetName(m_VolumeName);

  m_Output = m_Mesh;
  return OP_RUN_OK;
}

//----------------------------------------------------------------------------
vtkPolyData* medOpImporterDicomOffis::ExtractPolyData(int ts, int silceId)
//----------------------------------------------------------------------------
{
  // show the current slice
  int currImageId = GetImageId(ts, silceId);
  if (currImageId != -1) 
  {
    // update v_texture ivar
    CreateSlice(currImageId);
  }

  vtkMAFSmartPointer<vtkImageData> imageData;
  imageData = m_SliceTexture->GetInput();

  for(int x=0;x<imageData->GetPointData()->GetNumberOfTuples();x++)
  {
    double i = imageData->GetPointData()->GetScalars()->GetTuple1(x);
  }

  vtkMatrix4x4 *mat = vtkMatrix4x4::New();
  mat->Identity();
  if (m_IsRotated)
  {
  double orientation[9] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  m_ListSelected->Item(currImageId)->GetData()->GetSliceOrientation(orientation);

    //transform direction cosines to be used to set vtkMatrix
 /* [ orientation[0]  orientation[1]  orientation[2]  -dst_pos_x ] 
    [ orientation[3]  orientation[4]  orientation[5]  -dst_pos_y ]
    [ dst_nrm_dircos_x  dst_nrm_dircos_y  dst_nrm_dircos_z  -dst_pos_z ]
    [ 0                 0                 0                 1          ]*/

    double dst_nrm_dircos_x = orientation[1] * orientation[5] - orientation[2] * orientation[4]; 
    double dst_nrm_dircos_y = orientation[2] * orientation[2] - orientation[0] * orientation[5]; 
    double dst_nrm_dircos_z = orientation[0] * orientation[4] - orientation[1] * orientation[3]; 

    mat->SetElement(0,0,orientation[0]);
    mat->SetElement(1,0,orientation[3]);
    mat->SetElement(2,0,dst_nrm_dircos_x);
    mat->SetElement(3,0,0);
    mat->SetElement(0,1,orientation[1]);
    mat->SetElement(1,1,orientation[4]);
    mat->SetElement(2,1,dst_nrm_dircos_y);
    mat->SetElement(3,1,0);
    mat->SetElement(0,2,orientation[2]);
    mat->SetElement(1,2,orientation[5]);
    mat->SetElement(2,2,dst_nrm_dircos_z);
    mat->SetElement(3,2,0);
    mat->SetElement(3,3,1);
  }

  vtkTransform *trans = vtkTransform::New();
  trans->SetMatrix(mat);

  vtkImageDataGeometryFilter *surface = vtkImageDataGeometryFilter::New();
  surface->SetInput(imageData);
  surface->Update();

  vtkTransformPolyDataFilter *TranslateFilter = vtkTransformPolyDataFilter::New();
  TranslateFilter->SetTransform(trans);
  TranslateFilter->SetInput(surface->GetOutput());
  
  TranslateFilter->Update();

  mat->Delete();
  trans->Delete();
  vtkDEL(surface);
  
  return TranslateFilter->GetOutput();
}

//----------------------------------------------------------------------------
void medOpImporterDicomOffis::CreateLoadPage()
//----------------------------------------------------------------------------
{
	m_LoadPage = new medGUIWizardPageNew(m_Wizard,medUSEGUI|medUSERWI);
	m_LoadGuiLeft = new mafGUI(this);
  m_LoadGuiUnderLeft = new mafGUI(this);
  m_LoadGuiCenter = new mafGUI(this);

	m_SliceScannerLoadPage=m_LoadGuiLeft->Slider(ID_SCAN_SLICE,_("slice #"),&m_CurrentSlice,0,m_CurrentSlice,"",((medGUIDicomSettings*)GetSetting())->EnableNumberOfSlice());
  m_SliceScannerLoadPage->SetPageSize(1);
  if(((medGUIDicomSettings*)GetSetting())->EnableNumberOfTime())
  {
    m_TimeScannerLoadPage=m_LoadGuiLeft->Slider(ID_SCAN_TIME,_("time "),&m_CurrentTime,0,VTK_INT_MAX);
    m_TimeScannerLoadPage->SetPageSize(1);
  }

  m_StudyListbox = m_LoadGuiUnderLeft->ListBox(ID_STUDY,_("study"),80,"",wxLB_HSCROLL,190);
  m_SeriesListbox = m_LoadGuiCenter->ListBox(ID_SERIES,_("series"),80,"",wxLB_HSCROLL|wxLB_SORT,190);
  
  m_LoadGuiLeft->FitGui();
  m_LoadGuiUnderLeft->FitGui();
  m_LoadGuiCenter->FitGui();
  m_LoadPage->AddGuiLowerLeft(m_LoadGuiLeft);
  m_LoadPage->AddGuiLowerUnderLeft(m_LoadGuiUnderLeft);
  m_LoadPage->AddGuiLowerUnderCenter(m_LoadGuiCenter);
  
	m_LoadPage->GetRWI()->m_RwiBase->SetMouse(m_Mouse);
  m_LoadPage->GetRWI()->m_RenFront->AddActor(m_SliceActor);
  if(((medGUIDicomSettings*)GetSetting())->EnableVisualizationPosition())
  {
    m_LoadPage->GetRWI()->m_RenFront->AddActor(m_TextActor);
  }
}
//----------------------------------------------------------------------------
void medOpImporterDicomOffis::CreateCropPage()
//----------------------------------------------------------------------------
{
  m_ZCrop = ((medGUIDicomSettings*)GetSetting())->EnableZCrop();
	m_CropPage = new medGUIWizardPageNew(m_Wizard,medUSEGUI|medUSERWI,m_ZCrop);
  m_CropPage->SetListener(this);
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

	m_CropPage->GetRWI()->m_RwiBase->SetMouse(m_Mouse);
  m_CropPage->GetRWI()->m_RenFront->AddActor(m_SliceActor);
  m_CropPage->GetRWI()->m_RenFront->AddActor(m_CropActor);
  if(((medGUIDicomSettings*)GetSetting())->EnableVisualizationPosition())
  {
    m_CropPage->GetRWI()->m_RenFront->AddActor(m_TextActor);
  }
}
//----------------------------------------------------------------------------
void medOpImporterDicomOffis::CreateBuildPage()
//----------------------------------------------------------------------------
{
	m_BuildPage = new medGUIWizardPageNew(m_Wizard,medUSEGUI|medUSERWI);
	m_BuildGuiLeft = new mafGUI(this);
  m_BuildGuiUnderLeft = new mafGUI(this);
  m_BuildGuiCenter = new mafGUI(this);

	m_SliceScannerBuildPage=m_BuildGuiLeft->Slider(ID_SCAN_SLICE,_("slice #"),&m_CurrentSlice,0,VTK_INT_MAX,"",((medGUIDicomSettings*)GetSetting())->EnableNumberOfSlice());
  m_SliceScannerBuildPage->SetPageSize(1);
  
  m_TimeScannerBuildPage=m_BuildGuiLeft->Slider(ID_SCAN_TIME,_("time "),&m_CurrentTime,0,VTK_INT_MAX);
  m_TimeScannerBuildPage->SetPageSize(1);

  m_BuildGuiCenter->Divider();
  if(((medGUIDicomSettings*)GetSetting())->AutoVMEType())
     m_OutputType = ((medGUIDicomSettings*)GetSetting())->GetVMEType(); 
  else
  {
    wxString typeArray[3] = {_("Volume"),_("Mesh"),_("Image")};
    m_BuildGuiCenter->Radio(ID_VME_TYPE, "VME output", &m_OutputType, 3, typeArray, 1, ""/*, wxRA_SPECIFY_ROWS*/);
  }
  
  m_BuildGuiUnderLeft->String(ID_VOLUME_NAME," VME name",&m_VolumeName);
  
  m_BuildGuiLeft->FitGui();
  m_BuildGuiUnderLeft->FitGui();
	m_BuildPage->AddGuiLowerLeft(m_BuildGuiLeft);
  m_BuildPage->AddGuiLowerCenter(m_BuildGuiUnderLeft);

	m_BuildPage->GetRWI()->m_RwiBase->SetMouse(m_Mouse);
	m_BuildPage->GetRWI()->m_RenFront->AddActor(m_SliceActor);
  if(((medGUIDicomSettings*)GetSetting())->EnableVisualizationPosition())
  {
    m_BuildPage->GetRWI()->m_RenFront->AddActor(m_TextActor);
  }
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
  m_BuildGuiUnderLeft->Update();
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
      m_StudyListbox->SetSelection(FIRST_SELECTION);
      UpdateStudyListBox();
      OnEvent(&mafEvent(this, ID_STUDY));
     /* if(m_NumberOfStudy == 1)
      {
        OnEvent(&mafEvent(this, ID_STUDY));
      }*/
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
      m_LoadPage->GetRWI()->CameraReset();

      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------
void medOpImporterDicomOffis::ReadDicom() 
//----------------------------------------------------------------------------
{
  if (this->m_TestMode)
  {
    m_VectorSelected = m_DicomMap.begin()->first;
  }

  m_ListSelected = m_DicomMap[m_VectorSelected];

  // sort dicom slices
  if(m_ListSelected->GetCount() > 1)
  {
    double item1_pos[3],item2_pos[3],d[3];
    medImporterDICOMListElements *element1;
    medImporterDICOMListElements *element2;
    element1 = (medImporterDICOMListElements *)m_ListSelected->Item(0)->GetData();
    element2 = (medImporterDICOMListElements *)m_ListSelected->Item(1)->GetData();
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
  //need this?
   switch (m_SortAxes)
   {
   case 0:
     m_ListSelected->Sort(CompareX);
     break;
   case 1:
     m_ListSelected->Sort(CompareY);
     break;
   case 2:
     m_ListSelected->Sort(CompareZ);
     break;
   }

   //Build a wrong volume with this line!!!
   //m_ListSelected->Sort(CompareImageNumber);

  m_NumberOfTimeFrames = ((medImporterDICOMListElements *)m_ListSelected->Item(0)->GetData())->GetNumberOfImages();
  if(m_DicomTypeRead == medGUIDicomSettings::ID_CMRI_MODALITY) //If cMRI
    m_NumberOfSlices = m_ListSelected->GetCount() / m_NumberOfTimeFrames;
  else
    m_NumberOfSlices = m_ListSelected->GetCount();

  //Set bounds of ZCrop slider widget
  m_ZCropBounds[1] = m_NumberOfSlices-1;
  if (!this->m_TestMode && m_ZCrop)
  {
    m_CropPage->SetZCropBounds(0, m_ZCropBounds[1]);
  }
  

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
    case ID_RANGE_MODIFIED:
      {
        //ZCrop slider
        double fractpart, intpart;
        double minMax[2];
        m_CropPage->GetZCropBounds(minMax);

        //approximate form int to double
        fractpart = modf (minMax[0] , &intpart);
        fractpart >= 0.5 ?  m_ZCropBounds[0] = ceil(minMax[0]) : m_ZCropBounds[0] = floor(minMax[0]);
        fractpart = modf (minMax[1] , &intpart);
        fractpart >= 0.5 ?  m_ZCropBounds[1] = ceil(minMax[1]) : m_ZCropBounds[1] = floor(minMax[1]);

        m_SliceScannerBuildPage->SetRange(m_ZCropBounds[0], m_ZCropBounds[1]);

        if(m_ZCropBounds[0] > m_CurrentSlice || m_CurrentSlice > m_ZCropBounds[1])
        {
          m_CurrentSlice = m_ZCropBounds[0];
          m_SliceScannerBuildPage->SetValue(m_CurrentSlice);
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
        }
        GuiUpdate();
      }
      break;
		case medGUIWizard::MED_WIZARD_CHANGE_PAGE:
			{
				if(m_Wizard->GetCurrentPage()==m_LoadPage)//From Load page to Crop Page
				{
          if(m_NumberOfStudy<1)
          {
					  m_Wizard->EnableChangePageOff();
					  wxMessageBox(_("No study found!"));
					  return;
          }
          else
          {
            m_Wizard->SetButtonString("Build >");
            m_Wizard->EnableChangePageOn();
            m_CropPage->UpdateActor();
          }
        }

        if (m_Wizard->GetCurrentPage()==m_CropPage)//From Crop page to build page
        {
          if (e->GetBool())
          {
            if(m_CropPage)
            Crop();
            m_Wizard->SetButtonString("Import >"); 
            m_BuildPage->UpdateActor();
            if(m_ZCropBounds[0] > m_CurrentSlice || m_CurrentSlice > m_ZCropBounds[1])
            {
              m_CurrentSlice = m_ZCropBounds[0];
            }
            m_SliceScannerBuildPage->SetValue(m_CurrentSlice);
            //if only 1 slice, disable radio widget and create a VMEImage
            if (m_ZCropBounds[1]+1 - m_ZCropBounds[0] >1 )
            {
              m_BuildPage->AddGuiLowerUnderLeft(m_BuildGuiCenter);
              m_BuildPage->Update();
              GuiUpdate();
            }
            else
              m_OutputType = 2;
          } 
          else
          {
            m_Wizard->SetButtonString("Crop >"); 
            m_LoadPage->UpdateActor();
          }
        }

        if (m_Wizard->GetCurrentPage()==m_BuildPage && (!e->GetBool()))//From build page to crop page
        {
          UndoCrop();
          m_Wizard->SetButtonString("Build >");
          m_BuildPage->RemoveGuiLowerUnderLeft(m_BuildGuiCenter);
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
        mafString *st = (mafString *)m_StudyListbox->GetClientData(m_StudyListbox->GetSelection());
        m_VectorSelected.at(0) = st->GetCStr();
        if (m_VectorSelected.at(0).Compare(m_StudyListbox->GetString(m_StudyListbox->GetSelection())) != 0)
        {
          FillSeriesListBox();
          m_SeriesListbox->SetSelection(FIRST_SELECTION);
          OnEvent(&mafEvent(this, ID_SERIES));
        }
 			}
			break;
    case ID_SERIES:
      {
        mafString *st = (mafString *)m_StudyListbox->GetClientData(m_StudyListbox->GetSelection());
        m_VectorSelected.at(0) = st->GetCStr();
        wxString  seriesName = m_SeriesListbox->GetString(m_SeriesListbox->GetSelection());
        m_VectorSelected.at(2) = seriesName.SubString(0, seriesName.find_last_of("x")-1);
        std::map<std::vector<mafString>,medListDICOMFiles*>::iterator it;
        for ( it=m_DicomMap.begin() ; it != m_DicomMap.end(); it++ )
        {
          if ((*it).first.at(0).Compare(m_VectorSelected.at(0)) == 0)
          { 
            if ((*it).first.at(2).Compare(m_VectorSelected.at(2)) == 0)
            {
              m_VectorSelected.at(1) = (*it).first.at(1);
              break;
            }
          }
        }
        if(!this->m_TestMode)
        {
          m_BuildGuiLeft->Update();
          EnableSliceSlider(true);

          m_ListSelected = m_DicomMap[m_VectorSelected];

          medImporterDICOMListElements *element0;
          element0 = (medImporterDICOMListElements *)m_ListSelected->Item(0)->GetData();

          int numberOfImages =  element0->GetNumberOfImages();
          m_DicomTypeRead=-1;
          if(numberOfImages>1)
          {
            m_DicomTypeRead=medGUIDicomSettings::ID_CMRI_MODALITY;
            EnableTimeSlider(true);
          }
        }
        ReadDicom();
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
			case MOUSE_MOVE:  //resize gizmo
				{
					if(m_CroppedExetuted==false)
					{
						if (m_Wizard->GetCurrentPage()==m_CropPage)
					{
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
			case MOUSE_UP:  //block gizmo
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
  m_CropActor->VisibilityOn();
  m_CroppedExetuted=false;
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

	m_CropPage->GetRWI()->CameraReset(boundsCamera);
	m_CropPage->GetRWI()->CameraUpdate();

	m_LoadPage->GetRWI()->CameraReset(boundsCamera);
	m_LoadPage->GetRWI()->CameraUpdate();
	m_BuildPage->GetRWI()->CameraReset(boundsCamera);
	m_BuildPage->GetRWI()->CameraUpdate();

  //Modify name
  double spacing[3];
  int cropInterval = (m_ZCropBounds[1]+1 - m_ZCropBounds[0]);
  m_ListSelected->Item(currImageId)->GetData()->GetOutput()->GetSpacing(spacing);
  double pixelDimX = diffx/spacing[0] + 1;
  double pixelDimY = diffy/spacing[0] + 1;


  wxString  seriesName = m_SeriesListbox->GetString(m_SeriesListbox->GetSelection());
  m_VolumeName = seriesName.Mid(0,seriesName.find_last_of('_'));
  m_VolumeName.Append(wxString::Format("_%ix%ix%i", (int)pixelDimX, (int)pixelDimY, cropInterval));
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
  m_CropPage->UpdateActor();
  m_BuildPage->UpdateActor();
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

  // text stuff
  m_Text = "Orientation: ";
  m_TextMapper = vtkTextMapper::New();
  m_TextMapper->SetInput(m_Text.c_str()); 
  m_TextMapper->GetTextProperty()->AntiAliasingOn();

  m_TextActor = vtkActor2D::New();
  m_TextActor->GetProperty()->SetColor(0.8,0,0);
  m_TextActor->SetMapper(m_TextMapper);
  m_TextActor->SetPosition(3,3);  
  m_TextMapper->Modified();

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
void medOpImporterDicomOffis::FillStudyListBox(std::vector<mafString> studyAndSeriesVec)
//----------------------------------------------------------------------------
{
  bool newStudy = true;
  int studyConuter = m_StudyListbox->GetCount();
  mafString studyName = "study_";
  studyName.Append(wxString::Format("%i", studyConuter));
  for (int n = 0; n < m_StudyListbox->GetCount(); n++)
  {
    mafString *st = (mafString *)m_StudyListbox->GetClientData(n);
    m_VectorSelected.at(0) = st->GetCStr();
    if (m_VectorSelected.at(0).Compare(studyAndSeriesVec.at(0)) == 0)
    {
      newStudy = false;
      break;
    }
  }
  if (newStudy)
  { 
    m_StudyListbox->Append(studyName.GetCStr());
    mafString *ms = new mafString((studyAndSeriesVec.at(0)).GetCStr());
    m_StudyListbox->SetClientData(studyConuter, (void *) ms);
  }
}

//----------------------------------------------------------------------------
void medOpImporterDicomOffis::UpdateStudyListBox()
//----------------------------------------------------------------------------
{
  for (int n = 0; n < m_StudyListbox->GetCount(); n++)
  {
    int counter = 0;
    mafString study = m_StudyListbox->GetString(n);
    mafString *st = (mafString *)m_StudyListbox->GetClientData(n);
    m_VectorSelected.at(0) = st->GetCStr();
    //wxString  studyName = st->GetCStr();
    //m_VectorSelected.at(0) = studyName.SubString(0, studyName.find_last_of("_")-1);

    std::map<std::vector<mafString>,medListDICOMFiles*>::iterator it;
    for ( it=m_DicomMap.begin() ; it != m_DicomMap.end(); it++ )
    {
      if ((*it).first.at(0).Compare(m_VectorSelected.at(0)) == 0)
      { 
        m_VectorSelected.at(1) = (*it).first.at(1);
        m_VectorSelected.at(2) = (*it).first.at(2);
        if (m_DicomMap.find(m_VectorSelected) != m_DicomMap.end())
        {
          counter++;
        }
      }
    }
    m_StudyListbox->SetString(n, study.Append(wxString::Format("_%i", counter)).GetCStr());
  }
}

//----------------------------------------------------------------------------
void medOpImporterDicomOffis::FillSeriesListBox()
//----------------------------------------------------------------------------
{
  int counter = 0;
  m_SeriesListbox->Clear();
  std::map<std::vector<mafString>,medListDICOMFiles*>::iterator it;
  for ( it=m_DicomMap.begin() ; it != m_DicomMap.end(); it++ )
  {
    if ((*it).first.at(0).Compare(m_VectorSelected.at(0)) == 0)
    { 
      m_VectorSelected.at(1) = (*it).first.at(1);
      m_VectorSelected.at(2) = (*it).first.at(2);
      if (m_DicomMap.find(m_VectorSelected) != m_DicomMap.end())
      {
        m_ListSelected = m_DicomMap[m_VectorSelected];
        int numberOfImages = 0;

        int numberOfTimeFrames = ((medImporterDICOMListElements *)m_ListSelected->Item(0)->GetData())->GetNumberOfImages();
        if(numberOfTimeFrames > 1) //If cMRI
          numberOfImages = m_ListSelected->GetCount() / numberOfTimeFrames;
        else
          numberOfImages = m_ListSelected->GetCount();
        
        mafString seriesName = m_VectorSelected.at(2);
        seriesName.Append(wxString::Format("x%i", numberOfImages));
        m_SeriesListbox->Append(seriesName.GetCStr());
        m_SeriesListbox->SetClientData(counter,(void *)m_DicomMap[m_VectorSelected]/*filesList*/);
        counter++;
      }
    }
  }
}

//----------------------------------------------------------------------------
bool medOpImporterDicomOffis::BuildDicomFileList(const char *dir)
//----------------------------------------------------------------------------
{
  long progress;
  int sliceNum = -1;
  int i;
	double slice_pos[3];
  double lastZPos = 0;
	long int imageNumber = -1;
	long int numberOfImages = -1;
	double trigTime = -1.0;
  double imageOrientationPatient[9] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  double imagePositionPatient[3] = {0.0,0.0,0.0};
  bool enableToRead = true; //true for test mode
  bool errorOccurred = false;
  double lastDistance = 0;
  mafString lastFileName = "";

  double percentageTolerance = 0;
  double scalarTolerance = 0;
  bool enableScalarTolerance = false;
  bool enablePercentageTolerance = false;

  if(!this->m_TestMode)
  {
    enableScalarTolerance = ((medGUIDicomSettings*)GetSetting())->EnableScalarTolerance();
    enablePercentageTolerance = ((medGUIDicomSettings*)GetSetting())->EnablePercentageTolerance();

    if (enableScalarTolerance)
      scalarTolerance = ((medGUIDicomSettings*)GetSetting())->GetScalarTolerance();

    if (enablePercentageTolerance)
      percentageTolerance = ((medGUIDicomSettings*)GetSetting())->GetPercetnageTolerance();
  }

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
			file.Append(dir);
			file.Append("\\");
			file.Append(m_CurrentSliceName);

      DJDecoderRegistration::registerCodecs(); // register JPEG codecs
      DcmRLEDecoderRegistration ::registerCodecs(OFFalse, OFFalse,OFFalse); // register RLE codecs
      OFCondition status = dicomImg.loadFile(file);//load data into offis structure

      if (!status.good())
      {
        if(!this->m_TestMode)
        {
          wxLogMessage(wxString::Format("File <%s> can not be opened",file));
          errorOccurred = true;
          sliceNum--;
        }
        continue;
      }
      m_FileName = file; 
      
      DcmDataset *ds = dicomImg.getDataset();//obtain dataset information from dicom file (loaded into memory)
 
      // decompress data set if compressed
      //ds->chooseRepresentation(EXS_LittleEndianExplicit, NULL);
      OFCondition error = ds->chooseRepresentation(EXS_LittleEndianExplicit, NULL);


      DJDecoderRegistration::cleanup(); // deregister JPEG codecs
      DcmRLEDecoderRegistration::cleanup();

      if (!error.good())
      {
          wxLogMessage(wxString::Format("Error decoding the image <%s>",file));
          errorOccurred = true;
          //return false;
          continue;
      }

      const char *option = "?";
      ds->findAndGetString(DCM_ScanOptions,option);
      wxString scanOption = option;

      if (scanOption.Find("SCOUT") != -1)//check if it is a scout image
      {
        continue;
      }

      //now are used findAndGet* method to get dicom information
      long int val_long;
      ds->findAndGetLongInt(DCM_Columns, val_long);
      int width = val_long;
      ds->findAndGetLongInt(DCM_Rows, val_long);
      int height = val_long;
      
      ds->findAndGetFloat64(DCM_ImagePositionPatient,imagePositionPatient[0],0);
      ds->findAndGetFloat64(DCM_ImagePositionPatient,imagePositionPatient[1],1);
      ds->findAndGetFloat64(DCM_ImagePositionPatient,imagePositionPatient[2],2);

      ds->findAndGetFloat64(DCM_ImageOrientationPatient,imageOrientationPatient[0],0);
      ds->findAndGetFloat64(DCM_ImageOrientationPatient,imageOrientationPatient[1],1);
      ds->findAndGetFloat64(DCM_ImageOrientationPatient,imageOrientationPatient[2],2);
      ds->findAndGetFloat64(DCM_ImageOrientationPatient,imageOrientationPatient[3],3);
      ds->findAndGetFloat64(DCM_ImageOrientationPatient,imageOrientationPatient[4],4);
      ds->findAndGetFloat64(DCM_ImageOrientationPatient,imageOrientationPatient[5],5);

     /* if (sliceNum == 1)
      {
        for (int i = 0; i < 9; i++)
          m_ImageOrientationPatient[i] = imageOrientationPatient[i];
      }*/

      // check if the dataset is rotated: => different from 1. 0. 0. 0. 1. 0.
      /* FROM David Clunie's note
      2.2.2 Orientation of DICOM images
      Another question that is frequently asked in comp.protocols.dicom is how to determine which side of an image is which 
      (e.g. left, right) and so on. The short answer is that for projection radiographs this is specified explicitly using 
      the Patient Orientation attribute, and for cross-sectional images it needs to be derived from the Image Orientation (Patient)
      direction cosines. In the standard these are explained as follows:

      "C.7.6.1.1.1 Patient Orientation. The Patient Orientation (0020,0020) relative to the image plane shall be specified by two values 
      that designate the anatomical direction of the positive row axis (left to right) and the positive column axis (top to bottom).
      The first entry is the direction of the rows, given by the direction of the last pixel in the first row from the first pixel
      in that row. The second entry is the direction of the columns, given by the direction of the last pixel in the first column from 
      the first pixel in that column. Anatomical direction shall be designated by the capital letters: A (anterior), P (posterior),
      R (right), L (left), H (head), F (foot). Each value of the orientation attribute shall contain at least one of these characters.
      If refinements in the orientation descriptions are to be specified, then they shall be designated by one or two additional letters
      in each value. Within each value, the letters shall be ordered with the principal orientation designated in the first character." 
      "C.7.6.2.1.1 Image Position And Image Orientation. The Image Position (0020,0032) specifies the x, y, and z coordinates of the upper 
      left hand corner of the image; it is the center of the first voxel transmitted. Image Orientation (0020,0037) specifies the direction 
      cosines of the first row and the first column with respect to the patient. These Attributes shall be provide as a pair. Row value for 
      the x, y, and z axes respectively followed by the Column value for the x, y, and z axes respectively. The direction of the axes is 
      defined fully by the patient's orientation. The x-axis is increasing to the left hand side of the patient. The y-axis is increasing 
      to the posterior side of the patient. The z-axis is increasing toward the head of the patient. The patient based coordinate system 
      is a right handed system, i.e. the vector cross product of a unit vector along the positive x-axis and a unit vector along the 
      positive y-axis is equal to a unit vector along the positive z-axis." */

      m_IsRotated  =  m_IsRotated || !( \
      fabs(imageOrientationPatient[0] - 1.0) < 0.0001 && \
      fabs(imageOrientationPatient[4] - imageOrientationPatient[0]) < 0.0001 &&\
      fabs(imageOrientationPatient[1] - 0.0) < 0.0001 &&\
      fabs(imageOrientationPatient[1] - imageOrientationPatient[2]) < 0.0001 &&\
      fabs(imageOrientationPatient[1] - imageOrientationPatient[3]) < 0.0001 &&\
      fabs(imageOrientationPatient[1] - imageOrientationPatient[5]) < 0.0001 \
      );

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
      
      double slope;
      if(ds->findAndGetFloat64(DCM_RescaleSlope,slope).bad())
      {
        //Unable to get element: DCM_RescaleSlope[0];
        slope = 1;
      } 
      
      long highBit; 
      double intercept;
      if(ds->findAndGetLongInt(DCM_HighBit,highBit).bad())
      {
        //Unable to get element: DCM_RescaleIntercept[0];
        highBit = 0;
      } 
      m_HighBit = highBit;
            
      if(ds->findAndGetFloat64(DCM_RescaleIntercept,intercept).bad())
      {
        //Unable to get element: DCM_RescaleIntercept[0];
        intercept = 0;
      }
      m_RescaleIntercept = intercept;

///////////////////CREATE VTKIMAGEDATA////////////////////////////
      //initialize vtkImageData
      vtkMAFSmartPointer<vtkImageData> imageData;
      imageData->SetDimensions(height, width,1);
      imageData->SetWholeExtent(0,width-1,0,height-1,0,0);
      imageData->SetUpdateExtent(0,width-1,0,height-1,0,0);
      imageData->SetExtent(imageData->GetUpdateExtent());
      imageData->SetNumberOfScalarComponents(1);
      imageData->SetOrigin(imagePositionPatient);
      imageData->SetSpacing(spacing);

      long pixel_rep;
      ds->findAndGetLongInt(DCM_PixelRepresentation,pixel_rep);
      ds->findAndGetLongInt(DCM_BitsAllocated,val_long);

      long pixel_max;
      long pixel_min;
      ds->findAndGetLongInt(DCM_SmallestImagePixelValue, pixel_min);
      ds->findAndGetLongInt(DCM_LargestImagePixelValue, pixel_max);

      if (pixel_min == pixel_max)
        intercept = 0;

      if(val_long==16 && pixel_rep == 0 )
      {
        if(pixel_min*slope+intercept >= VTK_UNSIGNED_SHORT_MIN && pixel_max*slope+intercept <= VTK_UNSIGNED_SHORT_MAX)
        {
          imageData->SetScalarType(VTK_UNSIGNED_SHORT);
        }
        else if (pixel_min*slope+intercept >= VTK_SHORT_MIN && pixel_max*slope+intercept <= VTK_SHORT_MAX)
        {
          imageData->SetScalarType(VTK_SHORT);
        }
        else
        {
          if(!this->m_TestMode)
          {
            wxLogMessage(wxString::Format("Inconsistent scalar values. Can not import file <%s>",file));
            errorOccurred = true;
            continue;
          }
        }
      }
      else if(val_long==16 && pixel_rep == 1)
      {
        if (pixel_min*slope+intercept >= VTK_SHORT_MIN && pixel_max*slope+intercept <= VTK_SHORT_MAX)
        {
          imageData->SetScalarType(VTK_SHORT);
        }
        else
        {
          if(!this->m_TestMode)
          {
            wxLogMessage(wxString::Format("Inconsistent scalar values. Can not import file <%s>",file));
            errorOccurred = true;
            continue;
          }
        }
      }
      else if(val_long==16 && pixel_rep == 0)
      {
        imageData->SetScalarType(VTK_UNSIGNED_SHORT);
      }
      else if(val_long==8)
      {
        imageData->SetScalarType(VTK_CHAR);
      }

      imageData->AllocateScalars();
      imageData->GetPointData()->GetScalars()->SetName("Scalars");
      imageData->Update();

      const Uint16 *dicom_buf_short = NULL;
      const Uint8* dicom_buf_char = NULL;
      if (val_long==16)
      {
        ds->findAndGetUint16Array(DCM_PixelData, dicom_buf_short);
      }
      else
      {
        ds->findAndGetUint8Array(DCM_PixelData, dicom_buf_char);
      }

      int counter=0;
      for(int y=0;y<height;y++)
      {
        for(int x=0;x<width;x++)
        {
          imageData->GetPointData()->GetScalars()->SetTuple1(counter, dicom_buf_short[width*y+x]);
          counter++;
        }
      }
      imageData->Update();

      if (slope != 1 || intercept != 0)
      {
        if (imageData->GetScalarType() == VTK_UNSIGNED_SHORT)
        {
          vtkUnsignedShortArray *scalars=vtkUnsignedShortArray::SafeDownCast(imageData->GetPointData()->GetScalars());
          for(int indexScalar=0;indexScalar<imageData->GetPointData()->GetScalars()->GetNumberOfTuples();indexScalar++)
          {
            scalars->SetTuple1(indexScalar,scalars->GetTuple1(indexScalar)*slope+intercept);//modify scalars using slope and intercept
          }
        }
        else if (imageData->GetScalarType() == VTK_SHORT)
        {
          vtkShortArray *scalars=vtkShortArray::SafeDownCast(imageData->GetPointData()->GetScalars());
          for(int indexScalar=0;indexScalar<imageData->GetPointData()->GetScalars()->GetNumberOfTuples();indexScalar++)
          {
            scalars->SetTuple1(indexScalar,scalars->GetTuple1(indexScalar)*slope+intercept);//modify scalars using slope and intercept
          }
        }
        else if (imageData->GetScalarType() == VTK_CHAR)
        {
          vtkCharArray *scalars=vtkCharArray::SafeDownCast(imageData->GetPointData()->GetScalars());
          for(int indexScalar=0;indexScalar<imageData->GetPointData()->GetScalars()->GetNumberOfTuples();indexScalar++)
          {
            scalars->SetTuple1(indexScalar,scalars->GetTuple1(indexScalar)*slope+intercept);//modify scalars using slope and intercept
          }
        }
        
        imageData->Update();
      }
////////////////////////////////////////////////////   

      const char *mode = "?";
      ds->findAndGetString(DCM_Modality,mode);

      const char *patPos = "?";
      ds->findAndGetString(DCM_PatientPosition,patPos);
      m_PatientPosition = patPos;

      const char *studyUID = "?";
      ds->findAndGetString(DCM_StudyInstanceUID,studyUID);

      const char *seriesUID = "?";
      ds->findAndGetString(DCM_SeriesInstanceUID,seriesUID);

      //vector of string composed by:
      //-studyUID
      //-seriesUID
      //-name to applied to the "seriers listbox"
      std::vector<mafString> studyAndSeriesVec;
      
      studyAndSeriesVec.push_back(studyUID);
      studyAndSeriesVec.push_back(seriesUID);
      mafString seriesName = "series_";
      seriesName.Append(mode);
      seriesName.Append("_");

      bool seriesExist = false;
      int seriesCounter = 0;

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

        //if(m_DicomMap.find(studyAndSeriesVec) == m_DicomMap.end()) 
        std::map<std::vector<mafString>,medListDICOMFiles*>::iterator it;
        for ( it=m_DicomMap.begin() ; it != m_DicomMap.end(); it++ )
        {
          if(studyAndSeriesVec.at(0) == (*it).first.at(0))
          {
            seriesCounter++;
            if (studyAndSeriesVec.at(1) == (*it).first.at(1))
            {
              studyAndSeriesVec.push_back((*it).first.at(2));
              seriesExist = true;
              break;
            }
          }
        }
        if (!seriesExist)
        {
          m_NumberOfStudy++;
					// the study is not present into the listbox, so need to create new
					// list of files related to the new studyID
          medListDICOMFiles *filesList = new medListDICOMFiles;
          m_DicomTypeRead=-1;

          if(ds->findAndGetFloat64(DCM_SliceLocation,slice_pos[2]).bad())
          {
            //Unable to get element: DCM_SliceLocation;
            slice_pos[0] = imagePositionPatient[0];
            slice_pos[1] = imagePositionPatient[1];
            slice_pos[2] = imagePositionPatient[2];
          } 
          else
          {
            ds->findAndGetFloat64(DCM_SliceLocation,slice_pos[2],0);
            ds->findAndGetFloat64(DCM_SliceLocation,slice_pos[1],1);
            ds->findAndGetFloat64(DCM_SliceLocation,slice_pos[0],2);
          }
          lastZPos = slice_pos[2];

          seriesName.Append(wxString::Format("%i_%ix%i",seriesCounter, height, width));
          studyAndSeriesVec.push_back(seriesName);

          filesList->Append(new medImporterDICOMListElements(m_FileName,slice_pos, imageOrientationPatient, imageData));
          m_DicomMap.insert(std::pair<std::vector<mafString>,medListDICOMFiles*>(studyAndSeriesVec,filesList));
          if (!this->m_TestMode)
          {
            FillStudyListBox(studyAndSeriesVec);
          }
				}
				else 
				{
          if(ds->findAndGetFloat64(DCM_SliceLocation,slice_pos[2]).bad())
          {
            //Unable to get element: DCM_SliceLocation;
            slice_pos[0] = imagePositionPatient[0];
            slice_pos[1] = imagePositionPatient[1];
            slice_pos[2] = imagePositionPatient[2];
          } 
          else
          {
            ds->findAndGetFloat64(DCM_SliceLocation,slice_pos[2],0);
            ds->findAndGetFloat64(DCM_SliceLocation,slice_pos[1],1);
            ds->findAndGetFloat64(DCM_SliceLocation,slice_pos[0],2);
          }
          
          if  (sliceNum > 1)
          {
            double distancePercentage = ((lastDistance - (fabs(lastZPos - slice_pos[2])))*100)/lastDistance;

            // Check if slices are under tolerance distance
            if ((enableScalarTolerance && (fabs(lastZPos - slice_pos[2]) < scalarTolerance)) || (enablePercentageTolerance) && (distancePercentage > percentageTolerance))
            {
              wxLogMessage(wxString::Format("Warning: file <%s> and <%s> are under distance tolerance.",file.GetCStr(),lastFileName.GetCStr()));
              int answer = wxMessageBox(wxString::Format("Found 2 slices under distance tolerance. Please check the log area for details. Continue?"),"Warning!!", wxYES_NO, NULL);
              if (answer == wxNO)
              {
                if (!this->m_TestMode)
                {
                  mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));
                }
                return false;
              }

            }
            else
            {
              lastDistance = fabs(lastZPos - slice_pos[2]);
            }           
          }

          lastZPos = slice_pos[2];
          

          m_DicomMap[studyAndSeriesVec]->Append(new medImporterDICOMListElements(m_FileName,slice_pos, imageOrientationPatient, imageData));
				}
			}
			else if ( enableToRead && strcmp( (char *)mode, "MR" ) == 0)
			{
        seriesExist = false;
        seriesCounter = 0;
        std::map<std::vector<mafString>,medListDICOMFiles*>::iterator it;
        for ( it=m_DicomMap.begin() ; it != m_DicomMap.end(); it++ )
        {
          if(studyAndSeriesVec.at(0) == (*it).first.at(0))
          {
            seriesCounter++;
              if (studyAndSeriesVec.at(1) == (*it).first.at(1))
              {
                studyAndSeriesVec.push_back((*it).first.at(2));
                seriesExist = true;
                break;
              }
          }
        }
        if (!seriesExist)
				{
          m_NumberOfStudy++;
					// the study is not present into the listbox, so need to create new
					// list of files related to the new studyID
					medListDICOMFiles *filesList = new medListDICOMFiles;
          m_DicomTypeRead=-1;
          if(ds->findAndGetFloat64(DCM_SliceLocation,slice_pos[2]).bad())
          {
            //Unable to get element: DCM_SliceLocation;
            slice_pos[0] = imagePositionPatient[0];
            slice_pos[1] = imagePositionPatient[1];
            slice_pos[2] = imagePositionPatient[2];
          } 
          else
          {
            ds->findAndGetFloat64(DCM_SliceLocation,slice_pos[2],0);
            ds->findAndGetFloat64(DCM_SliceLocation,slice_pos[1],1);
            ds->findAndGetFloat64(DCM_SliceLocation,slice_pos[0],2);
          }
          

          ds->findAndGetLongInt(DCM_InstanceNumber,imageNumber);
          ds->findAndGetLongInt(DCM_CardiacNumberOfImages,numberOfImages);
          ds->findAndGetFloat64(DCM_TriggerTime,trigTime);
          lastZPos = slice_pos[2];

          if(numberOfImages>1)
          {
            if (m_DicomTypeRead==-1)
              m_DicomTypeRead=medGUIDicomSettings::ID_CMRI_MODALITY;
            else if(m_DicomTypeRead!=medGUIDicomSettings::ID_CMRI_MODALITY)
            {
              if(!this->m_TestMode)
              {
                wxString msg = _("cMRI damaged !");
                wxMessageBox(msg,"Confirm", wxOK , NULL);
                return false;
              }
            }
          }
          else
          {
            if (m_DicomTypeRead==-1)
              m_DicomTypeRead=medGUIDicomSettings::ID_MRI_MODALITY;
            else if(m_DicomTypeRead!=medGUIDicomSettings::ID_MRI_MODALITY)
            {
              if(!this->m_TestMode)
              {
                wxString msg = _("cMRI damaged !");
                wxMessageBox(msg,"Confirm", wxOK , NULL);
                return false;
              }
            }
          }

          seriesName.Append(wxString::Format("%i_%ix%i",seriesCounter, height, width));
          studyAndSeriesVec.push_back(seriesName);
         
          filesList->Append(new medImporterDICOMListElements(m_FileName,slice_pos, imageOrientationPatient, imageData, imageNumber, numberOfImages, trigTime));

          m_DicomMap.insert(std::pair<std::vector<mafString>,medListDICOMFiles*>(studyAndSeriesVec,filesList));
          if (!this->m_TestMode)
          {
            FillStudyListBox(studyAndSeriesVec);
          }	
				}
				else 
				{
          if(ds->findAndGetFloat64(DCM_SliceLocation,slice_pos[2]).bad())
          {
            //Unable to get element: DCM_SliceLocation;
            slice_pos[0] = imagePositionPatient[0];
            slice_pos[1] = imagePositionPatient[1];
            slice_pos[2] = imagePositionPatient[2];
          } 
          else
          {
            ds->findAndGetFloat64(DCM_SliceLocation,slice_pos[2],0);
            ds->findAndGetFloat64(DCM_SliceLocation,slice_pos[1],1);
            ds->findAndGetFloat64(DCM_SliceLocation,slice_pos[0],2);
          }

          ds->findAndGetLongInt(DCM_InstanceNumber,imageNumber);
          ds->findAndGetLongInt(DCM_CardiacNumberOfImages,numberOfImages);
          ds->findAndGetFloat64(DCM_TriggerTime,trigTime);

          // Check if slices are under tolerance distance
          // Commented because the problem is present in TAC dataset which are not time varying
          /*if (imageNumber % numberOfImages == 1)
          {
            double distancePercentage = ((lastDistance - (fabs(lastZPos - slice_pos[2])))*100)/lastDistance;

            // Check if slices are under tolerance distance
            if ((enableScalarTolerance && (fabs(lastZPos - slice_pos[2]) < scalarTolerance)) || (enablePercentageTolerance) && (distancePercentage > percentageTolerance))
            {
            wxLogMessage(wxString::Format("Warning: file <%s> and <%s> are under distance tolerance.",file.GetCStr(),lastFileName.GetCStr()));
            int answer = wxMessageBox(wxString::Format("Found 2 slices under distance tolerance. Please check the log area for details. Continue?"),"Warning!!", wxYES_NO, NULL);
            if (answer == wxNO)
            {
            if (!this->m_TestMode)
            {
            mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));
            }
            return false;
            }

            }
            else
            {
            lastDistance = fabs(lastZPos - slice_pos[2]);
            }  
            lastZPos = slice_pos[2];
          }*/

          m_DicomMap[studyAndSeriesVec]->Append(new medImporterDICOMListElements(m_FileName,slice_pos,imageOrientationPatient ,imageData,imageNumber,numberOfImages,trigTime));
				}
			}
      if (!this->m_TestMode)
      {
        progress = i * 100 / m_DirectoryReader->GetNumberOfFiles();
			  mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,progress));
      }
      dicomImg.clear();
      studyAndSeriesVec.clear();
		}
    lastFileName = m_FileName;
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
    if (errorOccurred)
    {
      if(!this->m_TestMode)
      {
        wxMessageBox("Some errors occurred while importing data. Please check the log area for details.");
      }
    }
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
    m_SeriesListbox->Clear();
  }
  
  std::map<std::vector<mafString>,medListDICOMFiles*>::iterator it;
  for ( it=m_DicomMap.begin() ; it != m_DicomMap.end(); it++ )
  {
    m_DicomMap[(*it).first]->DeleteContents(TRUE);
    m_DicomMap[(*it).first]->Clear();
  }

  m_DicomMap.clear();

  if(!this->m_TestMode)
  {
    m_StudyListbox->Clear();
    m_SeriesListbox->Clear();
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


  if (this->m_TestMode)
  {
    m_VectorSelected = m_DicomMap.begin()->first;
  }

  m_ListSelected = m_DicomMap[m_VectorSelected];

  medImporterDICOMListElements *element0;
  element0 = (medImporterDICOMListElements *)m_ListSelected->Item(0)->GetData();

  int numberOfImages =  element0->GetNumberOfImages();

	int numberOfDicomSlices = m_ListSelected->GetCount();
  int numSlicesPerTS;
  if (numberOfDicomSlices >= numberOfImages)
  {
     numSlicesPerTS = numberOfDicomSlices / numberOfImages;
  } 
  else
  {
    numSlicesPerTS = numberOfImages / numberOfDicomSlices;
  }
	//assert(numberOfDicomSlices % numberOfImages == 0);

	int maxHeigthId = numSlicesPerTS - 1; 
	int maxTimeId = numberOfImages - 1; 

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
  m_Text = "";

	m_ListSelected->Item(slice_num)->GetData()->GetSliceLocation(loc);
	m_ListSelected->Item(slice_num)->GetData()->GetOutput()->Update();
	m_ListSelected->Item(slice_num)->GetData()->GetOutput()->GetBounds(m_DicomBounds);

  double Origin[3];
  m_ListSelected->Item(slice_num)->GetData()->GetOutput()->GetOrigin(Origin);

  double orientation[9] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  m_ListSelected->Item(slice_num)->GetData()->GetSliceOrientation(orientation);
  m_Text.append(wxString::Format("Orientaion: %f, %f, %f, %f, %f, %f \nPosition: %f, %f, %f",orientation[0], orientation[1], orientation[2], orientation[3], orientation[4], orientation[5], Origin[0], Origin[1], Origin[2]));
  m_TextMapper->SetInput(m_Text.c_str());
  m_TextMapper->Modified();

	if (m_CropFlag) 
	{
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

    /*int k = 0;
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
*/

    double dim_x_clip = ceil((double)(((crop_bounds[1] - crop_bounds[0]) / spacing[0]) + 1));
    double dim_y_clip = ceil((double)(((crop_bounds[3] - crop_bounds[2]) / spacing[1]) + 1));

		vtkMAFSmartPointer<vtkStructuredPoints> clip;
		clip->SetOrigin(crop_bounds[0], crop_bounds[2], loc[m_SortAxes]);// Origin[m_SortAxes]);	
    clip->SetSpacing(spacing[0], spacing[1], 0);
		clip->SetDimensions(dim_x_clip, dim_y_clip, 1);
		clip->Update();

		vtkMAFSmartPointer<vtkProbeFilter> probe;
		probe->SetInput(clip);
		probe->SetSource(m_ListSelected->Item(slice_num)->GetData()->GetOutput());
    probe->Update();
    probe->GetOutput()->GetBounds(m_DicomBounds);

    //rescale to 16 bit
    if(m_RescaleTo16Bit == TRUE && m_HighBit == 11)
    {
      RescaleTo16Bit(m_ListSelected->Item(slice_num)->GetData()->GetOutput());
    }
		m_SliceTexture->SetInput((vtkImageData *)probe->GetOutput());
	} 
	else 
	{
    //rescale to 16 bit
    if(m_RescaleTo16Bit == TRUE && m_HighBit == 11)
    {
      RescaleTo16Bit(m_ListSelected->Item(slice_num)->GetData()->GetOutput());
    }

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
vtkImageData* medOpImporterDicomOffis::GetFirstSlice(mafString sliceName)
//----------------------------------------------------------------------------
{
  wxString name, path, short_name, ext;
  for (int i = 0; i < m_ListSelected->GetCount(); i++)
  {
    name = m_ListSelected->Item(i)->GetData()->GetFileName();
    wxSplitPath(name, &path, &short_name, &ext);
    if (sliceName.Compare(short_name) == 0)
    {
      m_ListSelected->Item(i)->GetData()->GetOutput()->Update();
      return m_ListSelected->Item(i)->GetData()->GetOutput();
    }
    else 
    {
      //if dicom file has not extension fit wxSplitPath error
      short_name = short_name + "." + ext;
      if (sliceName.Compare(short_name) == 0)
      {
        m_ListSelected->Item(i)->GetData()->GetOutput()->Update();
        return m_ListSelected->Item(i)->GetData()->GetOutput();
      }
    }
  }
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
  DcmRLEDecoderRegistration::registerCodecs();
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
  DcmRLEDecoderRegistration::cleanup();
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


  mafSmartPointer<mafTransformFrame> local_pose;
  local_pose->SetInput(box_pose);

  mafSmartPointer<mafTransformFrame> output_to_input;

  // In a future version if not a "Natural" data the filter should operate in place.
  mafString new_vme_name = "resampled_";
  new_vme_name += m_VolumeName;

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
        mafVME::SafeDownCast(m_Input)->GetOutput()->GetAbsMatrix(*input_parent_abs_pose.GetPointer(),input_item->GetTimeStamp());
        local_pose->SetTargetFrame(input_parent_abs_pose);
        local_pose->Update();

        mafSmartPointer<mafMatrix> output_abs_pose;
        m_Volume->GetOutput()->GetAbsMatrix(*output_abs_pose.GetPointer(),input_item->GetTimeStamp());
        output_to_input->SetInputFrame(box_pose->GetMatrixPointer());

        mafSmartPointer<mafMatrix> input_abs_pose;
        mafVME::SafeDownCast(m_Input)->GetOutput()->GetAbsMatrix(*input_abs_pose.GetPointer(),input_item->GetTimeStamp());
        output_to_input->SetTargetFrame(input_abs_pose);
        output_to_input->Update();

        double origin[3];
        origin[0] = volumeBounds[0];
        origin[1] = volumeBounds[2];
        origin[2] = volumeBounds[4];

        output_to_input->TransformPoint(origin,origin);
        resampler->SetVolumeOrigin(origin[0],origin[1],origin[2]);
        vtkMatrix4x4 *mat = output_to_input->GetMatrix().GetVTKMatrix();

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
        if(m_DiscardPosition == TRUE)
        {
          output_data->SetOrigin(0,0,0);
        }
        else
        {
          output_data->SetOrigin(volumeBounds[0],volumeBounds[2],volumeBounds[4]);
        }
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
void medOpImporterDicomOffis::RescaleTo16Bit(vtkImageData *dataSet)
//----------------------------------------------------------------------------
{
  if(dataSet->GetScalarType() == VTK_UNSIGNED_SHORT) return;
  
  int i=0, size = dataSet->GetNumberOfPoints();
  vtkMAFSmartPointer<vtkUnsignedShortArray> newScalars;
  for(;i<size;i++)
  {
    double value = dataSet->GetPointData()->GetScalars()->GetTuple1(i);
    value = (value - m_RescaleIntercept) * ((double)VTK_UNSIGNED_SHORT_MAX) / (4095.);
    newScalars->InsertNextTuple1(value);
  }
  dataSet->GetPointData()->SetScalars(newScalars);
  dataSet->SetScalarTypeToUnsignedShort();
  dataSet->Update();

}
//----------------------------------------------------------------------------
int CompareX(const medImporterDICOMListElements **arg1,const medImporterDICOMListElements **arg2)
//----------------------------------------------------------------------------
{
	// compare the x coordinate of both arguments
	// return:
	double x1 = (*(medImporterDICOMListElements **)arg1)->GetCoordinate(0);
	double x2 = (*(medImporterDICOMListElements **)arg2)->GetCoordinate(0);
	if (x1 > x2)
		return 1;
	if (x1 < x2)
		return -1;
	else
		return 0;
}
//----------------------------------------------------------------------------
int CompareY(const medImporterDICOMListElements **arg1,const medImporterDICOMListElements **arg2)
//----------------------------------------------------------------------------
{
	// compare the y coordinate of both arguments
	// return:
	double y1 = (*(medImporterDICOMListElements **)arg1)->GetCoordinate(1);
	double y2 = (*(medImporterDICOMListElements **)arg2)->GetCoordinate(1);
	if (y1 > y2)
		return 1;
	if (y1 < y2)
		return -1;
	else
		return 0;
}
//----------------------------------------------------------------------------
int CompareZ(const medImporterDICOMListElements **arg1,const medImporterDICOMListElements **arg2)
//----------------------------------------------------------------------------
{
	// compare the z coordinate of both arguments
	// return:
	double z1 = (*(medImporterDICOMListElements **)arg1)->GetCoordinate(2);
	double z2 = (*(medImporterDICOMListElements **)arg2)->GetCoordinate(2);
	if (z1 > z2)
		return 1;
	if (z1 < z2)
		return -1;
	else
		return 0;
}
//----------------------------------------------------------------------------
int CompareTriggerTime(const medImporterDICOMListElements **arg1,const medImporterDICOMListElements **arg2)
//----------------------------------------------------------------------------
{
	// compare the trigger time of both arguments
	// return:
	float t1 = (*(medImporterDICOMListElements **)arg1)->GetTriggerTime();
	float t2 = (*(medImporterDICOMListElements **)arg2)->GetTriggerTime();;
	if (t1 > t2)
		return 1;
	if (t1 < t2)
		return -1;
	else
		return 0;
}

//----------------------------------------------------------------------------
int CompareImageNumber(const medImporterDICOMListElements **arg1,const medImporterDICOMListElements **arg2)
//----------------------------------------------------------------------------
{
	// compare the trigger time of both arguments
	// return:
	float i1 = (*(medImporterDICOMListElements **)arg1)->GetImageNumber();
	float i2 = (*(medImporterDICOMListElements **)arg2)->GetImageNumber();;
	if (i1 > i2)
		return 1;
	if (i1 < i2)
		return -1;
	else
		return 0;
}
