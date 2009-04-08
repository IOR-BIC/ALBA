/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpImporterDicomGrassroots.cpp,v $
  Language:  C++
  Date:      $Date: 2009-04-08 10:28:40 $
  Version:   $Revision: 1.1.2.5 $
  Authors:   Roberto Mucci
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medOpImporterDicomGrassroots.h"

#include <wx/listimpl.cpp>
#include "wx/busyinfo.h"

#include "mafEvent.h"
#include "mafRWIBase.h"
#include "mafGUIValidator.h"
#include "mafGUI.h"
#include "mafRWI.h"
#include "mafGUIDialogPreview.h"

#include "mmdMouse.h"
#include "mmiDICOMImporterInteractor.h"

#include "mafVMEVolumeGray.h"
#include "mafVMEImage.h"
#include "mafTagArray.h"
#include "mafVMEItem.h"

//GDCM library
#include "vtkGDCMImageReader.h"
#include "gdcmImageReader.h"
#include "gdcmDataElement.h"
#include "vtkMedicalImageProperties.h"
#include "gdcmReader.h"
#include "gdcmFileMetaInformation.h"
#include "gdcmGlobal.h"
#include "gdcmDicts.h"
#include "gdcmDict.h"
#include "gdcmDefs.h"
//

#include "vtkMAFSmartPointer.h"
#include "vtkRendererCollection.h"
#include "vtkCamera.h"
#include "vtkPlaneSource.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkActor.h"
#include "vtkDicomUnPacker.h"
#include "vtkDirectory.h"
#include "vtkTexture.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkProbeFilter.h"
#include "vtkStructuredPoints.h"
#include "vtkImageData.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkOutlineFilter.h"
#include "vtkMAFRGSliceAccumulate.h"
#include "vtkRectilinearGrid.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"

/*#ifdef VME_VOLUME_LARGE
#include "vtkPointData.h"
#include "mafStorage.h"
#include "mafEventIO.h"
#include "../../openMAF/IO/mafVolumeLargeWriter.h"
#include "../../openMAF/vtkMAF/vtkMAFLargeImageReader.h"
#include "../../openMAF/vtkMAF/vtkMAFLargeImageData.h"
#include "../../openMAF/vtkMAF/vtkMAFLargeDataSetCallback.h"
#include "../../openMAF/vtkMAF/vtkMAFFileDataProvider.h"
#endif // VME_VOLUME_LARGE*/





int compareX(const mmoDICOMImporterListElementGrassroots **arg1,const mmoDICOMImporterListElementGrassroots **arg2);
int compareY(const mmoDICOMImporterListElementGrassroots **arg1,const mmoDICOMImporterListElementGrassroots **arg2);
int compareZ(const mmoDICOMImporterListElementGrassroots **arg1,const mmoDICOMImporterListElementGrassroots **arg2);
int compareTriggerTime(const mmoDICOMImporterListElementGrassroots **arg1,const mmoDICOMImporterListElementGrassroots **arg2);
int compareImageNumber(const mmoDICOMImporterListElementGrassroots **arg1,const mmoDICOMImporterListElementGrassroots **arg2);

WX_DEFINE_LIST(ListDicomFilesGrassroots);

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpImporterDicomGrassroots);
//----------------------------------------------------------------------------

#include "mafMemDbg.h"

//----------------------------------------------------------------------------
// constants :
//----------------------------------------------------------------------------
enum DICOM_IMPORTER_MODALITY
{
	CROP_SELECTED,
	ADD_CROP_ITEM,
	GIZMO_NOT_EXIST,
	GIZMO_RESIZING,
	GIZMO_DONE
};

//----------------------------------------------------------------------------
medOpImporterDicomGrassroots::medOpImporterDicomGrassroots(wxString label) : mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_IMPORTER;
	m_Canundo	= true;

	for (int i = 0; i < 6; i++) 
		m_DicomBounds[i] = 0;
	m_Volume	= NULL;
  m_Image	= NULL;
	m_SortAxes				= 2;
	m_NumberOfStudy		= 0;
	m_NumberOfSlices	= 0;
	m_NumberOfTimeFrames = 0;
	m_DICOMType = -1;
  m_CurrentID = -1;


	m_DICOMDir	= mafGetApplicationDirectory().c_str();
  m_DICOMDir += "\\Data\\External\\";
	m_CurrentSlice			  = 0;
	m_CurrentTime				  = 0;
  m_BuildStepValue			= 0;
	m_BuildStepChoices[0]	= "1x";
	m_BuildStepChoices[1]	= "2x";
  m_BuildStepChoices[2]	= "3x";
	m_BuildStepChoices[3] = "4x";
	
  m_PatientName	= " ";
	m_SurgeonName = " ";
	m_Identifier	= " ";

  m_SliceText     = NULL;
	m_SliceScanner	= NULL;
	m_SliceLabel		= NULL;

	m_TimeText     = NULL;
	m_TimeScanner	= NULL;
	m_TimeLabel		= NULL;

	m_CropMode = false;
	m_CropFlag = false;

	m_FilesList   = NULL;
	//m_FilesListCineMRI = NULL;
	m_DicomDialog	= NULL;
	m_TagArray 		= NULL;

  m_SliceTexture      = NULL;
  m_CTDirectoryReader	= NULL;
	m_DicomReader			  = NULL;
	m_CropPlane		 			= NULL;
  m_SlicePlane 				= NULL;
	m_SliceMapper				= NULL;
	m_SliceActor				= NULL;
	m_SliceLookupTable	= NULL;
  m_DicomInteractor = NULL;
	
	m_GizmoStatus = GIZMO_NOT_EXIST;
	m_SideToBeDragged = 0;

  m_BoxCorrect = false;

/*#ifdef VME_VOLUME_LARGE
  m_VolumeLarge = NULL;
  m_MemLimit = 16;	//memory limit in MB, default is 16 MB
#endif // VME_VOLUME_LARGE*/
}
//----------------------------------------------------------------------------
medOpImporterDicomGrassroots::~medOpImporterDicomGrassroots()
//----------------------------------------------------------------------------
{
/*#ifdef VME_VOLUME_LARGE
  vtkDEL(m_VolumeLarge);
#endif*/
  mafDEL(m_Output);
}
//----------------------------------------------------------------------------
mafOp *medOpImporterDicomGrassroots::Copy()
//----------------------------------------------------------------------------
{
	return new medOpImporterDicomGrassroots(m_Label);
}
//----------------------------------------------------------------------------
void medOpImporterDicomGrassroots::OpRun()   
//----------------------------------------------------------------------------
{
	CreatePipeline();
	CreateGui();
}
//----------------------------------------------------------------------------
void medOpImporterDicomGrassroots::OpStop(int result)
//----------------------------------------------------------------------------
{
	if(m_DicomInteractor)
		m_Mouse->RemoveObserver(m_DicomInteractor);

  //close dialog
	for (int i=0; i < m_NumberOfStudy;i++)
	{
    if(!this->m_TestMode)
    {
		  ((ListDicomFilesGrassroots *)m_StudyListbox->GetClientData(i))->DeleteContents(TRUE);
		  ((ListDicomFilesGrassroots *)m_StudyListbox->GetClientData(i))->Clear();
    }  
	}
  std::map<int,ListDicomFilesGrassroots*>::iterator it;
  for ( it=m_DicomMap.begin() ; it != m_DicomMap.end(); it++ )
  {
    m_DicomMap[(*it).first]->DeleteContents(TRUE);
    m_DicomMap[(*it).first]->Clear();
  }
     
  m_DicomMap.clear();

  if(!this->m_TestMode)
  {
    m_DicomDialog->GetRWI()->m_RenFront->RemoveActor(m_SliceActor);   //SIL. 28-11-2003: - you must always remove actors from the Renderer before cleaning
    m_DicomDialog->GetRWI()->m_RenFront->RemoveActor(m_CropActor);   
  }
  	
	vtkDEL(m_SliceTexture);
	vtkDEL(m_CTDirectoryReader);
	vtkDEL(m_DicomReader);
	vtkDEL(m_SliceLookupTable);
	vtkDEL(m_SlicePlane);
	vtkDEL(m_SliceMapper);
	vtkDEL(m_SliceActor);
	vtkDEL(m_CropPlane);
	vtkDEL(m_CropActor);

	mafDEL(m_TagArray);
  mafDEL(m_DicomInteractor);

	cppDEL(m_DicomDialog);
	cppDEL(m_FilesList);
  

  m_Volume = NULL;
  m_Image = NULL;

	mafEventMacro(mafEvent(this,result));
/*#ifdef VME_VOLUME_LARGE
  if (result == OP_RUN_OK && m_VolumeLarge != NULL)
  {
    //save the VME data, it should not prompt for saving
    mafEventMacro( mafEvent(this, MENU_FILE_SAVE));
  }
#endif*/
}
//----------------------------------------------------------------------------
void medOpImporterDicomGrassroots::OpDo()
//----------------------------------------------------------------------------
{
	//assert(m_Volume != NULL);
  //assert(m_Image != NULL);
  /*if(m_Image != NULL)
    m_Output = m_Image;
  if(m_Volume != NULL)
    m_Output = m_Volume;*/
/*#ifdef VME_VOLUME_LARGE
  if(m_VolumeLarge != NULL)
    mafEventMacro(mafEvent(this,VME_ADD,m_VolumeLarge));
#endif*/
  m_Output->ReparentTo(m_Input);

}
//----------------------------------------------------------------------------
void medOpImporterDicomGrassroots::OpUndo()
//----------------------------------------------------------------------------
{   
	//assert(m_Volume != NULL);
  //assert(m_Image != NULL);
  if(m_Image != NULL)
	  mafEventMacro(mafEvent(this,VME_REMOVE,m_Image));
  if(m_Volume != NULL)
    mafEventMacro(mafEvent(this,VME_REMOVE,m_Volume));
/*#ifdef VME_VOLUME_LARGE
  if(m_VolumeLarge != NULL)
    mafEventMacro(mafEvent(this,VME_REMOVE,m_VolumeLarge));
#endif*/
}
//----------------------------------------------------------------------------
void medOpImporterDicomGrassroots::CreatePipeline()
//----------------------------------------------------------------------------
{
	vtkNEW(m_CTDirectoryReader);
	vtkNEW(m_DicomReader);
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
// Constants :
//----------------------------------------------------------------------------
enum DICOM_IMPORTER_ID
{
	ID_FIRST = MINID,
	ID_OPEN_DIR,
	ID_SCAN_SLICE,
	ID_CROP_MODE_BUTTON,
	ID_BUILDVOLUME_MODE_BUTTON,
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
	ID_CT,
	ID_MRI,
	ID_CMRI,
  ID_BUILD_ALWAYS_RLG,
};
//----------------------------------------------------------------------------
void medOpImporterDicomGrassroots::CreateGui()
//----------------------------------------------------------------------------
{
	m_DicomDialog = new mafGUIDialogPreview("dicom importer", mafCLOSEWINDOW | mafRESIZABLE | mafUSEGUI | mafUSERWI);
	int x_init,y_init;
	x_init = mafGetFrame()->GetPosition().x;
	y_init = mafGetFrame()->GetPosition().y-20;

	mafString wildcard = "DICT files (*.dic)|*.dic|All Files (*.*)|*.*";

	m_Gui = new mafGUI(this);
	m_Gui->SetListener(this);

	m_Gui->Label("data files:",true);
	m_Gui->DirOpen(ID_OPEN_DIR, "Folder",	&m_DICOMDir);
	m_Gui->Divider();
	m_Gui->Label("patient info:",true);
  m_Gui->Label("name ", &m_PatientName);
  m_Gui->Label("id ",&m_Identifier);
	m_StudyListbox = m_Gui->ListBox(ID_STUDY,"study id",50);
	m_Gui->Label("surgeon info:",true);
	m_Gui->String(ID_SURGEON_NAME,"name ",&m_SurgeonName);
	m_Gui->Label("mode",true);
	m_Gui->Button(ID_CROP_MODE_BUTTON,"crop mode", "");
	m_Gui->Button(ID_BUILDVOLUME_MODE_BUTTON,"build volume mode", "");
	m_Gui->Label("crop",true);
	m_Gui->Button(ID_CROP_BUTTON,"crop", "");
	m_Gui->Button(ID_UNDO_CROP_BUTTON,"undo crop", "");
	m_Gui->Label("build volume",true);
	m_Gui->Combo(ID_BUILD_STEP, "step", &m_BuildStepValue, 4, m_BuildStepChoices);
  m_Gui->Bool(ID_BUILD_ALWAYS_RLG, _("build rectilinear grid"), &m_BuildRLG, 1, 
    _("if the DICOM data is stored in a regular grid and this option is not checked, "  \
    "the data will be imported as a regular grid; otherwise it will be imported as a rectilinear grid"));

/*#ifdef VME_VOLUME_LARGE
#ifdef __WIN32__
  MEMORYSTATUSEX ms; 
  ms.dwLength = sizeof(ms);
  GlobalMemoryStatusEx(&ms);

  int nMaxMem = ms.ullTotalPhys / (1024*1024);	//available memory
#else
  int nMaxMem = 2048;	//some constant
#endif

  nMaxMem /= 4;		//keep 75% free

  m_Gui->Label(_("memory limit [MB]: "));
  m_Gui->Slider(ID_MEMLIMIT, "", &m_MemLimit, 1, nMaxMem, 
    _("if the data to be loaded is larger than the specified memory limit,"
    "it will be loaded as VolumeLarge VME"));

  m_Gui->Divider(0);
#endif // VME_VOLUME_LARGE*/

	m_Gui->Button(ID_BUILD_BUTTON,"build", "");
	m_Gui->Divider();
	m_Gui->Button(ID_CANCEL,"cancel");

	//m_Gui->Enable(ID_OPEN_DIR,m_DictionaryFilename != "");
	m_Gui->Enable(ID_CROP_MODE_BUTTON,0);
	m_Gui->Enable(ID_BUILDVOLUME_MODE_BUTTON,0);
	m_Gui->Enable(ID_CROP_BUTTON,0);
	m_Gui->Enable(ID_UNDO_CROP_BUTTON,0);
	m_Gui->Enable(ID_BUILD_STEP,0);
	m_Gui->Enable(ID_BUILD_BUTTON,0);
	
	m_Gui->Show(true);
  m_Gui->Update();

	// slice scanner
	wxPoint dp = wxDefaultPosition;
	m_SliceLabel = new wxStaticText(m_DicomDialog, -1, " slice num. ",dp, wxSize(-1,16));
	m_SliceText = new wxTextCtrl(m_DicomDialog, -1, "", dp, wxSize(30,16), wxNO_BORDER);
	m_SliceScanner = new wxSlider(m_DicomDialog, -1, 0, 0, VTK_INT_MAX, dp, wxSize(200,22));
	m_SliceLabel->Enable(false);
	m_SliceText->Enable(false);
	m_SliceScanner->Enable(false);

	m_SliceScanner->SetValidator(mafGUIValidator(this,ID_SCAN_SLICE,m_SliceScanner,&m_CurrentSlice,m_SliceText));
  m_SliceText->SetValidator(mafGUIValidator(this,ID_SCAN_SLICE,m_SliceText,&m_CurrentSlice,m_SliceScanner,0,VTK_INT_MAX));

	wxBoxSizer *slice_sizer = new wxBoxSizer(wxHORIZONTAL);
	slice_sizer->Add(m_SliceLabel, 0, wxALIGN_CENTER|wxRIGHT, 5);
	slice_sizer->Add(m_SliceText, 0, wxALIGN_CENTER|wxRIGHT, 5);
	slice_sizer->Add(m_SliceScanner, 1, wxALIGN_CENTER|wxEXPAND);
	
	// time scanner
	m_TimeLabel = new wxStaticText(m_DicomDialog, -1, " time num. ",dp, wxSize(-1,16));
	m_TimeText = new wxTextCtrl(m_DicomDialog, -1, "", dp, wxSize(30,16), wxNO_BORDER);
	m_TimeScanner = new wxSlider(m_DicomDialog, -1, 0, 0, 100, dp, wxSize(200,22));
	m_TimeLabel->Enable(false);
	m_TimeText->Enable(false);
	m_TimeScanner->Enable(false);

	m_TimeScanner->SetValidator(mafGUIValidator(this,ID_SCAN_TIME,m_TimeScanner,&m_CurrentTime,m_TimeText));
  m_TimeText->SetValidator(mafGUIValidator(this,ID_SCAN_TIME,m_TimeText,&m_CurrentTime,m_TimeScanner,0,100));

	wxBoxSizer *time_sizer = new wxBoxSizer(wxHORIZONTAL);
	time_sizer->Add(m_TimeLabel, 0, wxALIGN_CENTER|wxRIGHT, 5);
	time_sizer->Add(m_TimeText, 0, wxALIGN_CENTER|wxRIGHT, 5);
	time_sizer->Add(m_TimeScanner, 1, wxALIGN_CENTER|wxEXPAND);

  m_DicomDialog->GetGui()->AddGui(m_Gui);
	m_DicomDialog->GetRWI()->SetSize(0,0,380,200);
  m_DicomDialog->m_RwiSizer->Add(slice_sizer, 0, wxEXPAND);
	m_DicomDialog->m_RwiSizer->Add(time_sizer, 0, wxEXPAND);
  m_DicomDialog->GetRWI()->SetListener(this);
  m_DicomDialog->GetRWI()->SetSize(0,0,500,500);
  m_DicomDialog->GetRWI()->CameraSet(CAMERA_BACK);
  m_DicomDialog->GetRWI()->m_RenFront->AddActor(m_SliceActor);
  m_DicomDialog->GetRWI()->m_RenFront->AddActor(m_CropActor);
  m_DicomDialog->GetRWI()->m_RwiBase->SetMouse(m_Mouse);

  int w,h;
  m_DicomDialog->GetSize(&w,&h);
  m_DicomDialog->SetSize(x_init+10,y_init+10,w,h);

	m_DicomDialog->ShowModal();
	int res = (m_DicomDialog->GetReturnCode() == wxID_OK) ? OP_RUN_OK : OP_RUN_CANCEL;

	OpStop(res);
	return;
}

//----------------------------------------------------------------------------
void medOpImporterDicomGrassroots::FillListBox()
//----------------------------------------------------------------------------
{
  int row = m_StudyListbox->FindString(m_DicomReader->GetMedicalImageProperties()->GetStudyID());//StudyUID
  if (row == -1)
  {
    m_StudyListbox->Append(m_DicomReader->GetMedicalImageProperties()->GetStudyID());
    m_StudyListbox->SetClientData(m_NumberOfStudy,(void *)m_FilesList);
  }
}

//----------------------------------------------------------------------------
void medOpImporterDicomGrassroots::BuildDicomFileList(const char *dir)
//----------------------------------------------------------------------------
{
  int i;
  double slice_pos[3];
  int imageNumber = -1;
  int cardNumImages = -1;
  double trigTime = -1.0;
  m_DICOMType = -1;

  if (m_CTDirectoryReader->Open(dir) == 0)
  {
    wxMessageBox(wxString::Format("Directory <%s> can not be opened",dir),"Warning!!");
    return;
  }
  if(!this->m_TestMode)
  {
    mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
    long progress = 0;
    // get the dicom files from the directory
    wxBusyInfo wait_info("Reading DICOM directory: please wait");
  }

  m_DICOMType=ID_CT;
  for (i=0; i < m_CTDirectoryReader->GetNumberOfFiles(); i++)
  {
    if ((strcmp(m_CTDirectoryReader->GetFile(i),".") == 0) || (strcmp(m_CTDirectoryReader->GetFile(i),"..") == 0)) 
    {
      continue;
    }
    else
    {
      m_CurrentSliceName = m_CTDirectoryReader->GetFile(i);

      // Append of the path at the dicom file
      wxString str_tmp, ct_mode;
      str_tmp.Append(dir);
      str_tmp.Append("\\");
      str_tmp.Append(m_CurrentSliceName);

      if (m_DicomReader->CanReadFile(str_tmp.c_str()))
      {
        m_DicomReader->SetFileName(str_tmp.c_str());
        m_DicomReader->Update();
      }
      else
      {
        continue;
      }

      ct_mode = m_DicomReader->GetMedicalImageProperties()->GetModality();
      
      //row = m_StudyListbox->FindString(m_DicomReader->GetMedicalImageProperties()->GetStudyID());//StudyUID
      //if (row == -1)
      int id = atoi(m_DicomReader->GetMedicalImageProperties()->GetStudyID());
      if(m_DicomMap.find(id) == m_DicomMap.end()) 
      {
        // the study is not present into the listbox, so need to create new
        // list of files related to the new studyID
        m_FilesList = new ListDicomFilesGrassroots;
        //m_DicomMap.insert(id, m_FilesList);


        m_DicomMap.insert(std::pair<int,ListDicomFilesGrassroots*>(id,m_FilesList));

        if (!this->m_TestMode)
        {
          FillListBox();
        }
        
        //m_StudyListbox->Append(m_DicomReader->GetMedicalImageProperties()->GetStudyID());
        //m_StudyListbox->SetClientData(m_NumberOfStudy,(void *)m_FilesList);
        m_DicomReader->GetImagePositionPatient(slice_pos);
        if (strcmp(ct_mode.c_str(),"MR") == 0)
        {
          m_DICOMType=-1;
          //attribute stuff
          gdcm::Reader imageReader;
          imageReader.SetFileName(m_DicomReader->GetFileName());
          imageReader.Read();

          gdcm::DataSet const& ds = imageReader.GetFile().GetDataSet();
          const gdcm::DataElement &ref  = ds.GetDataElement(gdcm::Tag(0x020,0x013));
          wxString instanceNumber = ref.GetByteValue()->GetPointer();
          instanceNumber.Truncate(ref.GetByteValue()->GetLength());

          const gdcm::DataElement &ref1  = ds.GetDataElement(gdcm::Tag(0x018,0x1090));
          wxString cardiacNumberOfImages = ref1.GetByteValue()->GetPointer();
          cardiacNumberOfImages.Truncate(ref1.GetByteValue()->GetLength());

          const gdcm::DataElement &ref2  = ds.GetDataElement(gdcm::Tag(0x018,0x1060));
          wxString triggerTime = ref2.GetByteValue()->GetPointer();
          triggerTime.Truncate(ref2.GetByteValue()->GetLength());

          imageNumber = atof(instanceNumber.c_str());
          cardNumImages = atof(cardiacNumberOfImages.c_str());
          trigTime = atof(triggerTime.c_str());
          if(cardNumImages>1)
          {
            if (m_DICOMType==-1)
              m_DICOMType=ID_CMRI;
            else if(m_DICOMType!=ID_CMRI)
            {
              wxString msg = "cMRI damaged !";
              wxMessageBox(msg,"Confirm", wxOK , NULL);
              return;
            }
          }
          else
          {
            if (m_DICOMType==-1)
              m_DICOMType=ID_MRI;
            else if(m_DICOMType!=ID_MRI)
            {
              wxString msg = "cMRI damaged !";
              wxMessageBox(msg,"Confirm", wxOK , NULL);
              return;
            }
          }
          m_FilesList->Append(new mmoDICOMImporterListElementGrassroots(str_tmp,slice_pos,imageNumber, cardNumImages, trigTime));
          m_NumberOfStudy++;
        }
        else
        {
          m_FilesList->Append(new mmoDICOMImporterListElementGrassroots(str_tmp,slice_pos));
          m_NumberOfStudy++;
        }
      }
      else 
      {
        m_DicomReader->GetImagePositionPatient(slice_pos);
        if (strcmp(ct_mode.c_str(),"MR") == 0)
        {
          //attribute stuff
          gdcm::Reader imageReader;
          imageReader.SetFileName(m_DicomReader->GetFileName());
          imageReader.Read();

          gdcm::DataSet const& ds = imageReader.GetFile().GetDataSet();
          const gdcm::DataElement &ref  = ds.GetDataElement(gdcm::Tag(0x020,0x013));
          wxString instanceNumber = ref.GetByteValue()->GetPointer();
          instanceNumber.Truncate(ref.GetByteValue()->GetLength());

          const gdcm::DataElement &ref1  = ds.GetDataElement(gdcm::Tag(0x018,0x1090));
          wxString cardiacNumberOfImages = ref1.GetByteValue()->GetPointer();
          cardiacNumberOfImages.Truncate(ref1.GetByteValue()->GetLength());

          const gdcm::DataElement &ref2  = ds.GetDataElement(gdcm::Tag(0x018,0x1060));
          wxString triggerTime = ref2.GetByteValue()->GetPointer();
          triggerTime.Truncate(ref2.GetByteValue()->GetLength());

          imageNumber = atof(instanceNumber.c_str());
          cardNumImages = atof(cardiacNumberOfImages.c_str());
          trigTime = atof(triggerTime.c_str());
          m_DicomMap[id]->Append(new mmoDICOMImporterListElementGrassroots(str_tmp,slice_pos,imageNumber,cardNumImages,trigTime));
          //((ListDicomFilesGrassroots *)m_StudyListbox->GetClientData(row))->Append(new mmoDICOMImporterListElementGrassroots(str_tmp,slice_pos,imageNumber,cardNumImages,trigTime));
        }
        else
        {
          //((ListDicomFilesGrassroots *)m_StudyListbox->GetClientData(row))->Append(new mmoDICOMImporterListElementGrassroots(str_tmp,slice_pos));
          m_DicomMap[id]->Append(new mmoDICOMImporterListElementGrassroots(str_tmp,slice_pos));
        }         
      }
    }
    if(!this->m_TestMode)
    {
      long progress = i * 100 / m_CTDirectoryReader->GetNumberOfFiles();
      mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,progress));
    }
  }

  if(!this->m_TestMode)
  {
    mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));
  }
  if(m_NumberOfStudy == 0)
  {
    wxString msg = "No study found!";
    wxMessageBox(msg,"Confirm", wxOK , NULL);
  }
}
//----------------------------------------------------------------------------
void medOpImporterDicomGrassroots::BuildVolumeCineMRI()
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

/*#ifdef VME_VOLUME_LARGE
  if (IsVolumeLarge(n_slices))
  {
    wxString msg = _("WARNING: The selected VOI is too large to fit the given memory limit, however, "      
      "as the data is in CineMRI format that is not supported by VolumeLarge VME, it "
      "will be imported as Volume VME (small). This may cause a crash of the application.\n"      
      "\nDo you want to proceed with the import?");

    if (wxMessageBox(msg, _("Warning: VolumeLarge VME"), 
      wxYES_NO | wxCENTRE | wxICON_QUESTION) != wxYES)
      return;    
  }
#endif //VME_VOLUME_LARGE*/

  if(!this->m_TestMode)
  {
    wxBusyInfo wait_info("Building volume: please wait");
  }

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
    
    mmoDICOMImporterListElementGrassroots *element0;
    element0 = (mmoDICOMImporterListElementGrassroots *)m_ListSelected->Item(tsImageId)->GetData();
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
        CreateSlice(currImageId);
        ShowSlice();
      }

      accumulate->SetSlice(targetVolumeSliceId, m_SliceTexture->GetInput());
      targetVolumeSliceId++;
	  }
  
		/*mafVME *cTItem =new mafVME;
	  cTItem->SetData(accumulate->GetOutput());
    cTItem->SetTimeStamp(tsDouble);*/
		accumulate->Update();

    //BES: 15.7.2008 - convert the data from rectilinear grid to image data, if possible
    vtkImageData* pImgData = GetImageData(accumulate->GetOutput());
    if (pImgData == NULL)
      m_Volume->SetDataByDetaching(accumulate->GetOutput(),tsDouble);
    else
    {
      m_Volume->SetDataByDetaching(pImgData,tsDouble);
      pImgData->Delete();
    }

    // clean up
	  //vtkDEL(cTItem);
    //vtkDEL(accumulate);
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

	//Nome VME = CTDir + IDStudio
	wxString name = m_DICOMDir + " - " + m_StudyListbox->GetString(m_StudyListbox->GetSelection());			
	m_Volume->SetName(name.c_str());
  if(m_Volume != NULL)
    m_Output = m_Volume;
	
}
//----------------------------------------------------------------------------
void medOpImporterDicomGrassroots::BuildVolume()
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
/*#ifdef VME_VOLUME_LARGE
  if (!VolumeLargeCheck(n_slices))
    return; //the volume is large but the user cancel it

  bool bIsLarge = IsVolumeLarge(n_slices);    
#endif //VME_VOLUME_LARGE*/


  if(!this->m_TestMode)
  {
    wxBusyInfo wait_info("Building volume: please wait");
  }

  vtkMAFSmartPointer<vtkMAFRGSliceAccumulate> accumulate;
/*#ifdef VME_VOLUME_LARGE   
  vtkMAFSmartPointer<vtkDoubleArray> s_coords;
  s_coords->Allocate(n_slices);
  double* pSCoords = s_coords->WritePointer(0, n_slices);

  vtkMAFFile2* pLargeFile = NULL;
  wxString szLargeFileName;
  try
  {    
    if (bIsLarge)
    {
      szLargeFileName = wxFileName::CreateTempFileName("mmoDICOMImporter_BES");

      pLargeFile = vtkMAFFile2::New();
      pLargeFile->Create(szLargeFileName);    
    }
    else
    {
#endif //VME_VOLUME_LARGE*/
      accumulate->SetNumberOfSlices(n_slices);
      accumulate->BuildVolumeOnAxes(m_SortAxes);
/*#ifdef VME_VOLUME_LARGE
    }
#endif //VME_VOLUME_LARGE*/

    if(!this->m_TestMode)
    {
      mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
    }

    long progress = 0;
    int count,s_count;
    for (count = 0, s_count = 0; count < m_NumberOfSlices; count += step)
    {
      if (s_count == n_slices) {break;}
      if(!this->m_TestMode)
      {
        CreateSlice(count);
        ShowSlice();
      }

/*#ifdef VME_VOLUME_LARGE
      if (!bIsLarge)
#endif //VME_VOLUME_LARGE*/
        accumulate->SetSlice(s_count, m_SliceTexture->GetInput());
/*#ifdef VME_VOLUME_LARGE
      else
      {
        //store the texture into the temporary file
        vtkImageData* pImgData = m_SliceTexture->GetInput();
        vtkDataArray* pScalars = pImgData->GetPointData()->GetScalars();
        pLargeFile->Write(pScalars->GetVoidPointer(0),
          (pScalars->GetMaxId() + 1)*pScalars->GetDataTypeSize());

        pSCoords[s_count] = pImgData->GetOrigin()[m_SortAxes];
      }
#endif //VME_VOLUME_LARGE*/
      s_count++;
      if(!this->m_TestMode)
      {
        progress = count * 100 / m_CTDirectoryReader->GetNumberOfFiles();
        mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,progress));
      }

    }
    if(!this->m_TestMode)
    {
      mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));
    }

/*#ifdef VME_VOLUME_LARGE
    if (bIsLarge)    
      pLargeFile->Delete(); //will close the file    
  }
  catch (std::exception& e)
  {    
    wxMessageBox(wxString::Format(_("Unable to create temporary file '%s'\nError: %s"), 
      szLargeFileName, e.what()));

    pLargeFile->Delete();
    return; //fatal error    
  }
#endif //VME_VOLUME_LARGE*/

  ImportDicomTags();
  if(m_NumberOfSlices == 1)
  {
/*#ifdef VME_VOLUME_LARGE
    assert(bIsLarge == false);
#endif*/
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

    //Nome VME = CTDir + IDStudio
    wxString name = m_DICOMDir + " - ";
    name.Append(wxString::Format("%d", m_CurrentID));		
    m_Image->SetName(name.c_str());
    if(m_Image != NULL)
      m_Output = m_Image;
  }
  else if(m_NumberOfSlices > 1)
  {
    //Copy inside the first VME item of m_Volume the CT volume and Dicom's tags
    //mafVME* pOutVME;
/*#ifdef VME_VOLUME_LARGE
    if (bIsLarge)
    {
      if (!ImportLargeRAWFile(szLargeFileName, s_coords.GetPointer()))      
        return; //error      

      pOutVME = m_VolumeLarge;
    }
    else
    {
#endif //VME_VOLUME_LARGE*/
      mafNEW(m_Volume);
      m_Volume->GetTagArray()->DeepCopy(m_TagArray);
      //mafDEL(m_TagArray);

      accumulate->Update();

      //BES: 15.7.2008 - convert the data from rectilinear grid to image data, if possible
      vtkImageData* pImgData = GetImageData(accumulate->GetOutput());
      if (pImgData == NULL)
      {
        m_Volume->SetDataByDetaching(accumulate->GetOutput(), 0);
      }
      else
      {
        m_Volume->SetDataByDetaching(pImgData, 0);
        pImgData->Delete();
      }      

     // pOutVME = m_Volume;
/*#ifdef VME_VOLUME_LARGE
    }
#endif //VME_VOLUME_LARGE*/


    //pOutVME->GetTagArray()->DeepCopy(m_TagArray);
    mafDEL(m_TagArray);

    mafTagItem tag_Nature;
    tag_Nature.SetName("VME_NATURE");
    tag_Nature.SetValue("NATURAL");
    m_Volume->GetTagArray()->SetTag(tag_Nature);

    mafTagItem tag_Surgeon;
    tag_Surgeon.SetName("SURGEON_NAME");
    tag_Surgeon.SetValue(m_SurgeonName.GetCStr());
    m_Volume->GetTagArray()->SetTag(tag_Surgeon);

    //Nome VME = CTDir + IDStudio
    wxString name = m_DICOMDir + " - ";
    name.Append(wxString::Format("%d", m_CurrentID));	
    m_Volume->SetName(name.c_str());
    //mafDEL(pOutVME);
  }
  if(m_Volume != NULL)
    m_Output = m_Volume;
}
//----------------------------------------------------------------------------
void medOpImporterDicomGrassroots::ImportDicomTags()
//----------------------------------------------------------------------------
{
  if (m_TagArray == NULL) 
    mafNEW(m_TagArray);

	m_TagArray->SetName("TagArray");

  //attribute stuff
  gdcm::Reader imageReader;
  imageReader.SetFileName(m_DicomReader->GetFileName());
  imageReader.Read();

  gdcm::DataSet const& ds = imageReader.GetFile().GetDataSet();

  gdcm::DataSet::ConstIterator it = ds.Begin();
  for( ; it != ds.End(); ++it)
  {
    const gdcm::DataElement &ref = *it;
    if (ref.IsEmpty())
    {
      continue;
    }
    const gdcm::ByteValue *bv = ref.GetByteValue();
    if (bv == NULL)
    {
      continue;
    }
    wxString attributeValue = bv->GetPointer();
    attributeValue.Truncate(ref.GetByteValue()->GetLength());

    //dictionary stuff
    gdcm::Global& g = gdcm::Global::GetInstance();
    g.LoadResourcesFiles();
    const gdcm::Dicts &ds = g.GetDicts();
    const gdcm::Dict &pub = ds.GetPublicDict();
    const gdcm::PrivateDict &priv = ds.GetPrivateDict();

    const gdcm::DictEntry& de = pub.GetDictEntry( gdcm::Tag(ref.GetTag()));
    const char *attributeName = de.GetName();
    if (!attributeValue.IsEmpty() && attributeValue.IsAscii())
    {
      if(attributeValue.Length()==1)
      {
        if (attributeValue.IsWord() || attributeValue.IsNumber())
        {
          m_TagArray->SetTag(mafTagItem(attributeName,attributeValue.c_str()));
        }
      }
      else
      {
        m_TagArray->SetTag(mafTagItem(attributeName,attributeValue.c_str()));
        //m_TagArray->GetTag(attributeName)->SetValue(attributeValue.c_str());
      }
    }
  }
}
//----------------------------------------------------------------------------
void medOpImporterDicomGrassroots::CreateSlice(int slice_num)
//----------------------------------------------------------------------------
{
  m_SliceTexture->InterpolateOn();
	// Description:
	// read the slice number 'slice_num' and generate the texture
  double spacing[3], crop_bounds[6], range[2], loc[3];

  m_DicomReader->SetFileName((char *)m_ListSelected->Item(slice_num)->GetData()->GetFileName());
	m_DicomReader->Update();

	m_DicomReader->GetImagePositionPatient(loc);

	m_DicomReader->GetOutput()->GetBounds(m_DicomBounds);
  
	// switch from m_DicomReader and v_dicom_probe on m_CropFlag
	if (m_CropFlag) 
	{
		double Origin[3];
		m_DicomReader->GetOutput()->GetOrigin(Origin);
    
		m_DicomReader->GetOutput()->GetSpacing(spacing);
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
		probe->SetSource(m_DicomReader->GetOutput());
		probe->Update();
		probe->GetOutput()->GetBounds(m_DicomBounds);
		probe->GetOutput()->GetScalarRange(range);
		m_SliceTexture->SetInput((vtkImageData *)probe->GetOutput());
	} 
	else 
	{
    m_DicomReader->GetOutput()->GetScalarRange(range);
    m_SliceTexture->SetInput(m_DicomReader->GetOutput());
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
void medOpImporterDicomGrassroots::ShowSlice()
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
vtkImageData* medOpImporterDicomGrassroots::GetSliceImageData(int slice_num)
//----------------------------------------------------------------------------
{
  m_DicomReader->SetFileName((char *)m_ListSelected->Item(slice_num)->GetData()->GetFileName());
  m_DicomReader->Update();

  return (vtkImageData *)m_DicomReader->GetOutput();
}
//----------------------------------------------------------------------------
void medOpImporterDicomGrassroots::ResetSliders()
//----------------------------------------------------------------------------
{
	m_SliceScanner->SetRange(0,m_NumberOfSlices - 1);
	m_SliceText->SetValidator(mafGUIValidator(this,ID_SCAN_SLICE,m_SliceText,&m_CurrentSlice,m_SliceScanner,0,m_NumberOfSlices-1));
	m_TimeScanner->SetRange(0,m_NumberOfTimeFrames -1);
	m_TimeText->SetValidator(mafGUIValidator(this,ID_SCAN_TIME,m_TimeText,&m_CurrentTime,m_TimeScanner,0,m_NumberOfTimeFrames-1));
	m_Gui->Update();
}
//----------------------------------------------------------------------------
void medOpImporterDicomGrassroots::ResetStructure()
//----------------------------------------------------------------------------
{
	// disable the scan slider
	//m_Gui->Enable(ID_SCAN_SLICE,0);
	m_SliceLabel->Enable(false);
	m_SliceText->Enable(false);
	m_SliceScanner->Enable(false);

	m_TimeLabel->Enable(false);
	m_TimeText->Enable(false);
	m_TimeScanner->Enable(false);
	
	// disable the button modality
	m_Gui->Enable(ID_CROP_MODE_BUTTON,0);
	m_Gui->Enable(ID_BUILDVOLUME_MODE_BUTTON,0);
	m_Gui->Enable(ID_CROP_BUTTON,0);
	m_Gui->Enable(ID_UNDO_CROP_BUTTON,0);
	// disable the build volume modality widgets
	m_Gui->Enable(ID_BUILD_STEP,0);	
	m_Gui->Enable(ID_BUILD_BUTTON,0);

	// delete the previous studyes detected and reset the related variables
	for (int i=0; i < m_NumberOfStudy;i++)
	{
		((ListDicomFilesGrassroots *)m_StudyListbox->GetClientData(i))->DeleteContents(true);
		((ListDicomFilesGrassroots *)m_StudyListbox->GetClientData(i))->Clear();    
	}
  std::map<int,ListDicomFilesGrassroots*>::iterator it;
  for ( it=m_DicomMap.begin() ; it != m_DicomMap.end(); it++ )
  {
    m_DicomMap[(*it).first]->DeleteContents(TRUE);
    m_DicomMap[(*it).first]->Clear();
  }
  m_DicomMap.clear();
	m_StudyListbox->Clear();
	m_NumberOfStudy		= 0;
	m_NumberOfSlices	= 0;
	m_CurrentSlice		= 0;
  m_NumberOfTimeFrames = 0;
  m_CurrentTime      = 0; 
	m_DICOMType				 = -1;

	m_CropFlag				= false;
	
	m_Gui->Update();
	mafYield();
}
//----------------------------------------------------------------------------
int compareX(const mmoDICOMImporterListElementGrassroots **arg1,const mmoDICOMImporterListElementGrassroots **arg2)
//----------------------------------------------------------------------------
{
	// compare the x coordinate of both arguments
	// return:
	double x1 = (*(mmoDICOMImporterListElementGrassroots **)arg1)->GetCoordinate(0);
	double x2 = (*(mmoDICOMImporterListElementGrassroots **)arg2)->GetCoordinate(0);
	if (x1 > x2)
		return 1;
	if (x1 < x2)
		return -1;
	else
		return 0;
}
//----------------------------------------------------------------------------
int compareY(const mmoDICOMImporterListElementGrassroots **arg1,const mmoDICOMImporterListElementGrassroots **arg2)
//----------------------------------------------------------------------------
{
	// compare the y coordinate of both arguments
	// return:
	double y1 = (*(mmoDICOMImporterListElementGrassroots **)arg1)->GetCoordinate(1);
	double y2 = (*(mmoDICOMImporterListElementGrassroots **)arg2)->GetCoordinate(1);
	if (y1 > y2)
		return 1;
	if (y1 < y2)
		return -1;
	else
		return 0;
}
//----------------------------------------------------------------------------
int compareZ(const mmoDICOMImporterListElementGrassroots **arg1,const mmoDICOMImporterListElementGrassroots **arg2)
//----------------------------------------------------------------------------
{
	// compare the z coordinate of both arguments
	// return:
	double z1 = (*(mmoDICOMImporterListElementGrassroots **)arg1)->GetCoordinate(2);
	double z2 = (*(mmoDICOMImporterListElementGrassroots **)arg2)->GetCoordinate(2);
	if (z1 > z2)
		return 1;
	if (z1 < z2)
		return -1;
	else
		return 0;
}
//----------------------------------------------------------------------------
int compareTriggerTime(const mmoDICOMImporterListElementGrassroots **arg1,const mmoDICOMImporterListElementGrassroots **arg2)
//----------------------------------------------------------------------------
{
	// compare the trigger time of both arguments
	// return:
	float t1 = (*(mmoDICOMImporterListElementGrassroots **)arg1)->GetTriggerTime();
	float t2 = (*(mmoDICOMImporterListElementGrassroots **)arg2)->GetTriggerTime();;
	if (t1 > t2)
		return 1;
	if (t1 < t2)
		return -1;
	else
		return 0;
}

//----------------------------------------------------------------------------
int compareImageNumber(const mmoDICOMImporterListElementGrassroots **arg1,const mmoDICOMImporterListElementGrassroots **arg2)
//----------------------------------------------------------------------------
{
	// compare the trigger time of both arguments
	// return:
	float i1 = (*(mmoDICOMImporterListElementGrassroots **)arg1)->GetImageNumber();
	float i2 = (*(mmoDICOMImporterListElementGrassroots **)arg2)->GetImageNumber();;
	if (i1 > i2)
		return 1;
	if (i1 < i2)
		return -1;
	else
		return 0;
}
//----------------------------------------------------------------------------
void medOpImporterDicomGrassroots::ImportDicom(const char *dir) 
//----------------------------------------------------------------------------
{
  // scan dicom directory
  BuildDicomFileList(dir);

  if(!this->m_TestMode)
  {
    if(m_NumberOfStudy == 1)
    {
      m_StudyListbox->SetSelection(0);
      OnEvent(&mafEvent(this, ID_STUDY));
    }
  }
}

//----------------------------------------------------------------------------
void medOpImporterDicomGrassroots::ReadDicom() 
//----------------------------------------------------------------------------
{
  int sel = 0;
  if(!this->m_TestMode)
  {
    sel = m_StudyListbox->GetSelection();
    m_ListSelected = (ListDicomFilesGrassroots *)m_StudyListbox->GetClientData(sel);
  }
  else
  {
    sel = m_DicomMap.begin()->first;
    m_ListSelected = m_DicomMap[sel];
  }
  
  // sort dicom slices
  if(m_ListSelected->GetCount() > 1)
  {
    double item1_pos[3],item2_pos[3],d[3];
    mmoDICOMImporterListElementGrassroots *element1;
    mmoDICOMImporterListElementGrassroots *element2;
    element1 = (mmoDICOMImporterListElementGrassroots *)m_ListSelected->Item(0)->GetData();
    element2 = (mmoDICOMImporterListElementGrassroots *)m_ListSelected->Item(1)->GetData();
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

  m_NumberOfTimeFrames = ((mmoDICOMImporterListElementGrassroots *)m_ListSelected->Item(0)->GetData())->GetCardiacNumberOfImages();
  if(m_DICOMType == ID_CMRI) //If cMRI
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
    }
  }

  //modified by STEFY 9-7-2003(begin)
  ImportDicomTags();
  mafTagItem *patient_name;
  mafTagItem *patient_id;

  const char* p_name;
  double p_id = 0;

  bool position = m_TagArray->IsTagPresent("Patient's Name");
  if (position)
  {
    patient_name = m_TagArray->GetTag("Patient's Name");
    p_name = patient_name->GetValue();
  }
  else 
    p_name = NULL;

  position = m_TagArray->IsTagPresent("Patient ID");
  if (position)
  {
    patient_id = m_TagArray->GetTag("Patient ID");
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
void medOpImporterDicomGrassroots::OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_CANCEL:
        m_DicomDialog->EndModal(wxID_CANCEL);
        //OpStop(OP_RUN_CANCEL); //mafEventMacro(mafEvent(this,OP_RUN_CANCEL));
      break;
      case ID_OPEN_DIR:
        m_Gui->Update();
        ResetStructure();
        ImportDicom(m_DICOMDir.GetCStr());
        
        // scan dicom directory
       /* BuildDicomFileList(m_DICOMDir.GetCStr());
        if(m_NumberOfStudy == 1)
        {
          m_StudyListbox->SetSelection(0);
          OnEvent(&mafEvent(this, ID_STUDY));
        }*/
      break;
      case ID_STUDY:
      {
        m_CurrentID = atoi(m_StudyListbox->GetString(m_StudyListbox->GetSelection()));
        if(!this->m_TestMode)
        {
          //m_Gui->Enable(ID_SCAN_SLICE,1);
          m_SliceLabel->Enable(true);
          m_SliceText->Enable(true);
          m_SliceScanner->Enable(true);
				  if(m_DICOMType == ID_CMRI)//If cMRI
				  {
					  m_TimeLabel->Enable(true);
					  m_TimeText->Enable(true);
					  m_TimeScanner->Enable(true);
				  }
          m_Gui->Enable(ID_CROP_MODE_BUTTON,1);
          m_Gui->Enable(ID_UNDO_CROP_BUTTON,0);
          m_Gui->Enable(ID_BUILDVOLUME_MODE_BUTTON,1);
        }

        ReadDicom();
        m_DicomDialog->GetRWI()->CameraReset();
        ResetSliders();
        m_DicomDialog->GetRWI()->CameraUpdate();
        m_Gui->Update();
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
						m_DicomDialog->GetRWI()->CameraUpdate();
					}
					m_SliceScanner->SetValue(m_CurrentSlice);
					m_DicomDialog->Update();
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
						m_DicomDialog->GetRWI()->CameraUpdate();
					}
					m_TimeScanner->SetValue(m_CurrentTime);
					m_DicomDialog->Update();
				}
      break;
      case ID_CROP_MODE_BUTTON:
        m_CropMode = true;		
        m_Gui->Enable(ID_CROP_BUTTON,1);
        if(m_CropFlag)
          m_Gui->Enable(ID_UNDO_CROP_BUTTON,1);
        else
          m_Gui->Enable(ID_UNDO_CROP_BUTTON,0);
        m_Gui->Enable(ID_BUILD_STEP,0);
        m_Gui->Enable(ID_BUILD_BUTTON,0);
        m_DicomDialog->GetRWI()->CameraUpdate();
      break;
      case ID_CROP_BUTTON:
				{     
        if( !m_BoxCorrect )
        {
          wxMessageBox("Error on selecting the box");
          return;
        }

        m_CropFlag = true;
        CreateSlice(m_CurrentSlice);
        ShowSlice();
				m_CropActor->VisibilityOff();
				double diffx,diffy,boundsCamera[6];
				diffx=m_DicomBounds[1]-m_DicomBounds[0];
				diffy=m_DicomBounds[3]-m_DicomBounds[2];
				boundsCamera[0]=0.0;
				boundsCamera[1]=diffx;
				boundsCamera[2]=0.0;
				boundsCamera[3]=diffy;
				boundsCamera[4]=0.0;
				boundsCamera[5]=0.0;

				m_DicomDialog->GetRWI()->CameraReset(boundsCamera);
        m_DicomDialog->GetRWI()->CameraUpdate();
        m_Gui->Enable(ID_UNDO_CROP_BUTTON,1);
				}
      break;
      case ID_UNDO_CROP_BUTTON:
				{
        m_CropFlag = false;
        CreateSlice(m_CurrentSlice);
        ShowSlice();
				double diffx,diffy,boundsCamera[6];
				diffx=m_DicomBounds[1]-m_DicomBounds[0];
				diffy=m_DicomBounds[3]-m_DicomBounds[2];
				boundsCamera[0]=0.0;
				boundsCamera[1]=diffx;
				boundsCamera[2]=0.0;
				boundsCamera[3]=diffy;
				boundsCamera[4]=0.0;
				boundsCamera[5]=0.0;
				m_DicomDialog->GetRWI()->CameraReset(boundsCamera);
        m_CropActor->VisibilityOn();
        m_DicomDialog->GetRWI()->CameraUpdate();
        m_Gui->Enable(ID_UNDO_CROP_BUTTON,0);
				}
      break;
      case ID_BUILDVOLUME_MODE_BUTTON:
        m_CropMode = false;
        m_CropActor->VisibilityOff();
        m_DicomDialog->GetRWI()->CameraUpdate();
        m_Gui->Enable(ID_CROP_BUTTON,0);
        m_Gui->Enable(ID_UNDO_CROP_BUTTON,0);
        m_Gui->Enable(ID_BUILD_STEP,1);
        m_Gui->Enable(ID_BUILD_BUTTON,1);
      break;
      case ID_BUILD_BUTTON:
				if(m_DICOMType == ID_CT || m_DICOMType == ID_MRI)
					BuildVolume();
				else if(m_DICOMType == ID_CMRI)
					BuildVolumeCineMRI();
				else
					break;;
        m_DicomDialog->EndModal(wxID_OK);
        //OpStop(OP_RUN_OK); 
      break;	
      case MOUSE_DOWN:
      {
        if (m_CropMode == true)
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
          m_DicomDialog->GetRWI()->CameraUpdate();
        }
      }
      break;
      case MOUSE_MOVE:  //ridimensiona il gizmo
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
        m_DicomDialog->GetRWI()->CameraUpdate();
      }
      break;
      case MOUSE_UP:  //blocca il gizmo
      {
        if (m_CropMode == true)
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
      break; 
			//case ID_TYPE_DICOM:
				/*if(m_DICOM==0)
				{
					m_TimeLabel->Enable(false);
					m_TimeText->Enable(false);
					m_TimeScanner->Enable(false);
				}
				else if(m_DICOM==1)
				{
					m_TimeLabel->Enable(false);
					m_TimeText->Enable(false);
					m_TimeScanner->Enable(false);
				}
				else if(m_DICOM==2)
				{
					m_TimeLabel->Enable(true);
					m_TimeText->Enable(true);
					m_TimeScanner->Enable(true);
				}*/
				//break;
      default:
        mafEventMacro(*e);
      break;
    }	
  }
}
//----------------------------------------------------------------------------
int medOpImporterDicomGrassroots::GetImageId(int timeId, int heigthId)
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

   cardiacNumberOfImages = 4 (from wxList element);
   maxTimeId = cardiacNumberOfImages - 1 = 3;
   
   numberOfDicomSlices = 12 (from wxList);

   numSlicesPerTS = numberOfDicomSlices / cardiacNumberOfImages = 12 / 4 = 3;
   maxHeigthId = numSlicesPerTS - 1;    
  
    test:                
    GetImageId(3,2) =    4 * hId + tId = 4*2 + 3 = 11 :) 
    GetImageId(1,2) =    4 * 2 + 1 = 9 :)
  
  */


	if (m_DICOMType == ID_CT || m_DICOMType == ID_MRI) //If CT o MRI
		return heigthId;

  assert(m_StudyListbox);
  if (m_StudyListbox == NULL)
    return -1;  //error

  m_ListSelected = (ListDicomFilesGrassroots *)m_StudyListbox->GetClientData(m_StudyListbox->GetSelection());
  
  mmoDICOMImporterListElementGrassroots *element0;
  element0 = (mmoDICOMImporterListElementGrassroots *)m_ListSelected->Item(0)->GetData();
  
  int cardiacNumberOfImages =  element0->GetCardiacNumberOfImages();

  int numberOfDicomSlices = m_ListSelected->GetCount();
    
  int numSlicesPerTS = numberOfDicomSlices / cardiacNumberOfImages;
  //assert(numberOfDicomSlices % cardiacNumberOfImages == 0);

  int maxHeigthId = numSlicesPerTS - 1; // 
  int maxTimeId = cardiacNumberOfImages - 1; // cardiacNumberOfImages - 1;

  if (heigthId < 0 || heigthId > maxHeigthId || timeId < 0 || timeId > maxTimeId )
  {
    return -1;
  }

  return (heigthId * cardiacNumberOfImages + timeId); 
}

//#ifdef VME_VOLUME_LARGE
//------------------------------------------------------------------------
//Detects whether the currently selected volume is large
//Returns true, if chosen ROI*given number of slices is larger than memory limit
/*virtual*/ //bool mmoDICOMImporter_BES::IsVolumeLarge(int nSlices)
//------------------------------------------------------------------------
/*{
  //m_SliceTexture must exist and contain valid slice
  vtkImageData* pImg = m_SliceTexture->GetInput();
  assert(pImg != NULL);
  if (pImg == NULL)
    return false;

  int* pIncr = pImg->GetIncrements();  //returns number of elements in plane (all components)
  int nSize = (int)
    ((((vtkIdType64)pIncr[2])*nSlices*pImg->GetScalarSize()) / (1024*1024));
  return nSize >= m_MemLimit; 
}

//------------------------------------------------------------------------
//if the volume (or VOI) is large, it displays a warning that the volume 
//to be imported is large and returns true, if the operation should continue,
//false otherwise (user canceled the import)
bool mmoDICOMImporter_BES::VolumeLargeCheck(int nSlices)
//------------------------------------------------------------------------
{ 
  //m_SliceTexture must exist and contain valid slice
  vtkImageData* pSlice = m_SliceTexture->GetInput();
  assert(pSlice != NULL);
  if (pSlice == NULL)
    return false;

  int VOI[6];
  pSlice->GetExtent(VOI);
  VOI[4] = 0; VOI[5] = nSlices - 1;

  mafString szStr;
  int nResult = mafVMEVolumeLargeUtils::VolumeLargeCheck(this, m_Listener,
    VOI, pSlice->GetScalarType(), pSlice->GetNumberOfScalarComponents(),
    m_MemLimit, szStr);

  if (nResult == 0)
    return false;
  else if (nResult == 2)
  {  
    wxString name, ext, path;
    wxSplitPath(m_DICOMDir.GetCStr(),&path,&name,&ext);

    m_OutputFileName = szStr;  
    m_OutputFileName += wxFILE_SEP_PATH + wxString::Format("%s_%X", 
      name, (int)time(NULL));  
  }
  return true;
}

//------------------------------------------------------------------------
//Creates VMEVolumeLarge by importing data from the given lpszFileName
//pSCoord contain the grid coordinates in m_SortAxis direction (its count must correspond
//to the number of slices stored in the RAW file lpszFileName
//N.B. File is deleted upon the return. The routine returns false, if an error occurs
bool mmoDICOMImporter_BES::ImportLargeRAWFile(const char* lpszFileName, vtkDoubleArray* pSCoords)
//------------------------------------------------------------------------
{
  //large volume, we need to import it
  int nSlices = pSCoords->GetNumberOfTuples();
  CreateSlice(0);   //enable the first one to get the required information 

  vtkImageData* pSlice = m_SliceTexture->GetInput();
  vtkMAFSmartPointer< vtkMAFLargeImageReader > r;// = vtkMAFLargeImageReader::New();
  r->SetFileName(lpszFileName);
  r->SetNumberOfScalarComponents(pSlice->GetNumberOfScalarComponents());
  r->SetDataScalarType(pSlice->GetScalarType());
  r->SetDataByteOrder(0);
  unsigned short end_test = 0xFFFE;
  if (*((unsigned char*)&end_test) == 0xFF)
    r->SetDataByteOrderToBigEndian();
  else
    r->SetDataByteOrderToLittleEndian();

  int VOI[6];
  pSlice->GetExtent(VOI);
  VOI[4] = 0; VOI[5] = nSlices - 1;

  r->SetDataVOI(VOI);
  r->SetDataExtent(VOI);    
  r->SetDataOrigin(pSlice->GetOrigin());
  r->SetHeaderSize(0);
  r->SetFileDimensionality(3);
  r->SetMemoryLimit(1);                       //fast memory limit

  double sp[3];
  pSlice->GetSpacing(sp);
  bool bRegular = m_BuildRLG == 0 && DetectSpacing(pSCoords, &sp[m_SortAxes]);
  r->SetDataSpacing(sp);
  r->Update();

  mafVolumeLargeWriter wr;
  wr.SetInputDataSet(r->GetOutput());
  if (!bRegular)
  {
    if (m_SortAxes == 0)
      wr.SetInputXCoordinates(pSCoords);
    else if (m_SortAxes == 1)
      wr.SetInputYCoordinates(pSCoords);
    else
      wr.SetInputZCoordinates(pSCoords);
  }
  wr.SetOutputFileName(m_OutputFileName);
  wr.SetListener(this->m_Listener);
  if (!wr.Update())
  {    
    if(!this->m_TestMode)
      wxMessageBox(_("Error while constructing the optimised layout"));
    return false; //error
  }
  mafVolumeLargeReader* rd = new mafVolumeLargeReader();
  rd->SetFileName(m_OutputFileName);
  rd->SetMemoryLimit(m_MemLimit * 1024);
  rd->SetVOI(VOI);
  rd->Update();

  mafNEW(m_VolumeLarge);  
  m_VolumeLarge->SetFileName(m_DICOMDir.GetCStr());
  if (m_VolumeLarge->SetLargeData(rd) != MAF_OK)
  {
    assert(false);

    if(!this->m_TestMode)
      wxMessageBox(_("VMEVolumeLarge internal error"));

    mafDEL(m_VolumeLarge);
    return false;
  }

  if(!this->m_TestMode)
    mafVMEVolumeLargeUtils::DisplayVolumeLargeSpaceConsumtion(rd->GetLevelFilesSize());

#pragma warning(suppress: 6031) // warning C6031: Return value ignored: '_unlink'
  _unlink(lpszFileName);
  return true;
}
#endif // VME_VOLUME_LARGE*/

//------------------------------------------------------------------------
//Converts the given rectilinear grid into a regular grid.
//If the operation cannot be successfully completed (e.g., because
//it is not allowed or it would needed sampling of data),
//it returns NULL, otherwise it constructs a new object
vtkImageData* medOpImporterDicomGrassroots::GetImageData(vtkRectilinearGrid* pInput)
//------------------------------------------------------------------------
{
  if (m_BuildRLG != 0)
    return NULL;  //user disallow it

  vtkDoubleArray* pXYZ[3];
  pXYZ[0] = vtkDoubleArray::SafeDownCast(pInput->GetXCoordinates());
  pXYZ[1] = vtkDoubleArray::SafeDownCast(pInput->GetYCoordinates());
  pXYZ[2] = vtkDoubleArray::SafeDownCast(pInput->GetZCoordinates());

  double sp[3];     //spacing
  double origin[3]; //origin
  for (int i = 0; i < 3; i++) 
  {
    if (pXYZ[i] == NULL || !DetectSpacing(pXYZ[i], &sp[i]))
      return NULL;  //cannot continue

    origin[i] = *(pXYZ[i]->GetPointer(0));
  }

  //we can convert it to image data
  //vtkImageData* pRet = vtkImageData::New();
  vtkMAFSmartPointer<vtkImageData> pRet;
  pRet->SetDimensions(pInput->GetDimensions());
  pRet->SetOrigin(origin);
  pRet->SetSpacing(sp);

  vtkDataArray *scalars = pInput->GetPointData()->GetScalars();
  pRet->SetNumberOfScalarComponents(scalars->GetNumberOfComponents());
  pRet->SetScalarType(scalars->GetDataType());
  pRet->GetPointData()->SetScalars(scalars);
  pRet->SetUpdateExtentToWholeExtent();

  return pRet;
}

//------------------------------------------------------------------------
//Detects spacing in the given array of coordinates.
//It returns false, if the spacing between values is non-uniform
bool medOpImporterDicomGrassroots::DetectSpacing(vtkDoubleArray* pCoords, double* pOutSpacing)
//------------------------------------------------------------------------
{
  int nCount = pCoords->GetNumberOfTuples();
  if (nCount <= 1)        //one slice
  {
    *pOutSpacing = 1.0;
    return true;
  }

  //at least 2 slices => detect min and max spacing
  double* pData = pCoords->GetPointer(0);  
  double dblMin, dblMax;
  dblMax = dblMin = pData[1] - pData[0];

  for (int i = 2; i < nCount; i++)
  {
    double dblSp = pData[i] - pData[i - 1];    
    if (dblSp < dblMin)
      dblMin = dblSp;
    else if (dblSp > dblMax)
      dblMax = dblSp;
  }

  //if the difference between min and max spacing is insignificant,
  //then we can assume the coordinates have uniform spacing
  *pOutSpacing = (pData[nCount - 1] - pData[0]) / nCount;
  if ((dblMax - dblMin) / *pOutSpacing <= 1e-3)  
    return true;  

  *pOutSpacing = 0.0;
  return false;
}