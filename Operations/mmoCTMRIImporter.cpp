/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCTMRIImporter.cpp,v $
  Language:  C++
  Date:      $Date: 2006-11-07 09:33:35 $
  Version:   $Revision: 1.10 $
  Authors:   Paolo Quadrani    Stefano Perticoni
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

#include "mmoCTMRIImporter.h"

#include <wx/listimpl.cpp>
#include "wx/busyinfo.h"

#include "mafEvent.h"
#include "mafRWIBase.h"
#include "mmgValidator.h"
#include "mmgGui.h"
#include "mafRWI.h"
#include "mmgDialogPreview.h"

#include "mmdMouse.h"
#include "mmiDICOMImporterInteractor.h"

#include "mafVMEVolumeGray.h"
#include "mafVMEImage.h"
#include "mafTagArray.h"
#include "mafVMEItem.h"

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
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkOutlineFilter.h"
#include "vtkRGSliceAccumulate.h"

int compareX(const mmoCTMRIImporterListElement **arg1,const mmoCTMRIImporterListElement **arg2);
int compareY(const mmoCTMRIImporterListElement **arg1,const mmoCTMRIImporterListElement **arg2);
int compareZ(const mmoCTMRIImporterListElement **arg1,const mmoCTMRIImporterListElement **arg2);
int compareTriggerTime(const mmoCTMRIImporterListElement **arg1,const mmoCTMRIImporterListElement **arg2);
int compareImageNumber(const mmoCTMRIImporterListElement **arg1,const mmoCTMRIImporterListElement **arg2);

WX_DEFINE_LIST(ListCTMRIFiles);

//----------------------------------------------------------------------------
mafCxxTypeMacro(mmoCTMRIImporter);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------
enum DICOM_IMPORTER_ID
{
	ID_FIRST = MINID,
	ID_DICTIONARY,
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
	ID_TYPE_DICOM,
	ID_SCAN_TIME,
	ID_CT,
	ID_MRI,
	ID_CMRI,
};

//----------------------------------------------------------------------------
mmoCTMRIImporter::mmoCTMRIImporter(wxString label) : mafOp(label)
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
	m_DICOM = 0;
	m_DICOMType = -1;
	
	if(this->m_Label == "CT") m_DICOM = 0;
	else if(this->m_Label == "MRI") m_DICOM = 1;

  m_DictionaryFilename	= "";
  wxString dictionary = mafGetApplicationDirectory().c_str();
  dictionary += "\\Config\\Dictionary\\";
	dictionary.Append("dicom3.dic");
	if(wxFileExists(dictionary)) 
		m_DictionaryFilename = dictionary;

	m_DICOMDir	= mafGetApplicationDirectory().c_str();
  m_DICOMDir += "\\Data\\External\\";
	//m_DICOMDir = "";
	m_CurrentSlice			  = 0;
	m_CurrentTime				  = 0;
  m_BuildStepValue			= 0;
	m_BuildStepChoices[0]	= "1x";
	m_BuildStepChoices[1]	= "2x";
	m_BuildStepChoices[2] = "4x";
	
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
	m_AutoLoad = false;

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
	
	m_GizmoStatus = GIZMO_NOT_EXIST;
	m_SideToBeDragged = 0;
}
//----------------------------------------------------------------------------
mmoCTMRIImporter::~mmoCTMRIImporter()
//----------------------------------------------------------------------------
{
	vtkDEL(m_Volume);
  vtkDEL(m_Image);
}
//----------------------------------------------------------------------------
mafOp *mmoCTMRIImporter::Copy()
//----------------------------------------------------------------------------
{
	return new mmoCTMRIImporter(m_Label);
}
//----------------------------------------------------------------------------
void mmoCTMRIImporter::OpRun()   
//----------------------------------------------------------------------------
{
	CreatePipeline();
	CreateGui();
	if(m_DictionaryFilename!="")
	{
		m_Gui->Enable(ID_OPEN_DIR,1);
		m_Gui->Update();
		if(m_DICOMDir!="" && m_AutoLoad)
		{
			OnEvent(&mafEvent(this, ID_OPEN_DIR));
			m_Gui->Update();
		}
	}
	WaitUser();
}
//----------------------------------------------------------------------------
void mmoCTMRIImporter::OpStop(int result)
//----------------------------------------------------------------------------
{
	if(m_DicomInteractor)
		m_Mouse->RemoveObserver(m_DicomInteractor);

  //close dialog
	for (int i=0; i < m_NumberOfStudy;i++)
	{
		((ListCTMRIFiles *)m_StudyListbox->GetClientData(i))->DeleteContents(TRUE);
		((ListCTMRIFiles *)m_StudyListbox->GetClientData(i))->Clear();
	}

  m_DicomDialog->GetRWI()->m_RenFront->RemoveActor(m_SliceActor);   //SIL. 28-11-2003: - you must always remove actors from the Renderer before cleaning
  m_DicomDialog->GetRWI()->m_RenFront->RemoveActor(m_CropActor);   
	
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

	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void mmoCTMRIImporter::OpDo()
//----------------------------------------------------------------------------
{
	//assert(m_Volume != NULL);
  //assert(m_Image != NULL);
  if(m_Image != NULL)
	  mafEventMacro(mafEvent(this,VME_ADD,m_Image));
  if(m_Volume != NULL)
    mafEventMacro(mafEvent(this,VME_ADD,m_Volume));

}
//----------------------------------------------------------------------------
void mmoCTMRIImporter::OpUndo()
//----------------------------------------------------------------------------
{   
	//assert(m_Volume != NULL);
  //assert(m_Image != NULL);
  if(m_Image != NULL)
	  mafEventMacro(mafEvent(this,VME_REMOVE,m_Image));
  if(m_Volume != NULL)
    mafEventMacro(mafEvent(this,VME_REMOVE,m_Volume));
}
//----------------------------------------------------------------------------
void mmoCTMRIImporter::CreatePipeline()
//----------------------------------------------------------------------------
{
	vtkNEW(m_CTDirectoryReader);

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
void mmoCTMRIImporter::CreateGui()
//----------------------------------------------------------------------------
{
	m_DicomDialog = new mmgDialogPreview("dicom importer", mafCLOSEWINDOW | mafRESIZABLE | mafUSEGUI | mafUSERWI);
	int x_init,y_init;
	x_init = mafGetFrame()->GetPosition().x;
	y_init = mafGetFrame()->GetPosition().y;

	mafString wildcard = "DICT files (*.dic)|*.dic|All Files (*.*)|*.*";

	m_Gui = new mmgGui(this);
	m_Gui->SetListener(this);

	if(m_DICOM == 0) m_Gui->Label("CT Importer",true);
	else if(m_DICOM == 0) m_Gui->Label("MRI Importer",true);

	//m_Gui->Label("Type of DICOM",true);
	//m_Gui->Combo(ID_TYPE_DICOM,"",&m_DICOM,1,TypeOfDICOM);
	m_Gui->Label("data files:",true);
	m_Gui->FileOpen (ID_DICTIONARY,	"dictionary",	&m_DictionaryFilename, wildcard);
	m_Gui->DirOpen(ID_OPEN_DIR, "Folder",	&m_DICOMDir);
	m_Gui->Divider();
	m_Gui->Label("patient info:",true);		
	m_Gui->String(ID_PATIENT_NAME,"name ",&m_PatientName);
	m_Gui->String(ID_PATIENT_ID,"id ",&m_Identifier);
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
	m_Gui->Combo(ID_BUILD_STEP, "step", &m_BuildStepValue, 3, m_BuildStepChoices);
	m_Gui->Button(ID_BUILD_BUTTON,"build", "");
	m_Gui->Divider();
	m_Gui->Button(ID_CANCEL,"cancel");

	m_Gui->Enable(ID_OPEN_DIR,m_DictionaryFilename != "");
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
	m_SliceScanner = new wxSlider(m_DicomDialog, -1, 0, 0, 100, dp, wxSize(200,22));
	m_SliceLabel->Enable(false);
	m_SliceText->Enable(false);
	m_SliceScanner->Enable(false);

	m_SliceScanner->SetValidator(mmgValidator(this,ID_SCAN_SLICE,m_SliceScanner,&m_CurrentSlice,m_SliceText));
  m_SliceText->SetValidator(mmgValidator(this,ID_SCAN_SLICE,m_SliceText,&m_CurrentSlice,m_SliceScanner,0,100));

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

	m_TimeScanner->SetValidator(mmgValidator(this,ID_SCAN_TIME,m_TimeScanner,&m_CurrentTime,m_TimeText));
  m_TimeText->SetValidator(mmgValidator(this,ID_SCAN_TIME,m_TimeText,&m_CurrentTime,m_TimeScanner,0,100));

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
  m_DicomDialog->GetRWI()->CameraSet(CAMERA_CT);
  m_DicomDialog->GetRWI()->m_RenFront->AddActor(m_SliceActor);
  m_DicomDialog->GetRWI()->m_RenFront->AddActor(m_CropActor);
  m_DicomDialog->GetRWI()->m_RwiBase->SetMouse(m_Mouse);

  int w,h;
  m_DicomDialog->GetSize(&w,&h);
  m_DicomDialog->SetSize(x_init+10,y_init+10,w,h);
}
//----------------------------------------------------------------------------
void mmoCTMRIImporter::BuildDicomFileList(const char *dir)
//----------------------------------------------------------------------------
{
	int row, i;
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
	// get the dicom files from the directory
	if(m_DICOM==0)
		wxBusyInfo wait_info("Reading CT directory: please wait");
	if(m_DICOM==1)
		wxBusyInfo wait_info("Reading MRI directory: please wait");
	if(m_DICOM==0)//CT
	{
		m_DICOMType=ID_CT;
		for (i=0; i < m_CTDirectoryReader->GetNumberOfFiles(); i++)
		{
			if ((strcmp(m_CTDirectoryReader->GetFile(i),".") == 0) || (strcmp(m_CTDirectoryReader->GetFile(i),"..") == 0)) 
			{
				continue;
			}
			else
			{
				m_current_slice_name = m_CTDirectoryReader->GetFile(i);
				// Append of the path at the dicom file
				wxString str_tmp, ct_mode;
				str_tmp.Append(dir);
				str_tmp.Append("\\");
				str_tmp.Append(m_current_slice_name);
				
				vtkDicomUnPacker *reader = vtkDicomUnPacker::New();
				reader->SetFileName((char *)str_tmp.c_str());
				reader->UseDefaultDictionaryOff();
				reader->SetDictionaryFileName(m_DictionaryFilename.GetCStr());
				reader->UpdateInformation();

				ct_mode = reader->GetCTMode();
				ct_mode.MakeUpper();
				ct_mode.Trim(FALSE);
				ct_mode.Trim();
				if (strcmp( reader->GetModality(), "CT" ) == 0 )
				{
					//if (strcmp(reader->GetCTMode(),"SCOUT MODE") == 0 || reader->GetStatus() == -1)
					if(ct_mode.Find("SCOUT") != -1 || reader->GetStatus() == -1)
					{
						reader->Delete();
						continue;
					}
					reader->Update();
					//row = m_StudyListbox->FindString(reader->GetStudy());
					row = m_StudyListbox->FindString(reader->GetStudyUID());
					if (row == -1)
					{
						// the study is not present into the listbox, so need to create new
						// list of files related to the new studyID
						m_FilesList = new ListCTMRIFiles;
						//m_StudyListbox->Append(reader->GetStudy());
						m_StudyListbox->Append(reader->GetStudyUID());
						m_StudyListbox->SetClientData(m_NumberOfStudy,(void *)m_FilesList);
						reader->GetSliceLocation(slice_pos);
						//mmoDICOMImporterListElement *element = new mmoDICOMImporterListElement(str_tmp,slice_pos);
						//m_FilesList->Append(element);
						m_FilesList->Append(new mmoCTMRIImporterListElement(str_tmp,slice_pos));
						m_NumberOfStudy++;
					}
					else 
					{
						reader->GetSliceLocation(slice_pos);
						//mmoDICOMImporterListElement *element = new mmoDICOMImporterListElement(str_tmp,SlicePos);
						//((ListCTMRIFiles *)m_StudyListbox->GetClientData(row))->Append(element);
						((ListCTMRIFiles *)m_StudyListbox->GetClientData(row))->Append(new mmoCTMRIImporterListElement(str_tmp,slice_pos));
					}
				}
				reader->Delete();
			}
		}
	}
	else if(m_DICOM==1) //if MRI
	{
		for (i=0; i < m_CTDirectoryReader->GetNumberOfFiles(); i++)
		{
			if ((strcmp(m_CTDirectoryReader->GetFile(i),".") == 0) || (strcmp(m_CTDirectoryReader->GetFile(i),"..") == 0)) 
			{
				continue;
			}
			else
			{
				m_current_slice_name = m_CTDirectoryReader->GetFile(i);
				// Append of the path at the dicom file
				wxString str_tmp, ct_mode;
				str_tmp.Append(dir);
				str_tmp.Append("\\");
				str_tmp.Append(m_current_slice_name);
				
				vtkDicomUnPacker *reader = vtkDicomUnPacker::New();
				reader->SetFileName((char *)str_tmp.c_str());
				reader->UseDefaultDictionaryOff();
				reader->SetDictionaryFileName(m_DictionaryFilename.GetCStr());
				reader->UpdateInformation();

				//ct_mode = reader->GetCTMode();
				//ct_mode.MakeUpper();
				//ct_mode.Trim(FALSE);
				//ct_mode.Trim();
				if ( strcmp( reader->GetModality(), "MR" ) == 0)
				{
				//if (strcmp(reader->GetCTMode(),"SCOUT MODE") == 0 || reader->GetStatus() == -1)
					if(/*ct_mode.Find("SCOUT") != -1 || */ reader->GetStatus() == -1)
					{
						reader->Delete();
						continue;
					}
				
					//row = m_StudyListbox->FindString(reader->GetStudy());
					row = m_StudyListbox->FindString(reader->GetStudyUID());
					if (row == -1)
					{
						// the study is not present into the listbox, so need to create new
						// list of files related to the new studyID
						m_FilesList = new ListCTMRIFiles;
						//m_StudyListbox->Append(reader->GetStudy());
						m_StudyListbox->Append(reader->GetStudyUID());
						m_StudyListbox->SetClientData(m_NumberOfStudy,(void *)m_FilesList);
						reader->GetSliceLocation(slice_pos);
						imageNumber=reader->GetInstanceNumber();
						cardNumImages = reader->GetCardiacNumberOfImages();
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
						trigTime = reader->GetTriggerTime();
						//mmoDICOMImporterListElement *element = new mmoDICOMImporterListElement(str_tmp,slice_pos);
						//m_FilesList->Append(element);
						m_FilesList->Append(new mmoCTMRIImporterListElement(str_tmp,slice_pos,imageNumber, cardNumImages, trigTime));
						m_NumberOfStudy++;
					}
					else 
					{
						reader->GetSliceLocation(slice_pos);
						imageNumber=reader->GetInstanceNumber();
						cardNumImages = reader->GetCardiacNumberOfImages();
						trigTime = reader->GetTriggerTime();
						//mmoDICOMImporterListElement *element = new mmoDICOMImporterListElement(str_tmp,SlicePos);
						//((ListCTMRIFiles *)m_StudyListbox->GetClientData(row))->Append(element);
						((ListCTMRIFiles *)m_StudyListbox->GetClientData(row))->Append(new mmoCTMRIImporterListElement(str_tmp,slice_pos,imageNumber,cardNumImages,trigTime));
					}
				}
				reader->Delete();
			}
		}
	}
	if(m_NumberOfStudy == 0)
	{
		wxString msg = "No study found!";
		wxMessageBox(msg,"Confirm", wxOK , NULL);
	}
}
//----------------------------------------------------------------------------
void mmoCTMRIImporter::BuildVolumeCineMRI()
//----------------------------------------------------------------------------
{
	int step;

	if(m_BuildStepValue == 0)
		step = 1;
	else 
		step = m_BuildStepValue << 1;

	int n_slices = m_NumberOfSlices / step;

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
    accumulate->BuildVolumeOnAxes(2);
    
    // get the time stamp from the dicom tag;
    // timestamp is in ms
    int probeHeigthId = 0;    
    int tsImageId = GetImageId(ts, probeHeigthId);
    if (tsImageId == -1) 
    {
      assert(FALSE);
    }
    
    mmoCTMRIImporterListElement *element0;
    element0 = (mmoCTMRIImporterListElement *)m_ListSelected->Item(tsImageId)->GetData();
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
  
		/*mafVME *cTItem =new mafVME;
	  cTItem->SetData(accumulate->GetOutput());
    cTItem->SetTimeStamp(tsDouble);*/
		accumulate->Update();
	  m_Volume->SetDataByDetaching(accumulate->GetOutput(),tsDouble);

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
	
}
//----------------------------------------------------------------------------
void mmoCTMRIImporter::BuildVolume()
//----------------------------------------------------------------------------
{
	int step;

	if(m_BuildStepValue == 0)
		step = 1;
	else 
		step = m_BuildStepValue << 1;

	int n_slices = m_NumberOfSlices / step;

	wxBusyInfo wait_info("Building volume: please wait");

	vtkMAFSmartPointer<vtkRGSliceAccumulate> accumulate;
	accumulate->SetNumberOfSlices(n_slices);
	accumulate->BuildVolumeOnAxes(m_SortAxes);
	int count,s_count;
	for (count = 0, s_count = 0; count < m_NumberOfSlices; count += step)
	{
		if (s_count == n_slices) {break;}
    ShowSlice(count);
		double b[6];
		m_SliceTexture->GetInput()->GetBounds(b);
    accumulate->SetSlice(s_count, m_SliceTexture->GetInput());
    s_count++;
	}
	
  ImportDicomTags();
  if(m_NumberOfSlices == 1)
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

	  //Nome VME = CTDir + IDStudio
	  wxString name = m_DICOMDir + " - " + m_StudyListbox->GetString(m_StudyListbox->GetSelection());			
	  m_Image->SetName(name.c_str());
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

	  //Nome VME = CTDir + IDStudio
	  wxString name = m_DICOMDir + " - " + m_StudyListbox->GetString(m_StudyListbox->GetSelection());			
	  m_Volume->SetName(name.c_str());
  }
}
//----------------------------------------------------------------------------
void mmoCTMRIImporter::ImportDicomTags()
//----------------------------------------------------------------------------
{
  if (m_TagArray == NULL) 
    mafNEW(m_TagArray);

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
void mmoCTMRIImporter::ShowSlice(int slice_num)
//----------------------------------------------------------------------------
{
	// Description:
	// read the slice number 'slice_num' and generate the texture
  double spacing[3], crop_bounds[6], range[2], loc[3];

  m_DicomReader->SetFileName((char *)m_ListSelected->Item(slice_num)->GetData()->GetFileName());
	m_DicomReader->Modified();
	m_DicomReader->Update();
	
	m_DicomReader->GetSliceLocation(loc);
	//double bounds[6];
  
	//m_DicomReader->GetOutput()->GetBounds(bounds);
	m_DicomReader->GetOutput()->GetBounds(m_DicomBounds);

	// switch from m_DicomReader and v_dicom_probe on m_CropFlag
	if (m_CropFlag) 
	{
		m_DicomReader->GetOutput()->GetSpacing(spacing);
		m_CropPlane->GetOutput()->GetBounds(crop_bounds);
		crop_bounds[4] = m_DicomBounds[4];
		crop_bounds[5] = m_DicomBounds[5];

		if(crop_bounds[1] > m_DicomBounds[1]) 
			crop_bounds[1] = m_DicomBounds[1];
		if(crop_bounds[3] > m_DicomBounds[3]) 
			crop_bounds[3] = m_DicomBounds[3];
        
		double Origin[3];
		m_DicomReader->GetOutput()->GetOrigin(Origin);

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
	
	m_SlicePlane->SetOrigin(m_DicomBounds[0],m_DicomBounds[2],0);
	m_SlicePlane->SetPoint1(m_DicomBounds[1],m_DicomBounds[2],0);
	m_SlicePlane->SetPoint2(m_DicomBounds[0],m_DicomBounds[3],0);
	m_SliceActor->VisibilityOn();
}
//----------------------------------------------------------------------------
void mmoCTMRIImporter::ResetSliders()
//----------------------------------------------------------------------------
{
	m_SliceScanner->SetRange(0,m_NumberOfSlices - 1);
	m_TimeScanner->SetRange(0, m_NumberOfTimeFrames -1);
	m_Gui->Update();
}
//----------------------------------------------------------------------------
void mmoCTMRIImporter::ResetStructure()
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
		((ListCTMRIFiles *)m_StudyListbox->GetClientData(i))->DeleteContents(true);
		((ListCTMRIFiles *)m_StudyListbox->GetClientData(i))->Clear();
	}
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
int compareX(const mmoCTMRIImporterListElement **arg1,const mmoCTMRIImporterListElement **arg2)
//----------------------------------------------------------------------------
{
	// compare the x coordinate of both arguments
	// return:
	double x1 = (*(mmoCTMRIImporterListElement **)arg1)->GetCoordinate(0);
	double x2 = (*(mmoCTMRIImporterListElement **)arg2)->GetCoordinate(0);
	if (x1 > x2)
		return 1;
	if (x1 < x2)
		return -1;
	else
		return 0;
}
//----------------------------------------------------------------------------
int compareY(const mmoCTMRIImporterListElement **arg1,const mmoCTMRIImporterListElement **arg2)
//----------------------------------------------------------------------------
{
	// compare the y coordinate of both arguments
	// return:
	double y1 = (*(mmoCTMRIImporterListElement **)arg1)->GetCoordinate(1);
	double y2 = (*(mmoCTMRIImporterListElement **)arg2)->GetCoordinate(1);
	if (y1 > y2)
		return 1;
	if (y1 < y2)
		return -1;
	else
		return 0;
}
//----------------------------------------------------------------------------
int compareZ(const mmoCTMRIImporterListElement **arg1,const mmoCTMRIImporterListElement **arg2)
//----------------------------------------------------------------------------
{
	// compare the z coordinate of both arguments
	// return:
	double z1 = (*(mmoCTMRIImporterListElement **)arg1)->GetCoordinate(2);
	double z2 = (*(mmoCTMRIImporterListElement **)arg2)->GetCoordinate(2);
	if (z1 > z2)
		return 1;
	if (z1 < z2)
		return -1;
	else
		return 0;
}
//----------------------------------------------------------------------------
int compareTriggerTime(const mmoCTMRIImporterListElement **arg1,const mmoCTMRIImporterListElement **arg2)
//----------------------------------------------------------------------------
{
	// compare the trigger time of both arguments
	// return:
	float t1 = (*(mmoCTMRIImporterListElement **)arg1)->GetTriggerTime();
	float t2 = (*(mmoCTMRIImporterListElement **)arg2)->GetTriggerTime();;
	if (t1 > t2)
		return 1;
	if (t1 < t2)
		return -1;
	else
		return 0;
}

//----------------------------------------------------------------------------
int compareImageNumber(const mmoCTMRIImporterListElement **arg1,const mmoCTMRIImporterListElement **arg2)
//----------------------------------------------------------------------------
{
	// compare the trigger time of both arguments
	// return:
	float i1 = (*(mmoCTMRIImporterListElement **)arg1)->GetImageNumber();
	float i2 = (*(mmoCTMRIImporterListElement **)arg2)->GetImageNumber();;
	if (i1 > i2)
		return 1;
	if (i1 < i2)
		return -1;
	else
		return 0;
}
//----------------------------------------------------------------------------
void mmoCTMRIImporter::	OnEvent(mafEventBase *maf_event) 
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
      case ID_DICTIONARY:
        m_DicomReader->SetDictionaryFileName(m_DictionaryFilename.GetCStr());
        m_Gui->Enable(ID_OPEN_DIR,1);
      break;
      case ID_OPEN_DIR:
        m_Gui->Update();
        ResetStructure();
        // scan dicom directory
        BuildDicomFileList(m_DICOMDir.GetCStr());
        /*if(m_NumberOfStudy == 1)
        {
          m_StudyListbox->SetSelection(0);
          OnEvent(&mafEvent(this, ID_STUDY));
        }*/
      break;
      case ID_STUDY:
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
        int sel = m_StudyListbox->GetSelection();
        m_ListSelected = (ListCTMRIFiles *)m_StudyListbox->GetClientData(sel);
        // sort dicom slices
        if(m_ListSelected->GetCount() > 1)
        {
          double item1_pos[3],item2_pos[3],d[3];
          mmoCTMRIImporterListElement *element1;
          mmoCTMRIImporterListElement *element2;
          element1 = (mmoCTMRIImporterListElement *)m_ListSelected->Item(0)->GetData();
          element2 = (mmoCTMRIImporterListElement *)m_ListSelected->Item(1)->GetData();
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
				m_NumberOfTimeFrames = ((mmoCTMRIImporterListElement *)m_ListSelected->Item(0)->GetData())->GetCardiacNumberOfImages();
        if(m_DICOMType == ID_CMRI) //If cMRI
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

        m_DicomDialog->GetRWI()->CameraReset();
        ResetSliders();
        m_DicomDialog->GetRWI()->CameraUpdate();

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

        m_Gui->Update();
      }
      break;
      case ID_SCAN_SLICE:
				{
					// show the current slice
					int currImageId = GetImageId(m_CurrentTime, m_CurrentSlice);
					if (currImageId != -1) 
					{
						ShowSlice(currImageId);
						m_DicomDialog->GetRWI()->CameraUpdate();
					}
				}
      break;
			case ID_SCAN_TIME:
				{
					// show the current slice
					int currImageId = GetImageId(m_CurrentTime, m_CurrentSlice);
					if (currImageId != -1) 
					{
						ShowSlice(currImageId);
						m_DicomDialog->GetRWI()->CameraUpdate();
					}
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
        m_CropFlag = true;
        ShowSlice(m_CurrentSlice);
        m_DicomDialog->GetRWI()->CameraUpdate();
        m_Gui->Enable(ID_UNDO_CROP_BUTTON,1);
      break;
      case ID_UNDO_CROP_BUTTON:
        m_CropFlag = false;
        ShowSlice(m_CurrentSlice);
        m_DicomDialog->GetRWI()->CameraUpdate();
        m_Gui->Enable(ID_UNDO_CROP_BUTTON,0);
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
          double dx = (b[1] - b[0]) / 10;
          double dy = (b[3] - b[2]) / 10;

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
            //	  6------------5----------4--->x
            //		|												|
            //		7												3
            //		|												|
            //		8------------1----------2
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
        if (m_CropMode == true)
          if (m_GizmoStatus == GIZMO_RESIZING)
            m_GizmoStatus = 	GIZMO_DONE;
          else if (m_GizmoStatus == GIZMO_DONE)
            m_SideToBeDragged = 0;
      break;
			case ID_TYPE_DICOM:
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
				break;
      default:
        mafEventMacro(*e);
      break;
    }	
  }
}
//----------------------------------------------------------------------------
int mmoCTMRIImporter::GetImageId(int timeId, int heigthId)
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

  m_ListSelected = (ListCTMRIFiles *)m_StudyListbox->GetClientData(m_StudyListbox->GetSelection());
  
  mmoCTMRIImporterListElement *element0;
  element0 = (mmoCTMRIImporterListElement *)m_ListSelected->Item(0)->GetData();
  
  int cardiacNumberOfImages =  element0->GetCardiacNumberOfImages();

  int numberOfDicomSlices = m_ListSelected->GetCount();
    
  int numSlicesPerTS = numberOfDicomSlices / cardiacNumberOfImages;
  assert(numberOfDicomSlices % cardiacNumberOfImages == 0);

  int maxHeigthId = numSlicesPerTS - 1; // 
  int maxTimeId = cardiacNumberOfImages - 1; // cardiacNumberOfImages - 1;

  if (heigthId < 0 || heigthId > maxHeigthId || timeId < 0 || timeId > maxTimeId )
  {
    return -1;
  }

  return (heigthId * cardiacNumberOfImages + timeId); 
}
//----------------------------------------------------------------------------
void mmoCTMRIImporter::WaitUser()
//----------------------------------------------------------------------------
{
	m_DicomDialog->ShowModal();
	int res = (m_DicomDialog->GetReturnCode() == wxID_OK) ? OP_RUN_OK : OP_RUN_CANCEL;

	OpStop(res);
	return;
}
//----------------------------------------------------------------------------
void mmoCTMRIImporter::SetDirectory(mafString dir)
//----------------------------------------------------------------------------
{
	m_DICOMDir=dir;
}
//----------------------------------------------------------------------------
void mmoCTMRIImporter::SetDictonary(mafString dic)
//----------------------------------------------------------------------------
{
	m_DictionaryFilename=dic;
}
//----------------------------------------------------------------------------
void mmoCTMRIImporter::SetModality(mafString modality)
//----------------------------------------------------------------------------
{
	if (modality.Equals("CT"))
	{
		m_DICOM = 0;
	}
	else if (modality.Equals("MRI"))
	{
		m_DICOM = 1;
	}
}
//----------------------------------------------------------------------------
void mmoCTMRIImporter::SetParameters(void *param)
//----------------------------------------------------------------------------
{
	if(param)
	{
		m_AutoLoad = true;
		mafString *settings=(mafString*)param;
		SetModality(settings[0]);
		SetDirectory(settings[1]);
		if(m_Gui)
			m_Gui->Update();
	}
}